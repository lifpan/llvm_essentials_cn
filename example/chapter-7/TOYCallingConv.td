def RetCC_TOY : CallingConv<[
  CCIfType<[i32], CCAssignToReg<[R0]>>,
  CCIfType<[i32], CCAssignToStack<4, 4>>]>;

def CC_TOY : CallingConv<[
  CCIfType<[i8, i16], CCPromoteToType<i32>>,
  CCIfType<[i32], CCAssignToReg<[R0, R1]>>,
  CCIfType<[i32], CCAssignToStack<4, 4>>]>;

def CC_Save : CalleeSavedRegs<(add R2, R3)>;