#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include <vector>
using namespace llvm;

static LLVMContext Context;
static Module *ModuleOb = new Module("my compiler", Context);

Function *createFunc(IRBuilder<> &Builder, std::string Name) {
    FunctionType *funcType = llvm::FunctionType::get(Builder.getInt32Ty(), false);
    Function *fooFunc = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, Name, ModuleOb);
    return fooFunc;
}

BasicBlock *createBB(Function *fooFunc, std::string Name) {
    return BasicBlock::Create(Context, Name, fooFunc);
}

int main(int argc, char *argv[]) {
    static IRBuilder<> Builder(Context);
    Function *fooFunc = createFunc(Builder, "foo");
    BasicBlock *entry = createBB(fooFunc, "entry");
    Builder.SetInsertPoint(entry);
    verifyFunction(*fooFunc);
    ModuleOb->dump();
    return 0;
}

/*
; ModuleID = 'my compiler'
source_filename = "my compiler"

define i32 @foo() {
entry:
}
/*