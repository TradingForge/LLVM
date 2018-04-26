//===--- MacroInfo.h - Information about #defined identifiers ---*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Defines the clang::MacroInfo and clang::MacroDirective classes.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_LEX_PROPERTYDIRECTIVEINFO_H
#define LLVM_CLANG_LEX_PROPERTYDIRECTIVEINFO_H

#include "llvm/ADT/ArrayRef.h"
#include "clang/Lex/Token.h"


namespace clang {
/// \brief Encapsulates the data about a property directive (e.g. the property's name and arguments).
///
/// There's an instance of this class for every #property.
struct PropertyDirectiveInfo {
  llvm::ArrayRef<Token> PropertyNameAndArgumentTokens;
  
  PropertyDirectiveInfo() {}
};
}

#endif
