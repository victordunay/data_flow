/* 046267 Computer Architecture - Winter 20/21 - HW #3               */
/* Implementation (skeleton)  for the dataflow statistics calculator */

#include <iostream>
#include "dflow_calc.h"
using namespace std;

class Graph {
    public:
    bool** adjMatrix;
    //**************
    unsigned int opcode;
    int src_reg_1;
    int src_reg_2;
    unsigned int latency;
    unsigned int depth;
    //**************
    int numVertices;

   public:
  // Initialize the matrix to zero
  Graph(int numVertices) {
    this->numVertices = numVertices;
    adjMatrix = new bool*[numVertices];
    for (int i = 0; i < numVertices; i++) 
    {
      adjMatrix[i] = new bool[numVertices];
      for (int j = 0; j < numVertices; j++)
        adjMatrix[i][j] = false;
    }
  }

  // Add edges
  void addEdge(int i, int j) {
    adjMatrix[i][j] = true;
   // adjMatrix[j][i] = true; // should remove , we need only directed graph
  }

  // Remove edges
  void removeEdge(int i, int j) {
    adjMatrix[i][j] = false;
  //  adjMatrix[j][i] = false; // should remove , we need only directed graph
  }

  // Print the martix
  void toString() {
    for (int i = 0; i < numVertices; i++) {
      cout << i << " : ";
      for (int j = 0; j < numVertices; j++)
        cout << adjMatrix[i][j] << " ";
      cout << "\n";
    }
  }

  ~Graph() {
    for (int i = 0; i < numVertices; i++)
      delete[] adjMatrix[i];
    delete[] adjMatrix;
  }
};



ProgCtx analyzeProg(const unsigned int opsLatency[], const InstInfo progTrace[], unsigned int numOfInsts) {

    Graph table(numOfInsts);
    for (unsigned int i = 0; i < numOfInsts; i++)
    {
        table.opcode = progTrace[i].opcode;
        table.src_reg_1 = progTrace[i].src1Idx  ;
        table.src_reg_2 = progTrace[i].src2Idx  ;
        table.latency = opsLatency[progTrace[i].opcode];
        table.addEdge(/*   THE ONE THAT DEPEND ON OTHERS */  i , table.src_reg_1/*DEPENDEE   */);
        table.addEdge(/*   THE ONE THAT DEPEND ON OTHERS */  i , table.src_reg_2/*DEPENDEE   */);
        cout << i<<")" << " opcode " << "=" << table.opcode   << "   | "<< 
         " src_reg_A "  <<"=" << table.src_reg_1   << "   | "<< 
         " src_reg_B "  <<"=" << table.src_reg_2   <<"   | "<< 
         " latency "  <<"=" << table.latency   << endl;
    }
    table.toString();

    return PROG_CTX_NULL;
}

void freeProgCtx(ProgCtx ctx) {
}

int getInstDepth(ProgCtx ctx, unsigned int theInst) {
    return -1;
}

int getInstDeps(ProgCtx ctx, unsigned int theInst, int *src1DepInst, int *src2DepInst) {
    return -1;
}

int getProgDepth(ProgCtx ctx) {
    return 0;
}


