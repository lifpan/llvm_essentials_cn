#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include <vector>
using namespace llvm;

typedef SmallVector<BasicBlock *, 16> BBList;
typedef SmallVector<Value *, 16> ValList;

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

Value *createLoop(IRBuilder<> &Builder, BBList List, ValList VL, Value *StartVal, Value *EndVal) {
    BasicBlock *PreheaderBB = Builder.GetInsertBlock();
    Value *val = VL[0];
    BasicBlock *LoopBB = List[0];
    Builder.CreateBr(LoopBB);
    Builder.SetInsertPoint(LoopBB);
    PHINode *IndVar = Builder.CreatePHI(Type::getInt32Ty(Context), 2, "i");
    IndVar->addIncoming(StartVal, PreheaderBB);
    Value *Add = Builder.CreateAdd(val, Builder.getInt32(5), "addtmp");
    Value *StepVal = Builder.getInt32(1);
    Value *NextVal = Builder.CreateAdd(IndVar, StepVal, "nextval");
    Value *EndCond = Builder.CreateICmpULT(IndVar, EndVal, "endcond");
    EndCond = Builder.CreateICmpNE(EndCond, Builder.getInt1(0), "loopcond");
    BasicBlock *LoopEndBB = Builder.GetInsertBlock();
    BasicBlock *AfterBB = List[1];
    Builder.CreateCondBr(EndCond, LoopBB, AfterBB);
    Builder.SetInsertPoint(AfterBB);
    IndVar->addIncoming(NextVal, LoopEndBB);
    return Add;
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

    Function::arg_iterator AI = fooFunc->arg_begin();
    Value *Arg1 = dyn_cast<Value>(AI++);
    Value *Arg2 = dyn_cast<Value>(AI);
    Value *constant = Builder.getInt32(16);
    Value *val = createArith(Builder, Arg1, constant);
    ValList VL;
    VL.push_back(Arg1);

    BBList List;
    BasicBlock *LoopBB = createBB(fooFunc, "loop");
    BasicBlock *AfterBB = createBB(fooFunc, "afterloop");
    List.push_back(LoopBB);
    List.push_back(AfterBB);

    Value *StartVal = Builder.getInt32(1);
    Value *Res = createLoop(Builder, List, VL, StartVal, Arg2);

    Builder.CreateRet(Res);

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
  br label %loop

loop:                                             ; preds = %loop, %entry
  %i = phi i32 [ 1, %entry ], [ %nextval, %loop ]
  %addtmp = add i32 %a, 5
  %nextval = add i32 %i, 1
  %endcond = icmp ult i32 %i, %b
  %loopcond = icmp ne i1 %endcond, false
  br i1 %loopcond, label %loop, label %afterloop

afterloop:                                        ; preds = %loop
  ret i32 %addtmp
}
*/