/* 046267 Computer Architecture - Winter 20/21 - HW #3               */
/* Implementation (skeleton)  for the dataflow statistics calculator */

#include <iostream>
#include "dflow_calc.h"

using namespace std;

struct Node ** dependency_list = NULL; 

struct Node
{
   unsigned instruction_index;
   unsigned modified_register;
   unsigned weight;
   struct Node *next;
};

void push(struct Node** head, unsigned modified_register, unsigned  instruction_index, unsigned weight)
{
   /* 1. create and allocate node */
   struct Node* newNode = new Node;
 
   /* 2. assign data to node */
   newNode->modified_register = modified_register;
   newNode->instruction_index = instruction_index;
   newNode->weight = weight;

   /* 3. set next of new node as head */
   newNode->next = (*head);
 
   /* 4. move the head to point to the new node */
   (*head) = newNode;
}

/* insert new node at the end of the linked list */
void append(struct Node** head, unsigned modified_register, unsigned  instruction_index, unsigned weight)
{
/* 1. create and allocate node */
struct Node* newNode = new Node;
 
struct Node *last = *head; /* used in step 5*/
 
/* 2. assign data to the node */
newNode->modified_register = modified_register;
newNode->instruction_index = instruction_index;
newNode->weight = weight;

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

void search_for_operand(struct Node ** branches, bool * found_operand, unsigned * found_branch_index, unsigned num_of_branches, unsigned operand, unsigned * found_branch_offset);
void add_operand_dependece_to_branch(struct Node ** dependency_list, unsigned found_branch_index, unsigned * num_of_active_branches, unsigned found_offset_in_branch, unsigned instruction_index, const unsigned int opsLatency[], const InstInfo progTrace[]);

void search_for_operand(struct Node ** branches, bool * found_operand, unsigned * found_branch_index, unsigned num_of_branches, unsigned operand, unsigned * found_branch_offset)
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
      }
      branch_offset++;
      branch_for_search = branch_for_search->next;
    }
  }
}

void add_operand_dependece_to_branch(struct Node ** dependency_list, unsigned found_branch_index, unsigned * num_of_active_branches, unsigned found_offset_in_branch, unsigned instruction_index, const unsigned int opsLatency[], const InstInfo progTrace[])
{   
  struct Node* dependent_branch = NULL; 

  dependent_branch = dependency_list[found_branch_index];
  
  if (found_offset_in_branch == 0)
  {
    push(&dependency_list[found_branch_index], progTrace[instruction_index].dstIdx, instruction_index,  opsLatency[progTrace[instruction_index].opcode]);
  }
  else
  {
    push(&dependency_list[*num_of_active_branches], progTrace[instruction_index].dstIdx, instruction_index,  opsLatency[progTrace[instruction_index].opcode]);
    dependent_branch = dependency_list[found_branch_index];
    while( found_offset_in_branch > 0)
    {
      found_offset_in_branch--;
      dependent_branch = dependent_branch->next;
    }
    while(NULL != dependent_branch)
    {
      append(&dependency_list[*num_of_active_branches], dependent_branch->modified_register, dependent_branch->instruction_index, opsLatency[progTrace[instruction_index].opcode]);
      
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
  unsigned instruction_index = 0;
  unsigned num_of_active_branches = 0;
  bool found_operand_1 = false;
  bool found_operand_2 = false;
  unsigned branch_index_of_operand_1 = 0;
  unsigned branch_index_of_operand_2 = 0;
  unsigned offset_in_branch_operand_1 = 0;
  unsigned offset_in_branch_operand_2 = 0;

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
    (void)search_for_operand(dependency_list, &found_operand_1, &branch_index_of_operand_1, num_of_active_branches, progTrace[instruction_index].src1Idx, &offset_in_branch_operand_1);
    (void)search_for_operand(dependency_list, &found_operand_2, &branch_index_of_operand_2, num_of_active_branches, progTrace[instruction_index].src2Idx, &offset_in_branch_operand_2);

    if (!found_operand_1 & !found_operand_2)
    {
      push(&dependency_list[num_of_active_branches], progTrace[instruction_index].dstIdx, instruction_index,  opsLatency[progTrace[instruction_index].opcode]);
      num_of_active_branches++;
    }
    else if (found_operand_1 & !found_operand_2)
    {
      (void)add_operand_dependece_to_branch(dependency_list, branch_index_of_operand_1, &num_of_active_branches, offset_in_branch_operand_1, instruction_index, opsLatency, progTrace);
    } 
    else if (!found_operand_1 & found_operand_2)
    {
      (void)add_operand_dependece_to_branch(dependency_list, branch_index_of_operand_2, &num_of_active_branches, offset_in_branch_operand_2, instruction_index, opsLatency, progTrace);
    }   
    else
    {
      (void)add_operand_dependece_to_branch(dependency_list, branch_index_of_operand_1, &num_of_active_branches, offset_in_branch_operand_1, instruction_index, opsLatency, progTrace);
      (void)add_operand_dependece_to_branch(dependency_list, branch_index_of_operand_2, &num_of_active_branches, offset_in_branch_operand_2, instruction_index, opsLatency, progTrace);
    }
  }

  return dependency_list;
}

void freeProgCtx(ProgCtx ctx) {
}

int getInstDepth(ProgCtx ctx, unsigned int theInst)
{

    return -1;
}

int getInstDeps(ProgCtx ctx, unsigned int theInst, int *src1DepInst, int *src2DepInst) {
    return -1;
}

int getProgDepth(ProgCtx ctx) 
{
    displayList(dependency_list[0]);
    displayList(dependency_list[1]);
    displayList(dependency_list[2]);
    displayList(dependency_list[3]);
    displayList(dependency_list[4]);
    displayList(dependency_list[5]);
    displayList(dependency_list[6]);
    displayList(dependency_list[7]);
    displayList(dependency_list[8]);
    displayList(dependency_list[9]);


    return 0;
}


