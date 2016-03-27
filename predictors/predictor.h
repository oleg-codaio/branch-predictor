/* Copyright 2016 Oleg Vaskevich */

#ifndef PREDICTORS_PREDICTOR_H_
#define PREDICTORS_PREDICTOR_H_

#include <pin.H>

#include <ostream>
#include <map>
#include <string>

namespace bp {

class Predictor {
 public:
  virtual bool GetPredictionAndUpdate(ADDRINT pc, bool br_taken,
                                      string *mnemonic) = 0;
  virtual void PrintStatistics(std::ostream *out) const = 0;
  virtual string get_name() const = 0;
};

template <typename T>
class PredictorImpl : public Predictor {
  ADDRINT max_mispredicted_[2];
  map <ADDRINT, int> mispredicted_count_;
  map <ADDRINT, string*> mispredicted_instrs_;
  int num_branches_ = 0;
  int correct_branches_ = 0;
  int taken_branches_ = 0;

 public:
  bool GetPredictionAndUpdate(ADDRINT pc, bool br_taken,
                              string *mnemonic) final;
  void PrintStatistics(std::ostream *out) const final;

 private:
  virtual T GetPrediction(int key) const = 0;
  virtual void Update(int key, T prediction, bool br_taken) = 0;
  virtual int getKey(ADDRINT key) const = 0;
  virtual bool asBoolean(T t) const = 0;
};

}  // namespace bp

#endif  // PREDICTORS_PREDICTOR_H_
