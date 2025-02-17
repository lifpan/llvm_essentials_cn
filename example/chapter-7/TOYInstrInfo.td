def ADDrr : InstTOY<outs GRRegs: $dst),
  (ins GRRegs:$src1, GRRegs:$src2),
  "add $dst, $src1, z$src2",
  [(set i32:$dst, (add i32:$src1, i32:$src2))]>;