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
using namespace clang;

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
