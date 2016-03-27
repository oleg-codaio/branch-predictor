/* Copyright 2016 Oleg Vaskevich */

#ifndef PREDICTORS_TWO_BIT_SATURATING_COUNTER_PREDICTOR_H_
#define PREDICTORS_TWO_BIT_SATURATING_COUNTER_PREDICTOR_H_

#include <pin.H>

#include <string>

#include "predictors/predictor.h"

namespace bp {

enum TwoBitState {STRONGLY_NOT_TAKEN, WEAKLY_NOT_TAKEN, WEAKLY_TAKEN,
    STRONGLY_TAKEN};

class TwoBitSaturatingCounterPredictor : public PredictorImpl<TwoBitState> {
  static const int kSize = 1024;
  TwoBitState states[kSize];

 public:
  TwoBitSaturatingCounterPredictor();
  string get_name() const override {
    int budget = kSize * 2;
    return "Two-bit saturating counter branch predictor (" +
        std::to_string(budget) + " b)";
  }

 private:
  TwoBitState GetPrediction(int key) const override;
  void Update(int key, TwoBitState prediction, bool br_taken) override;
  int getKey(ADDRINT pc) const override { return pc % kSize; };
  bool asBoolean(TwoBitState t) const override {
    return t == WEAKLY_TAKEN || t == STRONGLY_TAKEN;
  }
};

}  // namespace bp

#endif  // PREDICTORS_TWO_BIT_SATURATING_COUNTER_PREDICTOR_H_
