//===--- ParseTemplate.cpp - Template Parsing -----------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file implements parsing of C++ templates.
//
//===----------------------------------------------------------------------===//

#include "clang/AST/ASTContext.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/Parse/Parser.h"
#include "clang/Parse/RAIIObjectsForParser.h"
#include "clang/Sema/DeclSpec.h"
#include "clang/Sema/Scope.h"
#include "clang/Lex/PropertyDirectiveInfo.h"
using namespace clang;

/// \brief Handle the annotation token produced for
/// #property ...
void Parser::handleDirectiveProperty(ParsedAttributesWithRange &attrs) {
  const auto Info = reinterpret_cast<PropertyDirectiveInfo *>(Tok.getAnnotationValue());

  std::string Annotation = R"({"kind": "mql-property", "args": [)";
  for (auto PropTok = Info->PropertyNameAndArgumentTokens.begin();
       Info->PropertyNameAndArgumentTokens.end() != PropTok;
       ++PropTok) {
    if (Info->PropertyNameAndArgumentTokens.begin() == PropTok) {
      // The first token is the property's name
      assert(PropTok->is(tok::identifier) && "#property's name must be an indentifier");
      Annotation += "\"";
      Annotation += PropTok->getIdentifierInfo()->getName();
      Annotation += "\"";
    } else if (PropTok->isLiteral()) {
      // We reached the property's arguments
      Annotation += ", ";
      
      auto LiteralLength = PropTok->getLength();

      const auto IsStringLiteral = isStringLiteral(PropTok->getKind());
      if (IsStringLiteral) {
        --LiteralLength;
        Annotation += "\"\\";
      }

      Annotation += StringRef(PropTok->getLiteralData(), LiteralLength);

      if (IsStringLiteral)
        Annotation += "\\\"\"";
    }
  }
  Annotation += "]}";

  QualType CharTyConst = Actions.Context.CharTy;
  // A C++ string literal has a const-qualified element type (C++ 2.13.4p1).
  if (getLangOpts().CPlusPlus || getLangOpts().ConstStrings)
    CharTyConst.addConst();

  const auto CharByteWidth = PP.getTargetInfo().getCharWidth() / 8;
  const auto NumStringChars = Annotation.size() / CharByteWidth;

  // Get an array type for the string, according to C99 6.4.5.  This includes
  // the nul terminator character as well as the string length for pascal
  // strings.
  const QualType StrTy = Actions.Context.getConstantArrayType(CharTyConst,
                                 llvm::APInt(32, NumStringChars+1),
                                 ArrayType::Normal, 0);

  SmallVector<SourceLocation, 1> StringTokLocs = { SourceLocation() };

  // Pass &StringTokLocs[0], StringTokLocs.size() to factory!
  auto * Lit = StringLiteral::Create(Actions.Context, Annotation,
                                     StringLiteral::Ascii, /*Pascal=*/false, StrTy,
                                     &StringTokLocs[0], StringTokLocs.size());
  ArgsVector ArgExprs = { Lit };

  auto AttrName = PP.getIdentifierInfo("annotate");
  attrs.addNew(AttrName, SourceRange(Tok.getLocation(), Tok.getAnnotationEndLoc()), 
               /*ScopeName=*/nullptr, /*ScopeLoc=*/SourceLocation(),
               /*args=*/ArgExprs.data(), /*numArgs=*/ArgExprs.size(), AttributeList::AS_GNU);

  // Either a C++11 empty-declaration or attribute-declaration.
  //SingleDecl = Actions.ActOnEmptyDeclaration(getCurScope(),
  //                                           attrs.getList(),
  //                                           Tok.getLocation());
  auto & ASTContext = Actions.Context;
  ASTContext.getTranslationUnitDecl()->addAttr(
    ::new (ASTContext) AnnotateAttr(attrs.getList()->getRange(), 
                                    ASTContext, Annotation,
                                    attrs.getList()->getAttributeSpellingListIndex()));
  ConsumeAnnotationToken();
}

/// \brief Lex an MQL function for late parsing.
void Parser::lexFunctionForLateParsing(CachedTokens &BodyToks) {
  tok::TokenKind kind = Tok.getKind();
  if (!ConsumeAndStoreFunctionPrologue(BodyToks)) {
    // Consume everything up to (and including) the matching right brace.
    ConsumeAndStoreUntil(tok::r_brace, BodyToks, /*StopAtSemi=*/false);
  }

  // If we're in a function-try-block, we need to store all the catch blocks.
  if (kind == tok::kw_try) {
    while (Tok.is(tok::kw_catch)) {
      ConsumeAndStoreUntil(tok::l_brace, BodyToks, /*StopAtSemi=*/false);
      ConsumeAndStoreUntil(tok::r_brace, BodyToks, /*StopAtSemi=*/false);
    }
  }
}

void Parser::lateFunctionParserCallback(void *OpaqueParser, 
                                        LateParsedFunction &LateParsedFunc) {
  static_cast<Parser *>(OpaqueParser)->parseLateFuncDef(LateParsedFunc);
}

/// \brief Late parse a function in MQL mode.
void Parser::parseLateFuncDef(LateParsedFunction &LateParsedFunc) {
  if (!LateParsedFunc.Decl)
     return;

  // Get the FunctionDecl.
  FunctionDecl *FunctionDecl = LateParsedFunc.Decl->getAsFunction();
  // Track template parameter depth.
  TemplateParameterDepthRAII CurTemplateDepthTracker(TemplateParameterDepth);

  // To restore the context after late parsing.
  Sema::ContextRAII GlobalSavedContext(
      Actions, Actions.Context.getTranslationUnitDecl());

  SmallVector<ParseScope*, 4> TemplateParamScopeStack;

  // Get the list of DeclContexts to reenter.
  SmallVector<DeclContext*, 4> DeclContextsToReenter;
  DeclContext *DD = FunctionDecl;
  while (DD && !DD->isTranslationUnit()) {
    DeclContextsToReenter.push_back(DD);
    DD = DD->getLexicalParent();
  }

  // Reenter template scopes from outermost to innermost.
  SmallVectorImpl<DeclContext *>::reverse_iterator II =
      DeclContextsToReenter.rbegin();
  for (; II != DeclContextsToReenter.rend(); ++II) {
    TemplateParamScopeStack.push_back(new ParseScope(this,
          Scope::TemplateParamScope));
    unsigned NumParamLists =
      Actions.ActOnReenterTemplateScope(getCurScope(), cast<Decl>(*II));
    CurTemplateDepthTracker.addDepth(NumParamLists);
    if (*II != FunctionDecl) {
      TemplateParamScopeStack.push_back(new ParseScope(this, Scope::DeclScope));
      Actions.PushDeclContext(Actions.getCurScope(), *II);
    }
  }

  assert(!LateParsedFunc.BodyToks.empty() && "Empty body!");

  // Append the current token at the end of the new token stream so that it
  // doesn't get lost.
  LateParsedFunc.BodyToks.push_back(Tok);
  PP.EnterTokenStream(LateParsedFunc.BodyToks, true);

  // Consume the previously pushed token.
  ConsumeAnyToken(/*ConsumeCodeCompletionTok=*/true);
  assert(Tok.isOneOf(tok::l_brace, tok::colon, tok::kw_try) &&
         "Inline method not starting with '{', ':' or 'try'");

  // Parse the method body. Function body parsing code is similar enough
  // to be re-used for method bodies as well.
  ParseScope FnScope(this, Scope::FnScope|Scope::DeclScope);

  // Recreate the containing function DeclContext.
  Sema::ContextRAII FunctionSavedContext(Actions,
                                         Actions.getContainingDC(FunctionDecl));

  Actions.ActOnStartOfFunctionDef(getCurScope(), FunctionDecl);

  if (Tok.is(tok::kw_try)) {
    ParseFunctionTryBlock(LateParsedFunc.Decl, FnScope);
  } else {
    if (Tok.is(tok::colon))
      ParseConstructorInitializer(LateParsedFunc.Decl);
    else
      Actions.ActOnDefaultCtorInitializers(LateParsedFunc.Decl);

    if (Tok.is(tok::l_brace)) {
      assert((!isa<FunctionTemplateDecl>(LateParsedFunc.Decl) ||
              cast<FunctionTemplateDecl>(LateParsedFunc.Decl)
                      ->getTemplateParameters()
                      ->getDepth() == TemplateParameterDepth - 1) &&
             "TemplateParameterDepth should be greater than the depth of "
             "current template being instantiated!");
      ParseFunctionStatementBody(LateParsedFunc.Decl, FnScope);
      Actions.UnmarkAsLateParsedTemplate(FunctionDecl);
    } else
      Actions.ActOnFinishFunctionBody(LateParsedFunc.Decl, nullptr);
  }

  // Exit scopes.
  FnScope.Exit();
  SmallVectorImpl<ParseScope *>::reverse_iterator I =
   TemplateParamScopeStack.rbegin();
  for (; I != TemplateParamScopeStack.rend(); ++I)
    delete *I;
}
