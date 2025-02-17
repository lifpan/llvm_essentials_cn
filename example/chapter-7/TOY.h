#ifndef TARGET_TOY_H
#define TARGET_TOY_H
#include "MCTargetDesc/TOYMCTargetDesc.h"
#include "llvm/Target/TargetMachine.h"
namespace llvm {
  class TargetMachine;
  class TOYTargetMachine;
  FunctionPass *createTOYISelDag(TOYTargetMachine &TM, CodeGenOpt::Level OptLevel);
} // end namespace llvm;
#endif