bool vectorize()
{
    // Use the bottom up slp vectorizer to construct chains that start
    // with store instructions.
    BoUpSLP R(&F, SE, TTI, TLI, AA, LI, DT, AC);

    // Scan the blocks in the function in post order.
    for (auto BB: post_order(&F.getEntryBlock())) {
        // Vectorize trees that end at stores.
        if (unsigned count = collectStores(BB, R)) {
            (void)count;
            DEBUG(dbgs() << "SLP: Found" << count << " stores to vectorize.\n");
            Changed |= vectorizeStoreChains(R);
        }
        // Vectorize trees that end at reductions.
        Changed |= vectorizeChainsInBlock(BB, R);
    }

}

unsigned
SLPVectorizer::collectStores(BasicBlock *BB, BoUpSLP &R) {
    unsigned count = 0;
    StoreRefs.clear();
    const DataLayout &DL = BB->getModule()->getDataLayout();
    for (Instruction &I : *BB) {
        StoreInst *SI = dyn_cast<StoreInst>(&I);
        if (!SI)
            continue;

        // Don't touch volatile stores.
        if (!SI->isSimple())
            continue;

        // Check that the pointer points to scalars.
        Type *Ty = SI->getValueOperand()->getType();
        if (!isValidElementType(Ty))
            continue;

        // Find the base pointer.
        Value *Ptr = GetUnderlyingObject(SI->getPointerOperand(), DL);

        // Save the store locations.
        StoreRefs[Ptr].push_back(SI);
        count++;
    }
    return count;
}

bool
SLPVectorizer::vectorizeStoreChain(ArrayRef<Value *> Chain, int CostThreshold, BoUpSLP &R, unsigned VecRegSize) {
    ...
    ...
    R.buildTree(Operands);

    int Cost = R.getTreeCost();

    DEBUG(dbgs() << "SLP: Found cost=" << Cost << " for VF=" << VF << "\n");
    if (Cost < CostThreshold) {
        DEBUG(dbgs() << "SLP: Decided to vectorize cost=" << Cost << "\n");
        R.vectorizeTree();
    }
    ...
}

void
BoUpSLP::buildTree(ArrayRef<Value *> Roots, ArrayRef<Value *> UserIgnoreLst) {
    ...
    ...
    buildTree_rec(Roots, 0);
    ...
    ...
}

void
BoUpSLP::buildTree_rec(ArrayRef<Value *> VL, unsigned Depth) {
    ...
    ...
    case Instruction::Add:
    newTreeEntry(VL, true);
    DEBUG(dbgs() << "SLP: added a vector of bin op.\n");

    // Sort operands of the instructions so that each side is more
    // likely to have the same opcode
    if (isa<BinaryOperator>(VL0) && VL0->isCommutative()) {
        ValueList Left, Right;

        reorderInputsAccordingToOpcode(VL, Left, Right);
        buildTree_rec(Left, Depth + 1);
        buildTree_rec(Right, Depth + 1);
        return;
    }
    ...
    ...
    case Instruction::Load:
    // Check that a vectorized load would load the same memory as a
    // scalar load.
    // For example we don't want vectorize loads that are smaller than 8 bit.
    // Even though we have a packed struct {<i2, i2, i2, i2>} LLVM treats
    // loading/storing it as an i8 struct. If we vectorize loads/stores from
    // such a struct we read/write packed bits disagreeing with the
    // unvectorized version.
    const DataLayout &DL = F->getParent()->getDataLayout();
    Type *ScalarTy = VL[0]->getType();

    if (DL.getTypeSizeInBits(ScalarTy) != DL.getTypeAllocSizeInBits(ScalarTy)) {
        BS.cancelScheduling(VL);
        newTreeEntry(VL, false);
        DEBUG(dbgs() << "SLP: Gathering loads of non-packed type.\n");
        return;
    }
    // Check if the loads are consecutive or of we need to swizzle them.
    for (unsigned i = 0, e = VL.size() - 1; i < e; ++i) {
        LoadInst *L = cast<LoadInst>(VL[i]);
        if (!L->isSimple()) {
            BS.cancelScheduling(VL);
            newTreeEntry(VL, false);
            DEBUG(dbgs() << "SLP: Gathering non-simple loads.\n");
            return;
        }

        if (!isConsecutiveAccess(VL[i], VL[i + 1], DL)) {
            if (VL.size() == 2 && isConsecutiveAccess(VL[1], VL[0], DL)) {
                ++NumLoadsWantToChangeOrder;
            }
            BS.cancelScheduling(VL);
            newTreeEntry(VL, false);
            DEBUG(dbgs() << "SLP: Gathering non-consecutive loads.\n");
            return;
        }
        ++NumLoadsWantToKeepOrder;
        newTreeEntry(VL, true);
        DEBUG(dbgs() << "SLP: added a vector of loads.\n");
        return;
    }
    ...
    ...
}

int BoUpSLP::getTreeCost() {
    int Cost = 0;
    DEBUG(dbgs() << "SLP: Calculating cost for tree of size " << VectorizableTree.size() << ".\n");

    // We only vectorize tiny trees if it is fully vectorizable.
    if (VectorizableTree.size() < 3 && !isFullyVectorizableTinyTree()) {
        if (VectorizableTree.empty()) {
            assert(!ExternalUses.size && "We should not have any external users");
        }
        return INT_MAX;
    }

    unsigned BundleWidth = VectorizableTree[0].Scalars.size();

    for (unsigned i = 0, e = VectorizableTree.size(); i != e; ++i) {
        int C = getEntryCost(&VectorizableTree[i]);
        DEBUG(dbgs() << "SLP: Adding cost" << C << " for bundle that starts with "
            << *VectorizableTree[i].Scalars[0] << ".\n");
        Cost += C;
    }

    SmallSet<Value *, 16> ExtractCostCalculated;
    int ExtractCost = 0;
    for (UserList::iterator I = ExternalUses.begin(), E = ExternalUses.end(); I != E; ++I) {
        // We only add extract cost once for the same scalar.
        if (!ExtractCostCalculated.insert(I->Scalar).second)
            continue;

        // Uses by ephemeral values are free (because the ephemeral value will be
        // removed prior to code generation, and so the extraction will be
        // removed as well).
        if (EphValues.count(I->User))
            continue;

        VectorType *VecTy = VectorType::get(I->Scalar->getType(), BundleWidth);
        ExtractCost += TTI->getVectorInstrCost(Instruction::ExtractElement, VecTy, I->Lane);
    }

    Cost += getSpillCost();

    DEBUG(dbgs() << "SLP: Total Cost " << Cost + ExtractCost << ".\n");
    return Cost + ExtractCost;
}

int BoUpSLP::getEntryCost(TreeEntry *E) {
    ...
    ...
    case Instruction::Store: {
        // We know that we can merge the stores. Calculate the cost.
        int ScalarStCost = VecTy->getNumElements() *
           TTI->getMemoryOpCost(Instruction::Store, ScalarTy, 1, 0);
        int VecStCost = TTI->getMemoryOpCost(Instruction::Store, VecTy, 1, 0);
        return VecStCost - ScalarStCost;
    }
    ...
    ...
    case Instruction::Add: {
        // Calculate the cost of this instruction.
        int ScalarCost = 0;
        int VecCost = 0;
        if (Opcode == Instruction::FCmp ||
            Opcode == Instruction::ICmp ||
            Opcode == Instruction::Select) {
            VectorType *MaskTy = VectorType->get(Builder.getInt1Ty(), VL.size());
            ScalarCost = VecTy->getNumElements() * TTI->getCmpSelInstrCost(Opcode, VecTy, MaskTy);
        }
        else {
            // Certain instructions can be cheaper to vectorizer if they have
            // a constant second vector operand.
            TargetTransformInfo::OperandValueKind Op1VK = TargetTransformInfo::OK_AnyValue;
            TargetTransformInfo::OperandValueKind Op2VK = TargetTransformInfo::OK_UniformConstantValue;
            TargetTransformInfo::OperandValueProperties Op1VP = TargetTransformInfo::OP_None;
            TargetTransformInfo::OperandValueProperties Op2VP = TargetTransformInfo::OP_None;

            // If all operands are exactly the same ConstantInt then set the
            // operand kind to OK_UniformConstantValue.
            // If instead not all operands are constants, then set the operand kind
            // to OK_AnyValue. If all operands are constants but not the
            // same, then set the operand kind to OK_NonUniformConstantValue.
            ConstantInt *CInt = nullptr;
            for (unsigned i = 0; i < VL.size(); ++i) {
                const Instruction *I = cast<Instruction>(VL[i]);
                if (!isa<ConstantInt>(I->getOperand(1))) {
                    Op2VK = TargetTransformInfo::OK_AnyValue;
                    break;
                }
                if (i == 0) {
                    CInt = cast<ConstantInt>(I->getOperand(1));
                    continue;
                }
                if (Op2VK == TargetTransformInfo::OK_UniformConstantValue &&
                    CInt != cast<ConstantInt>(I->getOperand(1)))
                    Op2VK = TargetTransformInfo::OK_NonUniformConstantValue;
                // FIXME: Currently cost of model modification for division by
                // power of 2 is handled only for X86. Add support for other
                // targets.
                if (Op2VK == TargetTransformInfo::OK_UniformConstantValue &&
                    CInt &&
                    CInst->getValue().isPowerOf2())
                    Op2VK = TargetTransformInfo::OP_PowerOf2;

                ScalarCost = VecTy->getNumElements() *
                   TTI->getArithmeticInstrCost(Opcode, ScalarTy, Op1VK, Op2VK, Op1VP, Op2VP);
                VecCost = TTI->getArithmeticInstrCost(Opcode, VecTy, Op1VK, Op2VK, Op1VP, Op2VP);
                return VecCost - ScalarCost;
            }
        }
    }
}

Value *BoUpSLP::vectorizeTree() {
  ...
  vectorizeTree(&VectorizableTree[0]);
  ...
}