#include "dflow_calc.h"
int MAX_DEPTH=-1;

struct instructions * main_table = NULL; 
//one instruction with all the data we need
struct Inst
{
   unsigned int opcode;
   unsigned int depth;
   int dependency_1;
   int dependency_2;
   unsigned int latency;
};
//table of instructions 
struct instructions
{
    unsigned int num_of_inst;
    int reg_arr[32];
    struct Inst *inst_arr; 
};

//for left src
unsigned int init_1( instructions* main_table ,int index , int src_index)
{
    unsigned int updated_src_1;
    if (src_index != -2)//check reg_arr if it depend on other, if not then equal entry
    {
    (main_table->inst_arr[index]).dependency_1 = src_index;
    updated_src_1 = main_table->inst_arr[src_index].depth + main_table->inst_arr[src_index].latency; 
    }

    else
    {
        main_table->inst_arr[index].dependency_1 = -1;
        updated_src_1=0;
    }
    //sum the depth for calc the MAX depth and getInstDepth
     return updated_src_1;
}

//for right src
unsigned int init_2(instructions* main_table ,unsigned index, int src_index)
{
    unsigned int updated_src_2;
    if (src_index != -2)//check reg_arr if it depend on other, if not then equal entry
    {
    main_table->inst_arr[index].dependency_2 = src_index;
    updated_src_2 = main_table->inst_arr[src_index].depth + main_table->inst_arr[src_index].latency; 
    }
    else
    {
        main_table->inst_arr[index].dependency_2 = -1;
        updated_src_2=0;//depend on entry so depth =0
    }
    //sum the depth for calc the MAX depth and getInstDepth
     return updated_src_2;
}

ProgCtx analyzeProg(const unsigned int opsLatency[], const InstInfo progTrace[], unsigned int numOfInsts) {
    main_table = (struct instructions *) malloc(sizeof(struct Inst ) * numOfInsts);
    main_table->inst_arr = (struct Inst *)malloc(sizeof(struct Inst ) * numOfInsts);

    for (int i = 0; i < 32; i++) 
    {
        main_table->reg_arr[i] = -2; //-1 is entry so NULL is -2
    }

    main_table->num_of_inst = numOfInsts;
    for (unsigned int i = 0; i < main_table->num_of_inst; i++) 
    {
        main_table->inst_arr[i].opcode = progTrace[i].opcode;
        main_table->inst_arr[i].latency = opsLatency[progTrace[i].opcode];

        // left src
        unsigned int updated_depth_1=init_1( main_table , i , main_table->reg_arr[progTrace[i].src1Idx] );
        //right src
        unsigned int updated_depth_2=init_2( main_table , i , main_table->reg_arr[progTrace[i].src2Idx] );
        if(updated_depth_2 > updated_depth_1)
        {
            main_table->inst_arr[i].depth = updated_depth_2;
        }
        else 
        {
            main_table->inst_arr[i].depth = updated_depth_1;
        }
            //finding max_depth and deliver to global int
            int tmp1= updated_depth_1 + main_table->inst_arr[i].latency;
            int tmp2= updated_depth_2 + main_table->inst_arr[i].latency;
            if (tmp1>tmp2)
            {
                if (MAX_DEPTH < tmp1)
                {
                MAX_DEPTH = tmp1;
                }
            }
            else
            {
                if (MAX_DEPTH < tmp2)
                {
                MAX_DEPTH = tmp2;
                }
            }
        main_table->reg_arr[progTrace[i].dstIdx] = i;
    }
        if(main_table != NULL)
        { 
            //returning the "handle"
            return main_table;
        }
    return PROG_CTX_NULL;
}

void freeProgCtx(ProgCtx ctx) {
    free(((instructions*)ctx)->inst_arr); 
    free((instructions*)ctx);
}

int getInstDepth(ProgCtx ctx, unsigned int theInst) {
    if( theInst < 0  || theInst >= (((instructions*)ctx)->num_of_inst))
    {
        return -1; 
    } 
    return ((instructions*)ctx)->inst_arr[theInst].depth;
}

int getInstDeps(ProgCtx ctx, unsigned int theInst, int *src1DepInst, int *src2DepInst) {
    if(theInst < 0 || theInst >= (((instructions*)ctx)->num_of_inst)) 
    {
        return -1;
    }
    *src1DepInst = ((instructions*)ctx)->inst_arr[theInst].dependency_1;
    *src2DepInst = ((instructions*)ctx)->inst_arr[theInst].dependency_2;
    return 0;
}

int getProgDepth(ProgCtx ctx) {
    return MAX_DEPTH;
}


