#include "dflow_calc.h"
#define REGISTERS_NUMBER 32

class Instruction_Entry {
public:
    unsigned int ins_opcode;
    unsigned int ins_depth;
    int first_dep;
    int second_dep;
    unsigned int ins_latency;
};

class Instructions_Table {
public:
    int Registers_array[REGISTERS_NUMBER]; /* Registers Array */
    Instruction_Entry* Instructions_array; /* Entries Array */
    unsigned int Number_of_instructions;  /* Ins Number */

    /* Database Constructor , initializing the arrays */
    Instructions_Table(unsigned int numOfInsts) {
        Number_of_instructions = numOfInsts;
        for (int i = 0; i < REGISTERS_NUMBER; i++) {
            Registers_array[i] = -2;  /* -2 Means not entered */
        }
        Instructions_array = new Instruction_Entry[numOfInsts];
    }
};
unsigned int update_depth_src(int source_index,Instructions_Table* ctx_handle,int reg_index, bool is_first)
{
    unsigned int depth_srcs1; 
    if (source_index != -2)
    {
        if(is_first)
        {
            (ctx_handle->Instructions_array[reg_index]).first_dep = source_index;
        }
        else
        {
            (ctx_handle->Instructions_array[reg_index]).second_dep = source_index;
        }
        depth_srcs1 = ctx_handle->Instructions_array[source_index].ins_depth + ctx_handle->Instructions_array[source_index].ins_latency; 
    }
    else 
    {
        if(is_first)
        {
            (ctx_handle->Instructions_array[reg_index]).first_dep = -1;
        }
        else
        {
            (ctx_handle->Instructions_array[reg_index]).second_dep = -1;
        }
        depth_srcs1 = 0;
    }
    return depth_srcs1;
}

ProgCtx analyzeProg(const unsigned int opsLatency[], const InstInfo progTrace[], unsigned int numOfInsts) {
    /* Creating new table */
    Instructions_Table* ctx_handle= new Instructions_Table(numOfInsts);
    /* Looping over all the instructions in the program*/
    for (unsigned int i = 0; i < ctx_handle->Number_of_instructions; i++) 
    {
        /* Filling entries accordingly to the index of the ins */
        ctx_handle->Instructions_array[i].ins_opcode = progTrace[i].opcode;
        ctx_handle->Instructions_array[i].ins_latency = opsLatency[progTrace[i].opcode];

        unsigned int depth_src1 = update_depth_src(ctx_handle->Registers_array[progTrace[i].src1Idx],ctx_handle,i,true);
        unsigned int depth_src2 = update_depth_src(ctx_handle->Registers_array[progTrace[i].src2Idx],ctx_handle,i,false);

        /* Getting maximum depth */
        if(depth_src2 > depth_src1){
            ctx_handle->Instructions_array[i].ins_depth = depth_src2;
        }
        else{
            ctx_handle->Instructions_array[i].ins_depth = depth_src1;
        }
        /* Placing the ins index inside the dest index register */
        ctx_handle->Registers_array[progTrace[i].dstIdx] = i;
    }
       if(ctx_handle != NULL){ return ctx_handle;}
    return PROG_CTX_NULL;
}

void freeProgCtx(ProgCtx ctx) 
{   

	delete [] ((Instructions_Table*)ctx)->Instructions_array;
	delete (Instructions_Table*)ctx;
}

int getInstDepth(ProgCtx ctx, unsigned int theInst) {
    /* Making sure the input is legal */
    if(theInst < 0  || theInst >= (((Instructions_Table*)ctx)->Number_of_instructions)) 
        return -1; 
    return ((Instructions_Table*)ctx)->Instructions_array[theInst].ins_depth;
}

int getInstDeps(ProgCtx ctx, unsigned int theInst, int *src1DepInst, int *src2DepInst) {
    /* Making sure the input is legal */
    if(theInst < 0 || theInst >= (((Instructions_Table*)ctx)->Number_of_instructions)) 
        return -1;

    *src1DepInst = ((Instructions_Table*)ctx)->Instructions_array[theInst].first_dep;
    *src2DepInst = ((Instructions_Table*)ctx)->Instructions_array[theInst].second_dep;
    return 0;
}

int getProgDepth(ProgCtx ctx) 
{
    /* Obtaining maximum depth path */
    unsigned int max_depth = 0;
    for (unsigned int i = 0; i < ((Instructions_Table*)ctx)->Number_of_instructions; i++) {
        if (((((Instructions_Table*)ctx)->Instructions_array[i]).ins_depth + (((Instructions_Table*)ctx)->Instructions_array[i]).ins_latency) > max_depth) {
            max_depth = ((((Instructions_Table*)ctx)->Instructions_array[i]).ins_depth + (((Instructions_Table*)ctx)->Instructions_array[i]).ins_latency);
        }
    }
    return max_depth;
}



