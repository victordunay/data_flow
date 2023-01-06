#include "dflow_calc.h"
#include "stdio.h"

unsigned int MAX_DEPTH=0;
struct Inst_table * main_table = NULL; 
unsigned int num_of_inst = 0;

//struct Inst_table *inst_arr = NULL; 

//*****************DATA STRUCTURE*****************************
//individual instruction
struct Inst
{
   unsigned int opcode;
   unsigned int depth;
   int dependency_1;
   int dependency_2;
   unsigned int latency;
};

//arr of type struct each index is instruction 
 struct Inst_table
{
    int reg_arr[32];
    struct Inst *inst_arr; 
  //  inst_arr = malloc(sizeof(struct Inst ) * numOfInsts);
};


//*********************UPDATE FUNCTIONS**********************
//for left src
unsigned int init_1( struct Inst_table * main_table ,int index , int src_index)
{
    //  printf("index=%d\n",index);
    //  printf("src_index=%d\n",src_index);

    unsigned int updated_src_1;
    if (src_index != -2)//check reg_arr if it depend on other, if not then equal entry
    {
    (main_table->inst_arr[index]).dependency_1 = src_index;
    updated_src_1 = (main_table->inst_arr[src_index].depth) + (main_table->inst_arr[src_index]).latency; 
    }
    else
    {
        main_table->inst_arr[index].dependency_1 = -1;
         updated_src_1=0;
    }
    //sum the depth+latency for calc the MAX depth and inst_depth
    // printf("updated_src_1=%d\n",updated_src_1);
     return updated_src_1;
}

//for right src
unsigned int init_2(struct Inst_table * main_table ,unsigned index, int src_index)
{
    unsigned int updated_src_2;
    if (src_index != -2)//check reg_arr if it depend on other, if not then equal entry
    {
    main_table->inst_arr[index].dependency_2 = src_index;
    updated_src_2 = main_table->inst_arr[src_index].depth + (main_table->inst_arr[src_index]).latency; 
    }
    else
    {
        main_table->inst_arr[index].dependency_2 = -1;
         updated_src_2=0;//depend on entry so depth =0
    }
    //sum the depth+latency for calc the MAX depth and inst_depth
    // printf("updated_src_2=%d\n",updated_src_2);
     return updated_src_2;
}

ProgCtx analyzeProg(const unsigned int opsLatency[], const InstInfo progTrace[], unsigned int numOfInsts) 
{
    // int tmp=0;
    num_of_inst=numOfInsts;
    main_table = (struct Inst_table *) malloc(sizeof(struct Inst ) * numOfInsts);
    main_table->inst_arr = malloc(sizeof(struct Inst ) * numOfInsts);
    for (int i = 0; i < 32; i++) 
    {
        main_table->reg_arr[i] = -2; //-1 is entry so NULL is -2
    }




    for (unsigned int i = 0; i < numOfInsts; i++) 
    {
    //fill the database with the input
    main_table->inst_arr[i].opcode = progTrace[i].opcode;
    main_table->inst_arr[i].latency = opsLatency[progTrace[i].opcode];
    //left src
    //printf("reg_arr[i]=%d\n",main_table->reg_arr[progTrace[i].src1Idx]);
    unsigned int updated_depth_1=init_1( main_table , i , main_table->reg_arr[progTrace[i].src1Idx] );
    //right src
    unsigned int updated_depth_2=init_2( main_table , i , main_table->reg_arr[progTrace[i].src1Idx] );
    //the inst depth is the max between the two src depths (with the latency of the last)
    if(updated_depth_1 < updated_depth_2)
    {
        main_table->inst_arr[i].depth = updated_depth_2;
        // tmp= updated_depth_2;
        // if (MAX_DEPTH < tmp)
        // {
        // MAX_DEPTH =updated_depth_2;
        // }
    }
    else
    {
        main_table->inst_arr[i].depth = updated_depth_2;
        // tmp= updated_depth_2;
        // if (MAX_DEPTH < tmp)
        // {
        // MAX_DEPTH =updated_depth_2;
        // }
    }
    //updating register
    main_table->reg_arr[progTrace[i].dstIdx] = i;
    }
    if(main_table != NULL)
    { 
        return main_table;
    }
        //supplied FAIL return in the header file
    return PROG_CTX_NULL;
}








//*****************************DELIVER FUNCTIONS**********************

void freeProgCtx(ProgCtx ctx) 
{

}

// int getInstDepth(ProgCtx ctx, unsigned int theInst)
// {
//   if( theInst < 0  /*|| theInst >= num_of_instructions*/) 
//   {
//     return -1;
//   }
//      return main_table->inst_arr[theInst].depth;
    
// }

// int getInstDeps(ProgCtx ctx, unsigned int theInst, int *src1DepInst, int *src2DepInst) 
// {

//   if( theInst < 0  /*|| theInst >= num_of_instructions*/) 
//    {
//     return -1;
//    }
//     *src1DepInst = main_table->inst_arr[theInst].dependency_1; 
//     *src2DepInst = main_table->inst_arr[theInst].dependency_2;
//     return 0;   
// }

// int getProgDepth(ProgCtx ctx) 
// {
// // return MAX_DEPTH;
//     int max_depth = 0;
//     for ( int i = 0; i < num_of_inst; i++) 
//     {
//         if ((   (  main_table->inst_arr[i]).depth + (main_table->inst_arr[i]).latency) > max_depth)
//          {
//             max_depth = ((main_table->inst_arr[i]).depth + (main_table->inst_arr[i]).latency);
//         }
//     }
//     return max_depth;
// }

int getInstDepth(ProgCtx ctx, unsigned int theInst) 
{
    /* Making sure the input is legal */
    if(theInst < 0 /* || theInst >= (((Inst_table*)ctx)->Number_of_instructions)) */
        return -1; 
    return ((Inst_table*)ctx)->inst_arr[theInst].depth;
}

int getInstDeps(ProgCtx ctx, unsigned int theInst, int *src1DepInst, int *src2DepInst) {
    /* Making sure the input is legal */
    if(theInst < 0 /*|| theInst >= (((Inst_table*)ctx)->Number_of_instructions)) */
        return -1;

    *src1DepInst = ((Inst_table*)ctx)->inst_arr[theInst].dependency_1;
    *src2DepInst = ((Inst_table*)ctx)->inst_arr[theInst].dependency_2;
    return 0;
}

int getProgDepth(ProgCtx ctx) 
{
    unsigned int max_depth = 0;
    for (unsigned int i = 0; i < num_of_inst; i++) {
        if (((((Inst_table*)ctx)->inst_arr[i]).depth + (((Inst_table*)ctx)->inst_arr[i]).latency) > max_depth) {
            max_depth = ((((Inst_table*)ctx)->inst_arr[i]).depth + (((Inst_table*)ctx)->inst_arr[i]).latency);
        }
    }
    return max_depth;
}

