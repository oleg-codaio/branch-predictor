/*
 * Pin tool that implements two branch predictors:
 *   1) Simple 2-bit saturating counter predictor
 *   2) Perceptron-based predictor
 * To use, run 'make' to compile this Pin tool, and then run it with a target
 * binary to see how effective the two branch predictors are.
 *
 * Copyright 2016 Oleg Vaskevich (oleg@osv.im)
 */

#include <pin.H>

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "predictors/predictor.h"
#include "predictors/two_bit_saturating_counter_predictor.h"
#include "predictors/perceptron_predictor.h"

ostream *out = &cerr;
// Contains all the predictors that should be used when instrumentating a
// program with this Pin tool.
vector<bp::Predictor*> bps =
  {new bp::TwoBitSaturatingCounterPredictor(), new bp::PerceptronPredictor()};

// Runs the branch at the given address through each of the branch predictors
// with whether the branch was taken and its instruction mnemonic.
void ProcessBranch(ADDRINT pc, bool brTaken, void *arg) {
  string *mnemonic = reinterpret_cast<string *>(arg);
  for (bp::Predictor *bp : bps) {
    bp->GetPredictionAndUpdate(pc, brTaken, mnemonic);
  }
}

// Installs a call on conditional branches that then calls through to our
// branch predictors.
void InstrumentInstruction(INS ins, void *v) {
  if (INS_IsBranch(ins) && INS_HasFallThrough(ins)) {
    string *mnemonic = new string(INS_Mnemonic(ins));
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) ProcessBranch,
        IARG_INST_PTR, IARG_BRANCH_TAKEN, IARG_PTR, mnemonic, IARG_END);
  }
}

// Once the program has terminated, prints out all the branch predictor results
// and statistics.
void Finished(int code, void *v) {
  *out << endl << "Done! Results:" << endl << endl;
  for (bp::Predictor *bp : bps) {
    bp->PrintStatistics(out);
  }
}

int main(int argc, char *argv[]) {
  PIN_Init(argc, argv);
  INS_AddInstrumentFunction(InstrumentInstruction, 0);
  PIN_AddFiniFunction(Finished, 0);

  *out << "Running with the following branch predictor(s):" << endl;
  for (bp::Predictor *bp : bps) {
    *out << "  " << bp->get_name() << endl;
    // Due to the way Pin manages memory, we don't explicitly delete the branch
    // predictors here. For this reason, we also do not use unique_ptr in our
    // mnemonic map in PerceptronImpl.
  }
  *out << endl;

  PIN_StartProgram();

  return 0;
}
