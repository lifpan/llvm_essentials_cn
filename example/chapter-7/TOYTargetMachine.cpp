#include "TOYTargetMachine.h"
#include "TOY.h"
#include "TOYFrameLowering.h"
#include "TOYInstrInfo.h"
#include "TOYISelLowering.h"
#include "TOYSelectionDAGInfo.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/IR/Module.h"
#include "llvm/PassManager.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

TOYTargetMachine::TOYTargetMachine(const Target &T, StringRef TT,
    StringRef CPU, StringRef FS, const TargetOptions &Options,
    Reloc::Model RM, CodeModel::Model CM, CodeGenOpt::Level OL)
    : LLVMTargetMachine(T, TT, CPU, FS, Options, RM, CM, OL),
      Subtarget(TT, CPU, FS, *this) {
  initAsmInfo();
}

namespace {
  class TOYPassConfig : public TargetPassConfig {
  public:
    TOYPassConfig(TOYTargetMachine *TM, PassManagerBase &PM) : TargetPassConfig(TM, PM) {}
    TOYTargetMachine &getTOYTargetMachine() const {
      return getTM<TOYTargetMachine>();
    }
    virtual bool addPreISel();
    virtual bool addInstSelector();
    virtual bool addPreEmitPass();
  };
} // namespace

TargetPassConfig *TOYTargetMachine::createPassConfig(PassManagerBase &PM) {
  return new TOYPassConfig(this, PM);
}

bool TOYPassConfig::addPreISel() {
  return false;
}

bool TOYPassConfig::addInstSelector() {
  addPass(createTOYISelDag(getTOYTargetMachine(), getOptLevel()));
  return false;
}

bool TOYPassConfig::addPreEmitPass() {
  return false;
}

// Force static initialization.
extern "C" void LLVMInitializeTOYTarget() {
  RegisterTargetMachine<TOYTargetMachine> X(TheTOYTarget);
}

void TOYTargetMachine::addAnalysisPasses(PassManagerBase &PM) {}