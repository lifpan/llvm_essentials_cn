; ModuleID = 'test.ll'
source_filename = "test.ll"

; Function Attrs: norecurse nounwind readnone
define i32 @callercaller() local_unnamed_addr #0 {
  ret i32 3
}

attributes #0 = { norecurse nounwind readnone }
