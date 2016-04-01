/* Copyright 2016 Oleg Vaskevich */

#ifndef PREDICTORS_PERCEPTRON_PREDICTOR_H_
#define PREDICTORS_PERCEPTRON_PREDICTOR_H_

#include <pin.H>

#include <string>

#include "predictors/predictor.h"

namespace bp {

// Perceptron-based branch predictor. Uses a global history shift register along
// with multiple perceptrons in order to train and predict whether a given
// branch is taken. Based on: https://www.cs.utexas.edu/~lin/papers/hpca01.pdf
class PerceptronPredictor : public PredictorImpl<int> {
  // The number of perceptrons.
  static const int kSize = 20;
  // The size of the global register shift register, emulated here as a circular
  // buffer.
  static const int kHistorySize = 12;
  // The theta value used as the threshold for determining weight saturation.
  static const int kTheta = static_cast<int>(1.93 * kHistorySize + 14);
  // The number of bits each weight can use.
  static const int kWeightSize = 8;

  // The start offset of history_.
  int history_start_ = 0;
  // The circular buffer implementation of the global history shift register.
  int history_[kHistorySize] = {-1};
  // The bias, i.e., w_0 for each perceptron.
  int bias_[kSize] = {1};
  // The array of perceptrons.
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
  int get_key(ADDRINT pc) const override { return pc % kSize; };
  // Returns true if the output y value is greater than 0.
  bool as_boolean(int t) const override { return t >= 0; };
  // Given an index in the GHR, returns the circular buffer index.
  inline int get_history_index(int i) const {
    return (i + history_start_) % kHistorySize;
  }
  // Computes the y value of the perceptron with the given key.
  int ComputePerceptron(int key) const;
  // Trains the perceptron with the given key, given its previously-computed
  // y value as well as t, denoting whether or not the branch was taken.
  void TrainPerceptron(int key, int y, int t);
};

}  // namespace bp

#endif  // PREDICTORS_PERCEPTRON_PREDICTOR_H_
