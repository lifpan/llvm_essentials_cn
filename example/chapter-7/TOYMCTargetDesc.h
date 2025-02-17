#ifndef TOYMCTARGETDESC_H
#define TOYMCTARGETDESC_H
#include "llvm/Support/DataTypes.h"
namespace llvm {
class Target;
class MCInstrInfo;
class MCRegisterInfo;
class MCSubtargetInfo;
class MCContext;
class MCCodeEmitter;
class MCAsmInfo;
class MCCodeGenInfo;
class MCInstPrinter;
class MCObjectWriter;
class MCAsmBackend;
class StringRef;
class raw_ostream;
extern Target TheTOYTarget;

MCCodeEmitter *createTOYMCCodeEmitter(const MCInstrInfo &MCII,
  const MCRegisterInfo &MRI, const MCSubtargetInfo &STI, MCContext &Ctx);
MCAsmBackend *createTOYAsmBackend(const Target &T,
  const MCRegisterInfo &MRI, StringRef TT, StringRef CPU);
MCObjectWriter *createTOYELFObjectWriter(raw_ostream &OS, uint8_t OSABI);
} // End llvm namespace
#define GET_REGINFO_ENUM
#include "TOYGenInstrInfo.inc"
#define GET_SUBTARGETINFO_ENUM
#include "TOYGenSubtargetInfo.inc"
#endif