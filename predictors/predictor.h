/* Copyright 2016 Oleg Vaskevich */

#ifndef PREDICTORS_PREDICTOR_H_
#define PREDICTORS_PREDICTOR_H_

#include <pin.H>

#include <ostream>
#include <map>
#include <string>

namespace bp {

// Abstract class that provides a high-level interface for branch prediction.
class Predictor {
 public:
  virtual ~Predictor() {}
  // Gets the prediction for whether the branch at address pc is expected to be
  // taken. Furthermore, updates the underlying branch predictor model with
  // whether the branch was actually taken (br_taken) and its instruction
  // mnemonic.
  virtual bool GetPredictionAndUpdate(ADDRINT pc, bool br_taken,
                                      string *mnemonic) = 0;
  // Prints statistics pertaining to this predictor to the provided output
  // stream, including: the total, predicted, and actual number of branches; the
  // top two mispredicted branches; and the prediction ratio of this predictor.
  virtual void PrintStatistics(std::ostream *out) const = 0;
  // Returns the human-readable name of this predictor.
  virtual string get_name() const = 0;
};

// Templated branch predictor base class that should be inherited by branch
// predictor implementations.
template <typename T>
class PredictorImpl : public Predictor {
  // Contains the addresses of the top two mispredicted branches.
  ADDRINT max_mispredicted_[2];
  // Maps branch addresses to the number of times the corresponding branch was
  // mispredicted.
  map <ADDRINT, int> mispredicted_count_;
  // Mpas branch addresses to the corresponding instruction mnemonics.
  map <ADDRINT, string*> mispredicted_instrs_;
  // The total number of branches seen by this branch predictor.
  int num_branches_ = 0;
  // The number of correctly-predicted branches.
  int correct_branches_ = 0;
  // The number of branches that were actually taken.
  int taken_branches_ = 0;

 public:
  bool GetPredictionAndUpdate(ADDRINT pc, bool br_taken,
                              string *mnemonic) final;
  void PrintStatistics(std::ostream *out) const final;

 private:
  // Given the key derived from the branch address, returns the raw prediction
  // value used by the implementation.
  virtual T GetPrediction(int key) const = 0;
  // Updates the underlying model for the given key with the previously-computed
  // prediction and whether the branch was taken (br_taken).
  virtual void Update(int key, T prediction, bool br_taken) = 0;
  // Computes the key, later used by the GetPrediction and Update methods,
  // from the branch program counter address.
  virtual int get_key(ADDRINT pc) const = 0;
  // Converts the raw value used by this branch predictor implementation into
  // a boolean value denoting whether the branch is predicted to be taken.
  virtual bool as_boolean(T t) const = 0;
};

}  // namespace bp

#endif  // PREDICTORS_PREDICTOR_H_
