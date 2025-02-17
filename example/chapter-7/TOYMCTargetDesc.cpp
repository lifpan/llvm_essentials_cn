#include "TOYMCTargetDesc.h"
#include "InstPrinter/TOYInstPrinter.h"
#include "TOYMCAsmInfo.h"
#include "llvm/MC/MCCodeGenInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/TargetRegistry.h"
#define GET_INSTRINFO_MC_DESC
#include "TOYGenInstrInfo.inc"
#define GET_SUBTARGETINFO_MC_DESC
#include "TOYGenSubtargetInfo.inc"
#define GET_REGINFO_MC_DESC
#include "TOYGenRegisterInfo.inc"
using namespace llvm;

static MCInstrInfo *createTOYMCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitTOYMCInstrInfo(X);
  return X;
}

static MCRegisterInfo *createTOYMCRegisterInfo(StringRef TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitTOYMCRegisterInfo(X, TOY::LR);
  return X;
}

static MCSubtargetInfo *createTOYMCSubtargetInfo(StringRef TT,
  StringRef CPU, StringRef FS) {
  MCSubtargetInfo *X = new MCSubtargetInfo();
  InitTOYMCSubtargetInfo(X, TT, CPU, FS);
  return X;
}

static MCAsmInfo *createTOYMCAsmInfo(const MCRegisterInfo &MRI, StringRef TT) {
  MCAsmInfo *MAI = new TOYMCAsmInfo(TT);
  return MAI;
}

static MCCodeGenInfo *createTOYMCCodeGenInfo(StringRef TT, Reloc::Model RM,
  CodeModel::Model CM, CodeGenOpt::Level OL) {
  MCCodeGenInfo *X = new MCCodeGenInfo();
  if (RM == Reloc::Default) {
    RM = Reloc::Static;
  }
  if (CM == CodeModel::Default) {
    CM = CodeModel::Small;
  }
  if (CM != CodeModel::Small && CM != CodeModel::Large) {
    report_fatal_error("Target only supports CodeModel Small or Large");
  }
  X->initMCCodeGenInfo(RM, CM, OL);
  return X;
}

static MCInstPrinter *createTOYMCInstPrinter(const Target &T, unsigned SyntaxVariant,
  const MCAsmInfo &MAI, const MCInstrInfo &MII, const MCRegisterInfo &MRI,
  const MCSubtargetInfo &STI) {
  return new TOYInstPrinter(MAI, MII, MRI);
}

static MCStreamer *createMCAsmStreamer(MCContext &Ctx, formatted_raw_ostream &OS,
  bool isVerboseAsm, bool useDwarfDirectory, MCInstPrinter *InstPrint,
  MCCodeEmitter *CE, MCAsmBackend *TAB, bool ShowInst) {
  return createAsmStreamer(Ctx, OS, isVerboseAsm,
  useDwarfDirectory, InstPrint, CE, TAB, ShowInst);
}

static MCStreamer *createMCStreamer(const Target &T, StringRef TT,
  MCContext &Ctx, MCAsmBackend &MAB, raw_ostream &OS, MCCodeEmitter *Emitter,
  const MCSubtargetInfo &STI, bool RelaxAll, bool NoExecStack) {
  return createELFStreamer(Ctx, MAB, OS, Emitter, false, NoExecStack);
}

// Force static initialization.
extern "C" void LLVMInitializeTOYTargetMC() {
  // Register the MC asm info.
  RegisterMCAsmInfoFn X(TheTOYTarget, createTOYMCAsmInfo);
  // Register the MC codegen info.
  TargetRegistry::RegisterMCCodeGenInfo(TheTOYTarget, createTOYMCCodeGenInfo);
  // Register the MC instruction info.
  TargetRegistry::RegisterMCInstrInfo(TheTOYTarget, createTOYMCInstrInfo);
  // Register the MC register info.
  TargetRegistry::RegisterMCRegInfo(TheTOYTarget, createTOYMCRegisterInfo);
  // Register the MC subtarget info.
  TargetRegistry::RegisterMCSubtargetInfo(TheTOYTarget, createTOYMCSubtargetInfo);
  // Register the MCInstPrinter
  TargetRegistry::RegisterMCInstPrinter(TheTOYTarget, createTOYMCInstPrinter);
  // Register the ASM Backend.
  TargetRegistry::RegisterMCAsmBackend(TheTOYTarget, createTOYAsmBackend);
  // Register the assembly streamer.
  TargetRegistry::RegisterAsmStreamer(TheTOYTarget, createMCAsmStreamer);
  // Register the object streamer.
  TargetRegistry::RegisterMCObjectStreamer(TheTOYTarget, createMCStreamer);
  // Register the MCCodeEmitter
  TargetRegistry::RegisterMCCodeEmitter(TheTOYTarget, createTOYMCCodeEmitter);
}