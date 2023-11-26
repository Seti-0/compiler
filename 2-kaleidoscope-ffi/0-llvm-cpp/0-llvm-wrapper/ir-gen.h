#ifndef LLVM_WRAPPER_IR_GEN_H
#define LLVM_WRAPPER_IR_GEN_H

#define API_FUNCTION extern "C" __declspec(dllexport)

// ##################
// # Initialization #
// ##################

API_FUNCTION void LW_Init();

// ##################
// # Target Machine #
// ##################

struct LW_TargetMachine;

// TODO: This seems to create an instance, see target->createTargetMachine in the implementation.
// There should maybe be an LW_TargetMachine_Release() as well.
API_FUNCTION LW_TargetMachine* LW_GetDefaultMachine();

API_FUNCTION const char* LW_TargetMachine_GetDescription(LW_TargetMachine*);

// ################
// # IR Generator #
// ################

/*
struct LW_Gen;
struct LW_Var; // This will probably need to be split into Alloca vs. other kinds of storage?
struct LW_Val;
struct LW_Block;

API_FUNCTION LW_Gen* LW_Gen_Create();

API_FUNCTION LW_Var* LW_WriteAlloca(LW_Gen*);
API_FUNCTION void LW_WriteStore(LW_Gen*, LW_Var*, LW_Val*);
API_FUNCTION LW_Val* LW_WriteLoad(LW_Gen*, LW_Var*);

API_FUNCTION LW_Block* LW_CreateBlock(LW_Gen*);
API_FUNCTION void LW_ActivateBlock(LW_Gen*, LW_Block*);

API_FUNCTION void LW_WriteBranch(LW_Gen*, LW_Block*);
API_FUNCTION void LW_WriteConditionalBranch(LW_Gen*, LW_Val*, LW_Block* a, LW_Block* b);
*/

#endif