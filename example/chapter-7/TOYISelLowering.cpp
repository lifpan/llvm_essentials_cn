
#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/IntrinsicLowering.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineJumpTableInfo.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/Type.h"
#include "llvm/MC/MCSectionMachO.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetOptions.h"
#include <utility>

class TOYTargetLowering : public TargetLowering {
}

SDValue TOYTargetLowering::LowerCall(TargetLowering::CallLoweringInfo &CLI,
  SmallVectorImpl<SDValue> &InVals) const {
    SelectionDAG &DAG = CLI.DAG;
    SDLoc &Loc = CLI.DL;
    SmallVectorImpl<ISD::OutputArg> &Outs = CLI.Outs;
    SmallVectorImpl<SDValue> &OutVals = CLI.OutVals;
    SmallVectorImpl<ISD::InputArg> &Ins = CLI.Ins;
    SDValue Chain = CLI.Chain;
    SDValue Callee = CLI.Callee;
    CallingConv::ID CallConv = CLI.CallConv;
    const bool isVarArg = CLI.IsVarArg;

    CLI.IsTailCall = false;

    if (isVarArg) {
      llvm_unreachable("Unimplemented");
    }

    // Analyze operands of the call, assigning locations to each operand.
    SmallVector<CCValAssign, 16> ArgLocs;
    CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(), ArgLocs, *DAG.getContext());
    CCInfo.AnalyzeCallOperands(Outs, CC_TOY);

    // Get the size of the outgoing arguments stack space
    // requirement.
    const unsigned NumBytes = CCInfo.getNextStackOffset();

    Chain = DAG.getCALLSEQ_START(Chain, DAG.getIntPtrConstant(NumBytes, Loc, true), Loc);

    SmallVector<std::pair<unsigned, SDValue>, 8> RegsToPass;
    SmallVector<SDValue, 8> MemOpChains;

    // Walk the register/memloc assignments, inserting copies/loads.
    for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
      CCValAssign &VA = ArgLocs[i];
      SDValue Arg = OutVals[i];

      // We only handle fully promoted arguments.
      assert(VA.getLocInfo() == CCValAssign::Full && "Unhandled loc info");

      if (VA.isRegLoc()) {
        RegsToPass.push_back(std::make_pair(VA.getLocReg(), Arg));
        continue;
      }

      assert(VA.isMemLoc() && "Only support passing arguments through registers or via the stack");
      SDValue StackPtr = DAG.getRegister(TOY::SP, MVT::i32);
      SDValue PtrOff = DAG.getIntPtrConstant(VA.getLocMemOffset(), Loc);
      PtrOff = DAG.getNode(ISD::ADD, Loc, MVT::i32, StackPtr, PtrOff);

      MemOpChains.push_back(DAG.getStore(Chain, Loc, Arg, PtrOff, MachinePointerInfo(), false, false, 0));
    }

    // Emit all stores, make sure they occur before the call.
    if (!MemOpChains.empty()) {
      Chain = DAG.getNode(ISD::TokenFactor, Loc, MVT::Other, MemOpChains);
    }

    // Build a sequence of copy-to-reg nodes chained together with
    // token chain
    // and flag operands which copy the outgoing args into the
    // appropriate regs.
    SDValue InFlag;
    for (auto &Reg : RegsToPass) {
      Chain = DAG.getCopyToReg(Chain, Loc, Reg.first, Reg.second, InFlag);
      InFlag = Chain.getValue(1);
    }

    // We only support calling global addresses.
    GlobalAddressSDNode *G = dyn_cast<GlobalAddressSDNode>(Callee);
    assert(G && "We only support the calling of global addresses");
    EVT PtrVT = getPointerTy(DAG.getDataLayout());
    Callee = DAG.getGlobalAddress(G->getGlobal(), Loc, PtrVT, 0);

    std::vector<SDValue> Ops;
    Ops.push_back(Chain);
    Ops.push_back(Callee);

    // Add argument registers to the end of the list so that they
    // are known live into the call.
    for (auto &Reg : RegsToPass) {
      Ops.push_back(DAG.getRegister(Reg.first, Reg.second.getValueType()));
    }

    // Add a register mask operand representing the call-preserved
    // registers.
    const uint32_t *Mask;
    const TargetRegisterInfo *TRI = DAG.getSubtarget().getRegisterInfo();
    Mask = TRI->getCallPreservedMask(DAG.getMachineFunction(), CallConv);
    assert(Mask && "Missing call preserved mask for calling convertion");
    Ops.push_back(DAG.getRegisterMask(Mask));

    if (InFlag.getNode()) {
      Ops.push_back(InFlag);
    }
    SDVTList NodeTys = DAG.getVTList(MVT::Other, MVT::Glue);

    // Return a chain and a flag for retval copy to use.
    Chain = DAG.getNode(TOYISD::CALL, Loc, NodeTys, Ops);
    InFlag = Chain.getValue(1);
    Chain = DAG.getCALLSEQ_END(Chain,
      DAG.getIntPtrConstant(NumBytes, Loc, true),
      DAG.getIntPtrConstant(0, Loc, true),
      InFlag, Loc);
    if (!Ins.empty()) {
      InFlag = Chain.getValue(1);
    }

    // Handle result values, copying them out of physregs into vregs
    // that we return.
    return LowerCallResult(Chain, InFlag, CallConv, isVarArg, Ins, Loc, DAG, InVals);
  }

  SDValue TOYTargetLowering::LowerFormalArguments(SDValue Chain,
    CallingConv::ID CallConv, bool isVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins,
    SDLoc dl, SelectionDAG &DAG,
    SmallVectorImpl<SDValue> &InVals) const {
      MachineFunction &MF = DAG.getMachineFunction();
      MachineRegisterInfo &RegInfo = MF.getRegInfo();
      assert(!isVarArg && "VarArg not supported");

      // Assign locations to all of the incoming arguments.
      SmallVector<CCValAssign, 16> ArgLocs;
      CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(), ArgLocs, *DAG.getContext());
      CCInfo.AnalyzeFormalArguments(Ins, CC_TOY);

      for (auto &VA :: ArgLocs) {
        if (VA.isRegLoc()) {
          // Arguments passed in registers
          EVT RegVT = VA.getLocVT();
          asseert(RegVT.getSimpleVT().SimpleTy == MVT::i32 && "Only support MVT::i32 register passing");
          const unsigned VReg = RegInfo.createVirtualRegister(&TOY::GRRegsRegClass);
          RegInfo.addLiveIn(VA.getLocReg(), VReg);
          SDValue ArgIn = DAG.getCopyFromReg(Chain, dl, VReg, RegVT);
          InVals.push_back(ArgIn);
          continue;
        }
        assert(VA.isMemLoc() && "Can only pass arguments as either registers or via the stack");

        const unsigned Offset = VA.getLocMemOffset();
        const int FI = MF.getFrameInfo()->CreateFixedObject(4, Offset, true);
        EVT PtrTy = getPointerTy(DAG.getDataLayout());
        SDValue FIPtr = DAG.getFrameIndex(FI, PtrTy);
        assert(VA.getValVT() == MVT::i32 && "Only support passing arguments as i32");
        SDValue Load = DAG.getLoad(VA.getValVT(), dl, Chain, FIPtr,
          MachinePointerInfo(), false, false, false, 0);
        InVals.push_back(Load);
      }
      return Chain;
    }
  
  bool TOYTargetLowering::CanLowerReturn(CallingConv::ID CallConv, MachineFunction &MF, bool isVarArg,
    const SmallVectorImpl<ISD::OutputArg> &Outs, LLVMContext &Context) const {
      SmallVector<CCValAssign, 16> RVLocs;
      CCState CCInfo(CallConv, isVarArg, MF, RVLocs, Context);
      if (!CCInfo.CheckReturn(Outs, RetCC_TOY)) {
        return false;
      }
      if (CCInfo.getNextStackOffset() != 0 && isVarArg) {
        return false;
      }
      return true;
    }

  SDValue TOYTargetLowering::LowerReturn(SDValue Chain,
                                         CallingConv::ID CallConv, bool isVarArg,
                                         const SmallVectorImpl<ISD::OutputArg> &Outs,
                                         const SmallVectorImpl<SDValue> &OutVals,
                                         SDLoc dl, SelectionDAG &DAG) const {
    if (isVarArg) {
      report_fatal_error("VarArg not supported");
    }

    // CCValAssign - represent the assignment of
    // the return value to a location
    SmallVector<CCValAssign, 16> RVLocs;
    // CCState - Info about the registers and stack slot.
    CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(), RVLocs, *DAG.getContext());
    CCInfo.AnalyzeReturn(Outs, RetCC_TOY);
    SDValue Flag;
    SmallVector<SDValue, 4> RetOps(1, Chain);

    // Copy the result values into the output registers.
    for (unsigned i = 0, e = RVLocs.size(); i < e; ++i) {
      CCValAssign &VA = RVLocs[i];
      assert(VA.isRegLoc() && "Can only return in register!");

      Chain = DAG.getCopyToReg(Chain, dl, VA.getLocReg(), OutVals[i], Flag);
      Flag = Chain.getValue(1);
      RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
    }

    RetOps[0] = Chain; // Update chain.
    // Add the flag if we have it.
    if (Flag.getNode()) {
      RetOps.push_back(Flag);
    }

    return DAG.getNode(TOYISD::RET_FLAG, dl, MVT::Other, RetOps);
  }