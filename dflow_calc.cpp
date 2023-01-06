#include "dflow_calc.h"
int MAX_DEPTH=-1;

class Instruction_Entry {
public:
    unsigned int opcode;
    unsigned int depth;
    int dependency_1;
    int dependency_2;
    unsigned int latency;
};

class Instructions_Table {
public:
    int reg_arr[32]; /* Registers Array */
    Instruction_Entry* inst_arr; /* Entries Array */
    unsigned int Number_of_instructions;  /* Ins Number */

    /* Database Constructor , initializing the arrays */
    Instructions_Table(unsigned int numOfInsts) {
        Number_of_instructions = numOfInsts;
        for (int i = 0; i < 32; i++) {
            reg_arr[i] = -2;  /* -2 Means not entered */
        }
        inst_arr = new Instruction_Entry[numOfInsts];
    }
};



//*********************UPDATE FUNCTIONS**********************

//for left src
unsigned int init_1( Instructions_Table* main_table ,int index , int src_index)
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
    //sum the depth+latency for calc the MAX depth and inst_depth
     return updated_src_1;
}
//for right src
unsigned int init_2(Instructions_Table* main_table ,unsigned index, int src_index)
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
    //sum the depth+latency for calc the MAX depth and inst_depth
     return updated_src_2;
}


//****************************************************************************


ProgCtx analyzeProg(const unsigned int opsLatency[], const InstInfo progTrace[], unsigned int numOfInsts) 
{
    // int tmp=0;
    /* Creating new table */
    Instructions_Table* main_table= new Instructions_Table(numOfInsts);
    /* Looping over all the instructions in the program*/
    for (unsigned int i = 0; i < main_table->Number_of_instructions; i++) 
    {
        /* Filling entries accordingly to the index of the ins */
        main_table->inst_arr[i].opcode = progTrace[i].opcode;
        main_table->inst_arr[i].latency = opsLatency[progTrace[i].opcode];

        // left src
        unsigned int depth_src1=init_1( main_table , i , main_table->reg_arr[progTrace[i].src1Idx] );
        //right src
        unsigned int depth_src2=init_2( main_table , i , main_table->reg_arr[progTrace[i].src2Idx] );

        /* Getting maximum depth */
        if(depth_src2 > depth_src1)
        {
            main_table->inst_arr[i].depth = depth_src2;
            // tmp= depth_src2;
            // if (MAX_DEPTH <= tmp)
            // {
            // MAX_DEPTH = depth_src2 + main_table->inst_arr[i].latency;
            // }
        }
        else 
        {
            main_table->inst_arr[i].depth = depth_src1;
            // tmp= depth_src1;
            // if (MAX_DEPTH <= tmp)
            // {
            // MAX_DEPTH = depth_src1 + main_table->inst_arr[i].latency;
            // }
        }

        /* Placing the ins index inside the dest index register */
        main_table->reg_arr[progTrace[i].dstIdx] = i;
    }
        if(main_table != NULL)
        { 
            return main_table;
        }
    return PROG_CTX_NULL;
}

void freeProgCtx(ProgCtx ctx) 
{   

	delete [] ((Instructions_Table*)ctx)->inst_arr;
	delete (Instructions_Table*)ctx;
}

int getInstDepth(ProgCtx ctx, unsigned int theInst) {
    /* Making sure the input is legal */
    if(theInst < 0  || theInst >= (((Instructions_Table*)ctx)->Number_of_instructions)) 
        return -1; 
    return ((Instructions_Table*)ctx)->inst_arr[theInst].depth;
}

int getInstDeps(ProgCtx ctx, unsigned int theInst, int *src1DepInst, int *src2DepInst) {
    /* Making sure the input is legal */
    if(theInst < 0 || theInst >= (((Instructions_Table*)ctx)->Number_of_instructions)) 
        return -1;

    *src1DepInst = ((Instructions_Table*)ctx)->inst_arr[theInst].dependency_1;
    *src2DepInst = ((Instructions_Table*)ctx)->inst_arr[theInst].dependency_2;
    return 0;
}

int getProgDepth(ProgCtx ctx) 
{
    /* Obtaining maximum depth path */
    unsigned int max_depth = 0;
    for (unsigned int i = 0; i < ((Instructions_Table*)ctx)->Number_of_instructions; i++) {
        if (((((Instructions_Table*)ctx)->inst_arr[i]).depth + (((Instructions_Table*)ctx)->inst_arr[i]).latency) > max_depth) {
            max_depth = ((((Instructions_Table*)ctx)->inst_arr[i]).depth + (((Instructions_Table*)ctx)->inst_arr[i]).latency);
        }
    }
    return max_depth;
    // return MAX_DEPTH;
}



