; ModuleID = 'addsub.cpp'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@a = global [4 x i32] zeroinitializer, align 16
@b = global [4 x i32] zeroinitializer, align 16
@c = global [4 x i32] zeroinitializer, align 16

; Function Attrs: nounwind uwtable
define void @addsub() #0 {
  %1 = load i32, i32* getelementptr inbounds ([4 x i32], [4 x i32]* @b, i64 0, i64 0), align 16
  %2 = load i32, i32* getelementptr inbounds ([4 x i32], [4 x i32]* @c, i64 0, i64 0), align 16
  %3 = add nsw i32 %1, %2
  store i32 %3, i32* getelementptr inbounds ([4 x i32], [4 x i32]* @a, i64 0, i64 0), align 16
  %4 = load i32, i32* getelementptr inbounds ([4 x i32], [4 x i32]* @b, i64 0, i64 1), align 4
  %5 = load i32, i32* getelementptr inbounds ([4 x i32], [4 x i32]* @c, i64 0, i64 1), align 4
  %6 = add nsw i32 %4, %5
  store i32 %6, i32* getelementptr inbounds ([4 x i32], [4 x i32]* @a, i64 0, i64 1), align 4
  %7 = load i32, i32* getelementptr inbounds ([4 x i32], [4 x i32]* @b, i64 0, i64 2), align 8
  %8 = load i32, i32* getelementptr inbounds ([4 x i32], [4 x i32]* @c, i64 0, i64 2), align 8
  %9 = add nsw i32 %7, %8
  store i32 %9, i32* getelementptr inbounds ([4 x i32], [4 x i32]* @a, i64 0, i64 2), align 8
  %10 = load i32, i32* getelementptr inbounds ([4 x i32], [4 x i32]* @b, i64 0, i64 3), align 4
  %11 = load i32, i32* getelementptr inbounds ([4 x i32], [4 x i32]* @c, i64 0, i64 3), align 4
  %12 = add nsw i32 %10, %11
  store i32 %12, i32* getelementptr inbounds ([4 x i32], [4 x i32]* @a, i64 0, i64 3), align 4
  ret void
}

attributes #0 = { nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = !{!"clang version 3.8.1-15 (tags/RELEASE_381/final)"}
