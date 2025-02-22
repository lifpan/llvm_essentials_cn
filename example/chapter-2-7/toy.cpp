#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include <vector>
using namespace llvm;

static LLVMContext Context;
static Module *ModuleOb = new Module("my compiler", Context);
static std::vector <std::string> FunArgs;

Function *createFunc(IRBuilder<> &Builder, std::string Name) {
    std::vector<Type *> Integers(FunArgs.size(), Type::getInt32Ty(Context));
    FunctionType *funcType = llvm::FunctionType::get(Builder.getInt32Ty(), Integers, false);
    Function *fooFunc = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, Name, ModuleOb);
    return fooFunc;
}

void setFuncArgs(Function *fooFunc, std::vector<std::string> FunArgs) {
    unsigned Idx = 0;
    Function::arg_iterator AI, AE;
    for (AI = fooFunc->arg_begin(), AE = fooFunc->arg_end(); AI != AE; ++AI, ++Idx) {
        AI->setName(FunArgs[Idx]);
    }
}

BasicBlock *createBB(Function *fooFunc, std::string Name) {
    return BasicBlock::Create(Context, Name, fooFunc);
}

GlobalVariable *createGlob(IRBuilder<> &Builder, std::string Name) {
    ModuleOb->getOrInsertGlobal(Name, Builder.getInt32Ty());
    GlobalVariable *gVar = ModuleOb->getNamedGlobal(Name);
    gVar->setLinkage(GlobalValue::CommonLinkage);
    gVar->setAlignment(4);
    return gVar;
}

Value *createArith(IRBuilder<> &Builder, Value *L, Value *R) {
    return Builder.CreateMul(L, R, "multmp");
}

int main(int argc, char *argv[]) {
    FunArgs.push_back("a");
    FunArgs.push_back("b");
    static IRBuilder<> Builder(Context);
    GlobalVariable *gVar = createGlob(Builder, "x");
    Function *fooFunc = createFunc(Builder, "foo");
    setFuncArgs(fooFunc, FunArgs);
    BasicBlock *entry = createBB(fooFunc, "entry");
    Builder.SetInsertPoint(entry);
    Value *Arg1 = dyn_cast<Value>(fooFunc->arg_begin());
    Value *constant = Builder.getInt32(16);
    Value *val = createArith(Builder, Arg1, constant);
    Builder.CreateRet(val);

    verifyFunction(*fooFunc);
    ModuleOb->dump();
    return 0;
}

/*
; ModuleID = 'my compiler'
source_filename = "my compiler"

@x = common global i32, align 4

define i32 @foo(i32 %a, i32 %b) {
entry:
  %multmp = mul i32 %a, 16
  ret i32 %multmp
}
*/