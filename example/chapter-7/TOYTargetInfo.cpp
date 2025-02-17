#include "TOY.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;
Target llvm::TheTOYTarget;
extern "C" void
LLVMInitializeTOYTargetInfo() {
  RegisterTarget<Triple::toy> X(TheTOYTarget, "toy", "TOY");
}