/**
 * @brief Cpp-file for neural network functions
 */
#include "ml/neural_network/single_layer.hpp"
#include "ctr/vector.hpp"
#include "ml/dense_layer/interface.hpp"

namespace ml::neural_network {

namespace {
constexpr size_t min(const size_t x, const size_t y) noexcept {
  return x <= y ? x : y;
}
} // namespace

//--------------------------------------------------------------------------------//
SingleLayer::SingleLayer(ml::dense_layer::Interface &hiddenLayer,
                         ml::dense_layer::Interface &outputLayer,
                         const ml::Matrix2d &trainInput,
                         const ml::Matrix2d &trainOutput)
    : myHiddenLayer{hiddenLayer}, myOutputLayer{outputLayer},
      myTrainInput{trainInput}, myTrainOutput{trainOutput},
      myTrainSetCount(
          static_cast<unsigned>(min(trainInput.size(), trainOutput.size()))) {}

//--------------------------------------------------------------------------------//
const ml::Matrix1d &SingleLayer::predict(const ml::Matrix1d &input) noexcept {
  myHiddenLayer.feedforward(
      input); // run feedforward on the hidden layer with the input values
  myOutputLayer.feedforward(
      myHiddenLayer.output()); // run feedforward on the output layer using
                               // the hidden layer's output as input
  return myOutputLayer.output();
}

//--------------------------------------------------------------------------------//
bool SingleLayer::train(size_t epochCount, double learningrate) noexcept {
  if ((0U == epochCount) || (0.0 >= learningrate)) {
    return false;
  }

  for (size_t epoch = 0; epoch < epochCount; ++epoch) {
    for (size_t k{}; k < myTrainSetCount; k++) {
      // (a) forward: hidden then output
      if (!myHiddenLayer.feedforward(myTrainInput[k])) {
        return false;
      }
      if (!myOutputLayer.feedforward(myHiddenLayer.output())) {
        return false;
      }

      // (b) backprop: output with target, then hidden with next layer
      if (!myOutputLayer.backpropagate(myTrainOutput[k])) {
        return false;
      }
      if (!myHiddenLayer.backpropagate(myOutputLayer)) {
        return false;
      }

      // (c) optimize: each layer with its own input source
      if (!myHiddenLayer.optimize(myTrainInput[k], learningrate)) {
        return false;
      }
      if (!myOutputLayer.optimize(myHiddenLayer.output(), learningrate)) {
        return false;
      }
    }
  }
  return true;
}

} // namespace ml::neural_network
