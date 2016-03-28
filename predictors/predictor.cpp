/* Copyright 2016 Oleg Vaskevich */

#include <pin.H>

#include <string>

#include "predictors/predictor.h"
#include "predictors/two_bit_saturating_counter_predictor.h"

namespace bp {

template <typename T>
bool PredictorImpl<T>::GetPredictionAndUpdate(
    ADDRINT pc, bool br_taken, string *mnemonic) {
  int key = get_key(pc);
  T pred = GetPrediction(key);
  bool pred_bool = as_boolean(pred);

  ++num_branches_;

  if (br_taken) {
    ++taken_branches_;
  }

  if (pred_bool == br_taken) {
    // The prediction lined up with whether the branch was actually taken.
    ++correct_branches_;
  } else {
    // The branch was mispredicted.
    ++mispredicted_count_[pc];
    mispredicted_instrs_[pc] = mnemonic;

    // Update worst instructions.
    if (mispredicted_count_[pc] > mispredicted_count_[max_mispredicted_[0]]
        || !max_mispredicted_[0]) {
      max_mispredicted_[1] = max_mispredicted_[0];
      max_mispredicted_[0] = pc;
    }
  }

  Update(key, pred, br_taken);

  return pred_bool;
}

template <typename T>
void PredictorImpl<T>::PrintStatistics(std::ostream *out) const {
  *out << get_name() << " analysis results: " << endl;
  *out << "Number of branches: " << num_branches_ << endl;
  *out << "Number of correctly-predicted branches: " << correct_branches_ <<
      endl;
  *out << "Number of taken branches: " << taken_branches_ << endl;
  *out << "Worst branches:" << endl;
  for (int i = 0; i < 2; ++i) {
    if (max_mispredicted_[i]) {
        *out << "  0x" << std::hex << max_mispredicted_[i] << std::dec <<
            " (" << mispredicted_count_.at(max_mispredicted_[i]) <<
            " times): " << *mispredicted_instrs_.at(max_mispredicted_[i]) <<
            endl;
    }
  }
  *out << "Success ratio: " << correct_branches_ * 100.00 / num_branches_ <<
      "%" << endl;
  *out << "-----------" << endl;
}

template class PredictorImpl<TwoBitState>;
template class PredictorImpl<int>;
}  // namespace bp
