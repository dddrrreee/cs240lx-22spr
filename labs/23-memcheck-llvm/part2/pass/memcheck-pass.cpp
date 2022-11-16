#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

using namespace llvm;

namespace {
struct SkeletonPass : public ModulePass {
 public:
  SkeletonPass() : ModulePass(ID) {}

  virtual bool runOnModule(Module& M) {
    auto& context = M.getContext();

    // Set up types of functions in the runtime library.
    FunctionType* logMallocType = FunctionType::get(
        // return type of function
        Type::getVoidTy(context),
        // argument type of function
        {Type::getInt8PtrTy(context), Type::getInt64Ty(context)},
        // not a variadic function
        false);

    FunctionType* logFreeType = FunctionType::get(
        Type::getVoidTy(context), {Type::getInt8PtrTy(context)}, false);

    FunctionType* initType = FunctionType::get(Type::getVoidTy(context), false);

    FunctionCallee mallocFunc =
        M.getOrInsertFunction("log_malloc", logMallocType);
    FunctionCallee freeFunc = M.getOrInsertFunction("log_free", logFreeType);
    FunctionCallee loadFunc = M.getOrInsertFunction("log_load", logFreeType);
    FunctionCallee storeFunc = M.getOrInsertFunction("log_store", logFreeType);
    FunctionCallee stackFunc = M.getOrInsertFunction("log_stack", logFreeType);
    FunctionCallee initFunc = M.getOrInsertFunction("init_check", initType);
    FunctionCallee exitFunc = M.getOrInsertFunction("exit_check", initType);

    bool insertedStart = false;
    Instruction* last;
    // Loop through all functions.
    for (auto& F : M) {
      // Loop through all basic blocks.
      for (auto& B : F) {
        // Loop through all instructions.
        for (auto& I : B) {
          if (F.getName() == "main" && !insertedStart) {
            // Call the init_check function before executing any instructions
            // in main.
            IRBuilder<> builder(&I);
            builder.SetInsertPoint(&I);
            builder.CreateCall(initFunc, {}, "");
            insertedStart = true;
          }

          // Check if it was a call node.
          if (CallInst* call = dyn_cast<CallInst>(&I)) {
            // Check if callee was malloc.
            if (call->getCalledFunction()->getName() == "malloc") {
              // Grab pointer returned by malloc.
              Value* pointer = cast<Value>(call);
              // Get size of allocated memory in bytes.
              Value* size = call->getOperand(0);
              // Insert call to log_malloc after malloc has returned.
              IRBuilder<> builder(call);
              builder.SetInsertPoint((&I)->getNextNode());
              builder.CreateCall(mallocFunc, {pointer, size}, "");
            }

            if (call->getCalledFunction()->getName() == "free") {
              // Get pointer that is being freed.
              Value* pointer = call->getOperand(0);
              // Insert call to log_free after free has returned.
              IRBuilder<> builder(call);
              builder.SetInsertPoint((&I)->getNextNode());
              builder.CreateCall(freeFunc, {pointer}, "");
            }
          }

          // Instrument Loads
          if (LoadInst* load = dyn_cast<LoadInst>(&I)) {
            // TODO: Get the address where the load occurs.

            // TODO: Cast the address to a uint8 ptr, as expected by log_load.

            // TODO: Insert call to log_load.
           
          }

          // Instrument Stores
          if (StoreInst* store = dyn_cast<StoreInst>(&I)) {
            // TODO: Get the address where the store occurs.

            // TODO: Cast the address to a uint8 ptr, as expected by log_store.

            // TODO: Insert call to log_store.
          }

          // Keep track of stack addresses to eliminate false positives.
          if (AllocaInst* stack = dyn_cast<AllocaInst>(&I)) {
            // TODO: Get the address where the stack gets allocated.

            // TODO: Cast the address to a uint8 ptr, as expected by log_stack.

            // TODO: Insert call to log_store.
          }
          last = &I;
        }
      }
    }
    // Call the exit_check function.
    IRBuilder<> builder(last);
    builder.SetInsertPoint(last);
    builder.CreateCall(exitFunc, {}, "");
    return true;
  }

  static char ID;
};
}  // namespace

char SkeletonPass::ID = 0;
// Register the Pass. To enable this pass, need to compile with --memcheck.
static RegisterPass<SkeletonPass> X("memcheck",
                                    "enable memory corruption detection.");
