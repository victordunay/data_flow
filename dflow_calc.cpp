/* 046267 Computer Architecture - Winter 20/21 - HW #3               */
/* Implementation (skeleton)  for the dataflow statistics calculator */

#include <iostream>
#include "dflow_calc.h"

using namespace std;


typedef enum
{
  LEFT,
  RIGHT
} src_t;

int MAX_DEPTH=0;
int max_index = -1;
struct Node ** dependency_list = NULL; 
unsigned * weight_arr = NULL;
int ** dependency_arr = NULL;
static unsigned int num_of_instructions = 0;
struct Node
{
   unsigned instruction_index;
   unsigned modified_register;
   unsigned weight;
   src_t src;
   struct Node *next;
};

unsigned biggest_weight(unsigned weight_arr[],unsigned int numOfInsts, const unsigned int opsLatency[])
{
  unsigned MAX=0;
  for(unsigned i=0;i<numOfInsts;i++)
  {
    if (weight_arr[i] + opsLatency[i]> MAX)
    {
      MAX=weight_arr[i];
      max_index = i;
    }
  }

  return MAX;
}



void show_dependencies(struct Node ** branches, unsigned num_of_branches,const unsigned int opsLatency[],const InstInfo progTrace[],unsigned int numOfInsts,int ** dependency_arr)
{
  int j=0;
  struct Node * iterator = NULL;

  for(unsigned instruction_num = 0; instruction_num < numOfInsts; ++instruction_num) // instruction number i
  {
    //j=0;
    for(unsigned branch_index = 0; branch_index < num_of_branches; ++branch_index) // dependency_list[i]
    {
      iterator = dependency_list[branch_index] ;
      while(  iterator!=NULL )
      {

        if(iterator->next == NULL)
        {
          break;
        }
        //LEFT=0    RIGHT=1
        if(iterator->instruction_index== instruction_num)
        {
          if ( dependency_arr[instruction_num][0] !=(int)iterator->next->instruction_index && dependency_arr[instruction_num][1] !=(int)iterator->next->instruction_index )
          {
            if (iterator->src==LEFT)
            {
              j=0;
            }
            else
            {
              j=1;
            }
            dependency_arr[instruction_num][j]= (int)iterator->next->instruction_index;
            iterator = iterator->next;
            //j++;
            break;
          }
          else
          {
            iterator = iterator->next;
          }
        } 
        //not the index we were looking for 
        else
        {
          iterator = iterator->next;
        }
      }
      //there is only one dependency
      //  if (j==1)
      // {
      //   dependency_arr[instruction_num][1]=-1;
      // }
    }
  } 
}







void sum_weight(struct Node ** branches, unsigned num_of_branches,const unsigned int opsLatency[],const InstInfo progTrace[],unsigned int numOfInsts,unsigned int weight_arr[])
{
  struct Node * iterator = NULL;

  for(unsigned instruction_num = 0; instruction_num < numOfInsts; ++instruction_num) // instruction number i
  {
    int instruction_max=-1;
    for(unsigned branch_index = 0; branch_index < num_of_branches; ++branch_index) // dependency_list[i]
    {
      iterator = dependency_list[branch_index] ;
      while(  iterator!=NULL )
      {

          //if equal update weight
        if(iterator->instruction_index== instruction_num)
        {
          if(iterator->next == NULL )
          {
            weight_arr[instruction_num]=0;
            instruction_max=0;
            break;
          }
          else
          {
            //not pointing to entry so increse weight by adding the latancy of previous to the weight of the previous
            if(iterator->next != NULL)
            {
            iterator = iterator->next;
            }
            weight_arr[instruction_num] = weight_arr[iterator->instruction_index] + opsLatency[progTrace[iterator->instruction_index].opcode];
            int tmp =weight_arr[iterator->instruction_index] + opsLatency[progTrace[iterator->instruction_index].opcode];
            if (tmp > instruction_max)
            {
            instruction_max=tmp;
            }
            break;
            }
            
          }
          //IF NOT GO NEXT UNTIL NULL AND THAN GOT NEXT BRANCH
        else
        {
          iterator = iterator->next;
        }   
      }
    }
    weight_arr[instruction_num] =instruction_max;
  }
}


void push(struct Node** head, unsigned modified_register, unsigned  instruction_index, unsigned weight, src_t src)
{
   /* 1. create and allocate node */
   struct Node* newNode = new Node;
 
   /* 2. assign data to node */
   newNode->modified_register = modified_register;
   newNode->instruction_index = instruction_index;
   newNode->weight = weight;
   newNode->src = src;

   /* 3. set next of new node as head */
   newNode->next = (*head);
 
   /* 4. move the head to point to the new node */
   (*head) = newNode;
}

/* insert new node at the end of the linked list */
void append(struct Node** head, unsigned modified_register, unsigned  instruction_index, unsigned weight, src_t src)
{
  /* 1. create and allocate node */
  struct Node* newNode = new Node;
  
  struct Node *last = *head; /* used in step 5*/
  
  /* 2. assign data to the node */
  newNode->modified_register = modified_register;
  newNode->instruction_index = instruction_index;
  newNode->weight = weight;
  newNode->src = src;

  /* 3. set next pointer of new node to null as its the last node*/
  newNode->next = NULL;
  
  /* 4. if list is empty, new node becomes first node */
  if (*head == NULL)
  {
  *head = newNode;
  return;
  }
  
  /* 5. Else traverse till the last node */
  while (last->next != NULL)
  last = last->next;
  
  /* 6. Change the next of last node */
  last->next = newNode;
  return;
}
 
// display linked list contents
void displayList(struct Node *node)

{
   //traverse the list to display each node
   while (node != NULL)
   {
      cout<<node->instruction_index <<"-->";
      node = node->next;
   }
 
if(node== NULL)
cout<<"ENTRY" << "\n"; 

} 

void search_for_operand(struct Node ** branches, bool * found_operand, unsigned * found_branch_index, unsigned num_of_branches, unsigned operand, unsigned * found_branch_offset, unsigned * num_of_active_branches);
void add_operand_dependece_to_branch(struct Node ** dependency_list, unsigned found_branch_index, unsigned * num_of_active_branches, unsigned found_offset_in_branch, unsigned instruction_index, const unsigned int opsLatency[], const InstInfo progTrace[], src_t src);

void search_for_operand(struct Node ** branches, bool * found_operand, unsigned * found_branch_index, unsigned num_of_branches, unsigned operand, unsigned * found_branch_offset, unsigned * num_of_active_branches)
{
  unsigned branch_index = 0;
  unsigned branch_offset = 0;
  struct Node * branch_for_search = NULL;

  *found_operand = false;
  *found_branch_index = 0;
  *found_branch_offset = 0;

  for(branch_index = 0; branch_index < num_of_branches; ++branch_index)
  {
    branch_for_search = branches[branch_index];
    branch_offset = 0;
    while(NULL != branch_for_search)
    {
      if(operand == branch_for_search->modified_register)
      {
        *found_operand = true;
        *found_branch_index = branch_index;
        *found_branch_offset = branch_offset;
        break;
      }
      branch_offset++;
      branch_for_search = branch_for_search->next;
    }
  }
}

void add_operand_dependece_to_branch(struct Node ** dependency_list, unsigned found_branch_index, unsigned * num_of_active_branches, unsigned found_offset_in_branch, unsigned instruction_index, const unsigned int opsLatency[], const InstInfo progTrace[], src_t src)
{   
  struct Node* dependent_branch = NULL; 

  dependent_branch = dependency_list[found_branch_index];
  
  if (found_offset_in_branch == 0)
  {
    push(&dependency_list[found_branch_index], progTrace[instruction_index].dstIdx, instruction_index,  opsLatency[progTrace[instruction_index].opcode], src);

  }
  else
  {
    push(&dependency_list[*num_of_active_branches], progTrace[instruction_index].dstIdx, instruction_index,  opsLatency[progTrace[instruction_index].opcode], src);
    dependent_branch = dependency_list[found_branch_index];
    while( found_offset_in_branch > 0)
    {
      found_offset_in_branch--;
      dependent_branch = dependent_branch->next;
    }
    while(NULL != dependent_branch)
    {
      append(&dependency_list[*num_of_active_branches], dependent_branch->modified_register, dependent_branch->instruction_index, opsLatency[progTrace[instruction_index].opcode], dependent_branch->src);
      
      if ( NULL == dependent_branch->next)
      {
        break;
      }
      else
      {
        dependent_branch = dependent_branch->next;
      }    
    }
    (*num_of_active_branches)++;
  }
}


ProgCtx analyzeProg(const unsigned int opsLatency[], const InstInfo progTrace[], unsigned int numOfInsts) 
{
  // unsigned weight_arr[numOfInsts]={};
  // int dependency_arr[numOfInsts][2]={};
  unsigned instruction_index = 0;
  unsigned num_of_active_branches = 0;
  bool found_operand_1 = false;
  bool found_operand_2 = false;
  unsigned branch_index_of_operand_1 = 0;
  unsigned branch_index_of_operand_2 = 0;
  unsigned offset_in_branch_operand_1 = 0;
  unsigned offset_in_branch_operand_2 = 0;
  num_of_instructions = numOfInsts;

  dependency_arr = (int **)malloc(sizeof(int *) * numOfInsts);
  for (instruction_index = 0 ; instruction_index < numOfInsts; instruction_index++)
  {
    dependency_arr[instruction_index] = (int * )malloc(sizeof(int) * 2);
  }
  for (instruction_index = 0 ; instruction_index < numOfInsts; instruction_index++)
  {
    for (int i = 0; i < 2; i++)
    {
      dependency_arr[instruction_index][i] = -1;
    }
  }
  weight_arr = (unsigned *)malloc(sizeof(unsigned) * numOfInsts);
  dependency_list = (struct Node **)malloc(sizeof(struct Node *) * numOfInsts);
  for (instruction_index = 0; instruction_index < numOfInsts; ++instruction_index)
  {
    dependency_list[instruction_index] = NULL;
  }

  printf("=========== PROGRAM TRACE ==========\n");
  printf("    op     dst    src1     src2    latency\n");
  
  for (instruction_index = 0; instruction_index < numOfInsts; ++instruction_index)
  {
   
    printf("%d ) %d      %d        %d         %d     %d\n", instruction_index, progTrace[instruction_index].opcode, progTrace[instruction_index].dstIdx, progTrace[instruction_index].src1Idx, progTrace[instruction_index].src2Idx,  opsLatency[progTrace[instruction_index].opcode]);
    // (void)search_for_operand(dependency_list, &found_operand_1, &branch_index_of_operand_1, num_of_active_branches, progTrace[instruction_index].src1Idx, &offset_in_branch_operand_1,&num_of_active_branches);
    // (void)search_for_operand(dependency_list, &found_operand_2, &branch_index_of_operand_2, num_of_active_branches, progTrace[instruction_index].src2Idx, &offset_in_branch_operand_2,&num_of_active_branches);

    // if (!found_operand_1 & !found_operand_2)
    // {
    //   push(&dependency_list[num_of_active_branches], progTrace[instruction_index].dstIdx, instruction_index,  opsLatency[progTrace[instruction_index].opcode], RIGHT);
    //   num_of_active_branches++;
    // }
    // else if (found_operand_1 & !found_operand_2)
    // {
    //   (void)add_operand_dependece_to_branch(dependency_list, branch_index_of_operand_1, &num_of_active_branches, offset_in_branch_operand_1, instruction_index, opsLatency, progTrace, LEFT);
    // } 
    // else if (!found_operand_1 & found_operand_2)
    // {
    //   (void)add_operand_dependece_to_branch(dependency_list, branch_index_of_operand_2, &num_of_active_branches, offset_in_branch_operand_2, instruction_index, opsLatency, progTrace, RIGHT);
    // }   
    // else
    // {
    //   if (progTrace[instruction_index].src1Idx == progTrace[instruction_index].src2Idx)
    //   {
    //     (void)add_operand_dependece_to_branch(dependency_list, branch_index_of_operand_1, &num_of_active_branches, offset_in_branch_operand_1, instruction_index, opsLatency, progTrace, LEFT);
    //   }
    //   else
    //   {
    //     (void)add_operand_dependece_to_branch(dependency_list, branch_index_of_operand_1, &num_of_active_branches, offset_in_branch_operand_1, instruction_index, opsLatency, progTrace, LEFT);
    //     (void)add_operand_dependece_to_branch(dependency_list, branch_index_of_operand_2, &num_of_active_branches, offset_in_branch_operand_2, instruction_index, opsLatency, progTrace, RIGHT);
    //   }
    // }
        (void)search_for_operand(dependency_list, &found_operand_1, &branch_index_of_operand_1, num_of_active_branches, progTrace[instruction_index].src1Idx, &offset_in_branch_operand_1,&num_of_active_branches);
        if (found_operand_1  )
        {
          (void)add_operand_dependece_to_branch(dependency_list, branch_index_of_operand_1, &num_of_active_branches, offset_in_branch_operand_1, instruction_index, opsLatency, progTrace, LEFT);
        }
        (void)search_for_operand(dependency_list, &found_operand_2, &branch_index_of_operand_2, num_of_active_branches, progTrace[instruction_index].src2Idx, &offset_in_branch_operand_2,&num_of_active_branches);
        if (found_operand_2  )
        {
          (void)add_operand_dependece_to_branch(dependency_list, branch_index_of_operand_2, &num_of_active_branches, offset_in_branch_operand_2, instruction_index, opsLatency, progTrace, RIGHT);
        }

        if (!found_operand_1 & !found_operand_2)
        {
          push(&dependency_list[num_of_active_branches], progTrace[instruction_index].dstIdx, instruction_index,  opsLatency[progTrace[instruction_index].opcode], LEFT);
          num_of_active_branches++;
        }



  }
  sum_weight(dependency_list,  num_of_active_branches, opsLatency, progTrace,numOfInsts, weight_arr );
    // cout <<endl;
    // for (unsigned i=0 ; i<numOfInsts ; i++)
    // {
    //   cout << " depth in clocks :"<< i <<" ) " << weight_arr[i]<<endl;
    // }
    MAX_DEPTH=biggest_weight(weight_arr,numOfInsts, opsLatency) + opsLatency[progTrace[max_index].opcode] ;
    //cout <<endl<<  "MAX DEPTH= " << biggest_weight(weight_arr,numOfInsts) + opsLatency[progTrace[numOfInsts-1].opcode] << endl<<endl;
  
  show_dependencies(dependency_list,  num_of_active_branches, opsLatency, progTrace,numOfInsts, dependency_arr);
    //   cout <<endl<<"SHOW DEPENDECIES:"<<endl<<endl;
    //   for (unsigned h=0 ; h<numOfInsts ; h++)
    // {

    //   cout << h <<": " <<"A)"<< dependency_arr[h][0] <<"   "<<"B)"<< dependency_arr[h][1] <<endl;
      
    // }
    // cout<<endl;

  return dependency_list;

}

void freeProgCtx(ProgCtx ctx) 
{

}

int getInstDepth(ProgCtx ctx, unsigned int theInst)
{
  if( theInst < 0  || theInst >= num_of_instructions) 
  {
    return -1;
  }
    return weight_arr[theInst] ;
}

int getInstDeps(ProgCtx ctx, unsigned int theInst, int *src1DepInst, int *src2DepInst) 
{

  if( theInst < 0  || theInst >= num_of_instructions) 
   {
    return -1;
   }
   *src1DepInst = dependency_arr[theInst][0]; 
   *src2DepInst = dependency_arr[theInst][1];
    return 0;

    
}

int getProgDepth(ProgCtx ctx) 
{
  for (int i=0;i<51;i++)
  {
     displayList(dependency_list[i]);
  }
    // displayList(dependency_list[0]);
    // displayList(dependency_list[1]);
    // displayList(dependency_list[2]);
    // displayList(dependency_list[3]);
    // displayList(dependency_list[4]);
    // displayList(dependency_list[5]);
    // displayList(dependency_list[6]);
    // displayList(dependency_list[7]);
    // displayList(dependency_list[8]);
    // displayList(dependency_list[9]);
  
return MAX_DEPTH;
}


