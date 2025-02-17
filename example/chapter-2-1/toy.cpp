#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
using namespace llvm;

static LLVMContext Context;
static Module *ModuleOb = new Module("my compiler", Context);

int main(int argc, char *argv[]) {
    ModuleOb->dump();
    return 0;
}

/*
; ModuleID = 'my compiler'
source_filename = "my compiler"
/*