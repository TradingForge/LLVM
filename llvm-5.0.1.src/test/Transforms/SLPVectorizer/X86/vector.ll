; NOTE: Assertions have been autogenerated by utils/update_test_checks.py
; RUN: opt < %s -slp-vectorizer -S -mtriple=x86_64-apple-macosx10.8.0 -mcpu=corei7-avx | FileCheck %s

target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.8.0"

; Make sure that we are not crashing or changing the code.
define void @test(<4 x i32> %in, <4 x i32> %in2) {
; CHECK-LABEL: @test(
; CHECK-NEXT:    [[K:%.*]] = icmp eq <4 x i32> [[IN:%.*]], [[IN2:%.*]]
; CHECK-NEXT:    ret void
;
  %k = icmp eq <4 x i32> %in, %in2
  ret void
}

define i1 @cmpv2f32(<2 x i32> %x, <2 x i32> %y) {
; CHECK-LABEL: @cmpv2f32(
; CHECK-NEXT:  entry:
; CHECK-NEXT:    [[X0:%.*]] = extractelement <2 x i32> [[X:%.*]], i32 0
; CHECK-NEXT:    [[Y0:%.*]] = extractelement <2 x i32> [[Y:%.*]], i32 0
; CHECK-NEXT:    [[CMP0:%.*]] = icmp eq i32 [[X0]], [[Y0]]
; CHECK-NEXT:    br i1 [[CMP0]], label [[IF:%.*]], label [[ENDIF:%.*]]
; CHECK:       if:
; CHECK-NEXT:    [[X1:%.*]] = extractelement <2 x i32> [[X]], i32 1
; CHECK-NEXT:    [[Y1:%.*]] = extractelement <2 x i32> [[Y]], i32 1
; CHECK-NEXT:    [[CMP1:%.*]] = icmp eq i32 [[X1]], [[Y1]]
; CHECK-NEXT:    br label [[ENDIF]]
; CHECK:       endif:
; CHECK-NEXT:    [[AND_OF_CMPS:%.*]] = phi i1 [ false, [[ENTRY:%.*]] ], [ [[CMP1]], [[IF]] ]
; CHECK-NEXT:    ret i1 [[AND_OF_CMPS]]
;
  entry:
  %x0 = extractelement <2 x i32> %x, i32 0
  %y0 = extractelement <2 x i32> %y, i32 0
  %cmp0 = icmp eq i32 %x0, %y0
  br i1 %cmp0, label %if, label %endif

  if:
  %x1 = extractelement <2 x i32> %x, i32 1
  %y1 = extractelement <2 x i32> %y, i32 1
  %cmp1 = icmp eq i32 %x1, %y1
  br label %endif

  endif:
  %and_of_cmps = phi i1 [ false, %entry ], [ %cmp1, %if ]
  ret i1 %and_of_cmps
}

