; ModuleID = 'intrinsic.cpp'
target datalayout = "e-p:32:32-p1:64:64-p2:64:64-p3:32:32-p4:64:64-p5:32:32-p24:64:64-i64:64-v16:16-v24:32-v32:32-v48:64-v96:128-v192:256-v256:256-v512:512-v1024:1024-v2048:2048-n32:64"
target triple = "amdgcn"

; Function Attrs: nounwind
define i32 @_Z4funcv() #0 {
  %a = alloca [5 x i32], align 4
  %i = alloca i32, align 4
  store i32 0, i32* %i, align 4
  br label %1

; <label>:1                                       ; preds = %7, %0
  %2 = load i32, i32* %i, align 4
  %3 = icmp ne i32 %2, 5
  br i1 %3, label %4, label %10

; <label>:4                                       ; preds = %1
  %5 = load i32, i32* %i, align 4
  %6 = getelementptr inbounds [5 x i32], [5 x i32]* %a, i32 0, i32 %5
  store i32 0, i32* %6, align 4
  br label %7

; <label>:7                                       ; preds = %4
  %8 = load i32, i32* %i, align 4
  %9 = add nsw i32 %8, 1
  store i32 %9, i32* %i, align 4
  br label %1

; <label>:10                                      ; preds = %1
  %11 = getelementptr inbounds [5 x i32], [5 x i32]* %a, i32 0, i32 0
  %12 = load i32, i32* %11, align 4
  ret i32 %12
}

attributes #0 = { nounwind "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = !{!"clang version 3.8.1-15 (tags/RELEASE_381/final)"}
