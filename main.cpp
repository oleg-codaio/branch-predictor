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
vector<bp::Predictor*> bps =
  {new bp::TwoBitSaturatingCounterPredictor(), new bp::PerceptronPredictor()};

int sign(int num) {
  return (0 < num) - (num < 0);
}

void ProcessBranch(ADDRINT pc, bool brTaken, void *arg) {
  string *mnemonic = reinterpret_cast<string *>(arg);
  for (bp::Predictor *bp : bps) {
    bp->GetPredictionAndUpdate(pc, brTaken, mnemonic);
  }
}

void InstrumentInstruction(INS ins, void *v) {
  // Instrument conditional branches.
  if (INS_IsBranch(ins) && INS_HasFallThrough(ins)) {
    string *mnemonic = new string(INS_Mnemonic(ins));
    /* *out << "Trying to instrument: " << mnemonic << ": 0x" << endl; */
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) ProcessBranch,
        IARG_INST_PTR, IARG_BRANCH_TAKEN, IARG_PTR, mnemonic, IARG_END);
  }
}

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
  }
  *out << endl;

  // Start the program.
  PIN_StartProgram();

  return 0;
}
