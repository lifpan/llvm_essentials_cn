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
    Type *u32Ty = Type::getInt32Ty(Context);
    Type *vecTy = VectorType::get(u32Ty, 4);
    FunctionType *funcType = FunctionType::get(Builder.getInt32Ty(), vecTy, false);
    Function *fooFunc = Function::Create(funcType, Function::ExternalLinkage, Name, ModuleOb);
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

Value *createArith(IRBuilder<> &Builder, Value *L, Value *R) {
    return Builder.CreateAdd(L, R, "add");
}

Value *getExtractElement(IRBuilder<> &Builder, Value *Vec, Value *Index) {
    return Builder.CreateExtractElement(Vec, Index);
}

int main(int argc, char *argv[]) {
    FunArgs.push_back("a");
    static IRBuilder<> Builder(Context);
    Function *fooFunc = createFunc(Builder, "foo");
    setFuncArgs(fooFunc, FunArgs);

    BasicBlock *entry = createBB(fooFunc, "entry");
    Builder.SetInsertPoint(entry);

    Value *Vec = dyn_cast<Value>(fooFunc->arg_begin());
    SmallVector<Value *, 4> V;
    for (unsigned int i = 0; i < 4; i++) {
        Value *Val = getExtractElement(Builder, Vec, Builder.getInt32(i));
        V.push_back(Val);
    }
    Value *add1 = createArith(Builder, V[0], V[1]);
    Value *add2 = createArith(Builder, add1, V[2]);
    Value *add = createArith(Builder, add2, V[3]);

    Builder.CreateRet(add);
    verifyFunction(*fooFunc);
    ModuleOb->dump();
    return 0;
}

/*
; ModuleID = 'my compiler'
source_filename = "my compiler"

define i32 @foo(<4 x i32> %a) {
entry:
  %0 = extractelement <4 x i32> %a, i32 0
  %1 = extractelement <4 x i32> %a, i32 1
  %2 = extractelement <4 x i32> %a, i32 2
  %3 = extractelement <4 x i32> %a, i32 3
  %add = add i32 %0, %1
  %add1 = add i32 %add, %2
  %add2 = add i32 %add1, %3
  ret i32 %add2
}
*/