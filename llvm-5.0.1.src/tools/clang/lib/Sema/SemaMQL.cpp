//===--- SemaTemplateInstantiateDecl.cpp - C++ Template Decl Instantiation ===/
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//===----------------------------------------------------------------------===/
//
//  This file implements MQL functions late parsing.
//
//===----------------------------------------------------------------------===/
#include "clang/Sema/SemaInternal.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/DeclVisitor.h"
#include "clang/Sema/Initialization.h"
#include "clang/Sema/Template.h"

using namespace clang;

void Sema::markAsLateParsedFunction(FunctionDecl *FunctionDecl, 
                                    Decl *Decl,
                                    CachedTokens &Toks) {
  if (!FunctionDecl)
    return;

  auto LateParsedFunc = llvm::make_unique<LateParsedFunction>();

  // Take tokens to avoid allocations
  LateParsedFunc->BodyToks.swap(Toks);
  LateParsedFunc->Decl = Decl;
  LateParsedFunctions.push_back(std::move(LateParsedFunc));

  FunctionDecl->setWillHaveBody(true);
}


/// \brief Performs parsing of all the functions lexed in TU
void Sema::performLateFunctionsParsing() {
  assert(LangOpts.MQL && 
    "performLateFunctionsParsing must be called only in MQL mode");

  assert(LateFunctionParser && 
    "LateFunctionParser is not supplied");

  for (auto & LateParsedFunction : LateParsedFunctions) {
    LateFunctionParser(OpaqueLateFunctionParser, *LateParsedFunction);
  }
}
