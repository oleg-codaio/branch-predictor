/* Copyright 2016 Oleg Vaskevich */

#include "predictors/perceptron_predictor.h"
#include <string>

namespace bp {

namespace {
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
    int h = getHistoryIndex(i);
    y += weights_[key][i] * history_[h];
  }
  return y;
}

void PerceptronPredictor::TrainPerceptron(int key, int y, int t) {
  if (sign(y) != t || abs(y) <= kTheta) {
    if (abs(bias_[key] + t) < 1 << kWeightSize)
      bias_[key] += t;
    for (int i = 0; i < kHistorySize; ++i) {
      int h = getHistoryIndex(i);
      int w = weights_[key][i] + t * history_[h];
      if (abs(w) < 1 << kWeightSize) {
        weights_[key][i] = w;
      }
    }
  }

  history_[history_start_] = t;
  history_start_ = getHistoryIndex(1);
}
};  // namespace bp
