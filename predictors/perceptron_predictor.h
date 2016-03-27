/* Copyright 2016 Oleg Vaskevich */

#ifndef PREDICTORS_PERCEPTRON_PREDICTOR_H_
#define PREDICTORS_PERCEPTRON_PREDICTOR_H_

#include <pin.H>

#include <string>

#include "predictors/predictor.h"

namespace bp {

/// \brief Perceptron-based branch predictor.
/// Based on: https://www.cs.utexas.edu/~lin/papers/hpca01.pdf */
class PerceptronPredictor : public PredictorImpl<int> {
  static const int kSize = 20;
  static const int kHistorySize = 12;
  static const int kTheta = static_cast<int>(1.93 * kHistorySize + 14);
  static const int kWeightSize = 8;

  int history_start_ = 0;
  int history_[kHistorySize] = {-1};
  int bias_[kSize] = {1};
  int weights_[kSize][kHistorySize] = {{0}};

 public:
  PerceptronPredictor();
  string get_name() const override {
    int budget = (kSize + 1) * kHistorySize * kWeightSize + kHistorySize;
    return "Perceptron branch predictor (" + std::to_string(budget) + " b)";
  }

 private:
  int GetPrediction(int key) const override;
  void Update(int key, int prediction, bool br_taken) override;
  int getKey(ADDRINT pc) const override { return pc % kSize; };
  bool asBoolean(int t) const override { return t >= 0; };
  inline int getHistoryIndex(int i) const {
    return (i + history_start_) % kHistorySize;
  }
  int ComputePerceptron(int key) const;
  void TrainPerceptron(int key, int y, int t);
};

}  // namespace bp

#endif  // PREDICTORS_PERCEPTRON_PREDICTOR_H_
