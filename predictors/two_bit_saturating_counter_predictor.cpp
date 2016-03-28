/* Copyright 2016 Oleg Vaskevich */

#include <string>

#include "predictors/two_bit_saturating_counter_predictor.h"

namespace bp {

TwoBitSaturatingCounterPredictor::TwoBitSaturatingCounterPredictor() {
  for (int i = 0; i < kSize; ++i) {
    states_[i] = WEAKLY_TAKEN;
  }
}

TwoBitState TwoBitSaturatingCounterPredictor::GetPrediction(int key) const {
  return states_[key];
}

// Implements the two-bit FSM.
void TwoBitSaturatingCounterPredictor::Update(int key, TwoBitState prediction,
    bool br_taken) {
  switch (states_[key]) {
    case STRONGLY_NOT_TAKEN:
      states_[key] = br_taken ? WEAKLY_NOT_TAKEN : STRONGLY_NOT_TAKEN;
      break;
    case WEAKLY_NOT_TAKEN:
      states_[key] = br_taken ? WEAKLY_TAKEN : STRONGLY_NOT_TAKEN;
      break;
    case WEAKLY_TAKEN:
      states_[key] = br_taken ? STRONGLY_TAKEN : WEAKLY_NOT_TAKEN;
      break;
    case STRONGLY_TAKEN:
      states_[key] = br_taken ? STRONGLY_TAKEN : WEAKLY_TAKEN;
      break;
  }
}
}  // namespace bp
