; ModuleID = 'test.ll'
source_filename = "test.ll"

; Function Attrs: norecurse nounwind readonly
define internal fastcc i32 @test(i32* nocapture readonly %X, i32* nocapture readonly %Y) unnamed_addr #0 {
  %A = load i32, i32* %X, align 4
  %B = load i32, i32* %Y, align 4
  %C = add i32 %B, %A
  ret i32 %C
}

; Function Attrs: norecurse nounwind readonly
define internal fastcc i32 @caller(i32* nocapture readonly %B) unnamed_addr #0 {
  %A = alloca i32, align 4
  store i32 1, i32* %A, align 4
  %C = call fastcc i32 @test(i32* nonnull %A, i32* %B)
  ret i32 %C
}

; Function Attrs: norecurse nounwind readonly
define i32 @callercaller() local_unnamed_addr #0 {
  %B = alloca i32, align 4
  store i32 2, i32* %B, align 4
  %X = call fastcc i32 @caller(i32* nonnull %B)
  ret i32 %X
}

attributes #0 = { norecurse nounwind readonly }
