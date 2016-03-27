# Branch Prediction Pin Tool
This repository provides a Pin tool that can be used for performing branch
prediction studies, i.e., evaluating different branch prediction algorithms.
It uses [Pin][1] for instrumentation.

## Implemented algorithms
This tool provides two ready-to-use implementations of branch predictors:

1. Two-bit saturating counter branch predictor
2. Perceptron branch predictor ([link][2])

Additional branch predictors can be implemented by inheriting from the Predictor
abstract class.

## Running
To run, make sure you have Pin installed. You can then compile:

    export PIN_ROOT=/path/to/pin
    make

and instrument any executable:

    sudo pin -t branch-predictor/obj-ia32/branch_predictor.so -- /usr/bin/curl google.com

[1]: https://software.intel.com/en-us/articles/pin-a-dynamic-binary-instrumentation-tool
[2]: https://www.cs.utexas.edu/~lin/papers/hpca01.pdf
