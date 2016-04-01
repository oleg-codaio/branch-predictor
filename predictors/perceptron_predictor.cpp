/* Copyright 2016 Oleg Vaskevich */

#include "predictors/perceptron_predictor.h"
#include <string>

namespace bp {

namespace {
  // Returns the sign of the given value.
  int sign(int val) {
    return (val > 0) - (val < 0);
  }
}

PerceptronPredictor::PerceptronPredictor() {
}

int PerceptronPredictor::GetPrediction(int key) const {
  return ComputePerceptron(key);
}

void PerceptronPredictor::Update(int key, int prediction, bool br_taken) {
  TrainPerceptron(key, prediction, br_taken ? 1 : -1);
}

int PerceptronPredictor::ComputePerceptron(int key) const {
  int y = bias_[key];
  for (int i = 0; i < kHistorySize; ++i) {
    int h = get_history_index(i);
    y += weights_[key][i] * history_[h];
  }
  return y;
}

void PerceptronPredictor::TrainPerceptron(int key, int y, int t) {
  if (sign(y) != t || abs(y) <= kTheta) {
    int ws = 1 << (kWeightSize - 1);
    int b = bias_[key] + t;
    if (abs(b) < ws) {
      bias_[key] = b;
    }
    for (int i = 0; i < kHistorySize; ++i) {
      int h = get_history_index(i);
      int w = weights_[key][i] + t * history_[h];
      if (abs(w) < ws) {
        weights_[key][i] = w;
      }
    }
  }

  history_[history_start_] = t;
  history_start_ = get_history_index(1);
}
};  // namespace bp
