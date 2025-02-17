#ifndef TOYTARGETASMINFO_H
#define TOYTARGETASMINFO_H
#include "llvm/MC/MCAsmInfoELF.h"
namespace llvm {
class StringRef;
class Target;
class TOYMCAsmInfo : public MCAsmInfoELF {
  virtual void anchor();

public:
  explicit TOYMCAsmInfo(StringRef TT);
}
} // namespace llvm
#endif