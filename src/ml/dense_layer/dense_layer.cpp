/**
 * @brief Dense layer implementation details.
 */
#include <cstddef>
#include <math.h>
#include <stdlib.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include "ml/dense_layer/dense_layer.hpp"
#include "ml/types.hpp"

namespace ml::dense_layer {
namespace {
// -----------------------------------------------------------------------------
void initRandom() noexcept {
  // Static variable to track whether random generator has been seeded.
  static bool initialized{false};

  // Terminate the function if already initialized to avoid reseeding.
  if (initialized) {
    return;
  }

  // Set up random numbers using the current time as a starting point.
  srand((unsigned)k_cycle_get_32());

  // Mark as initialized to prevent future reseeding.
  initialized = true;
}

// -----------------------------------------------------------------------------
double randomStartVal() noexcept {
  // Generate random weight initialization value in range [0.0, 1.0].
  // Cast to double ensures floating-point division for precision.
  return static_cast<double>(rand()) / RAND_MAX;
}

// -----------------------------------------------------------------------------
double actFuncOutput(const ml::ActFunc actFunc, const double input) noexcept {
  // Compute activation function output for the given input value.
  switch (actFunc) {
  case ml::ActFunc::Relu:
    // ReLU: f(x) = max(0, x) - return input if positive, zero otherwise.
    return 0.0 < input ? input : 0.0;
  case ml::ActFunc::Tanh:
    // Hyperbolic tangent: f(x) = tanh(x) - output range [-1, 1].
    return tanh(input);
  default:
    printk("invalid activation function\n");
    return 0.0;
  }
}

// -----------------------------------------------------------------------------
double actFuncDelta(const ml::ActFunc actFunc, const double input) noexcept {
  // Calculate how much the activation function changes (needed for learning).
  switch (actFunc) {
  case ml::ActFunc::Relu:
    // ReLU derivative: f'(x) = 1 if x > 0, else 0.
    return 0.0 < input ? 1.0 : 0.0;
  case ml::ActFunc::Tanh:
    // Tanh derivative: f'(x) = 1 - tanh²(x).
    return 1.0 - tanh(input) * tanh(input);
  default:
    printk("invalid activation function\n");
    return 0.0;
  }
}
} // namespace

// -----------------------------------------------------------------------------
DenseLayer::DenseLayer(const size_t nodeCount, const size_t weightCount,
                       const ml::ActFunc actFunc)
    : myOutput{}, myError{}, myBias{}, myWeights{}, myActFunc{actFunc} {
  // Make sure we have at least 1 node and 1 weight per node.
  if ((0U == nodeCount) || (0U == weightCount)) {
    printk("invalid dense layer parameters\n");
    while (1) {
    }
  }

  myOutput.resize(nodeCount);
  myError.resize(nodeCount);
  myBias.resize(nodeCount);
  myWeights.resize(nodeCount);

  // Initialize the random number generator (only done once).
  initRandom();

  // Initialize all biases and weights with random starting values.
  for (size_t i{}; i < nodeCount; ++i) {

    myOutput[i] = 0.0;
    myError[i] = 0.0;
    myBias[i] = randomStartVal();

    ml::Matrix1d weights(weightCount);
    myWeights[i] = weights;

    for (size_t j{}; j < weightCount; ++j) {
      myWeights[i][j] = randomStartVal();
    }
  }
}

// -----------------------------------------------------------------------------
size_t DenseLayer::nodeCount() const noexcept {
  // Return the number of nodes in this layer.
  return myOutput.size();
}

// -----------------------------------------------------------------------------
size_t DenseLayer::weightCount() const noexcept {
  // Return the number of weights per node (same for all nodes).
  return myWeights[0U].size();
}

// -----------------------------------------------------------------------------
const ml::Matrix1d &DenseLayer::output() const noexcept {
  // Return read-only access to layer's output values.
  return myOutput;
}

// -----------------------------------------------------------------------------
const ml::Matrix1d &DenseLayer::error() const noexcept {
  // Return read-only access to layer's error values.
  return myError;
}

// -----------------------------------------------------------------------------
const ml::Matrix1d &DenseLayer::bias() const noexcept {
  // Return read-only access to layer's bias values.
  return myBias;
}

// -----------------------------------------------------------------------------
const ml::Matrix2d &DenseLayer::weights() const noexcept {
  // Return read-only access to layer's weights.
  return myWeights;
}

// -----------------------------------------------------------------------------
bool DenseLayer::feedforward(const ml::Matrix1d &input) noexcept {
  // Validate that we have the correct number of inputs.

  if (input.size() != weightCount()) {
    printk("input dimension mismatch: expected %u actual %u\n",
           (unsigned)weightCount(), (unsigned)input.size());
    return false;
  }

  // Compute the output value for each node in this layer.
  for (size_t i{}; i < nodeCount(); ++i) {
    // Start with the bias (like a starting point for each node).
    auto sum{myBias[i]};

    // Add up all the weighted inputs (input * weight for each connection).
    for (size_t j{}; j < weightCount(); ++j) {
      sum += input[j] * myWeights[i][j];
    }
    // Pass the sum through the activation function to get the final output.
    myOutput[i] = actFuncOutput(myActFunc, sum);
  }
  return true;
}

// -----------------------------------------------------------------------------
bool DenseLayer::backpropagate(const ml::Matrix1d &reference) noexcept {
  // Validate reference vector size matches number of output nodes.
  if (reference.size() != nodeCount()) {
    printk("output dimension mismatch: expected %u actual %u\n",
           (unsigned)nodeCount(), (unsigned)reference.size());
    return false;
  }

  // Compute error gradients for each node (this is for the output layer).
  for (size_t i{}; i < nodeCount(); ++i) {
    // Calculate prediction error: target - actual output.
    const auto rawError{reference[i] - myOutput[i]};

    // Apply chain rule: multiply by activation function derivative.
    // This determines how much to adjust weights and biases.
    myError[i] = rawError * actFuncDelta(myActFunc, myOutput[i]);
  }
  return true;
}

// -----------------------------------------------------------------------------
bool DenseLayer::backpropagate(const Interface &nextLayer) noexcept {
  // Validate that the layers connect properly.
  if (nextLayer.weightCount() != nodeCount()) {
    printk("layer dimension mismatch: expected %u actual %u\n",
           (unsigned)nodeCount(), (unsigned)nextLayer.weightCount());
    return false;
  }

  // Compute error gradients for each node (this is for hidden layers).
  for (size_t i{}; i < nodeCount(); ++i) {
    double weightedErrorSum{};

    // Accumulate weighted error contributions from the next layer.
    // Each connection propagates error back through its weight.
    for (size_t j{}; j < nextLayer.nodeCount(); ++j) {
      // Add error contribution: next_layer_error * connecting_weight.
      weightedErrorSum += nextLayer.error()[j] * nextLayer.weights()[j][i];
    }
    // Apply chain rule: multiply by activation function derivative.
    // This determines how much to adjust this node's weights and biases.
    myError[i] = weightedErrorSum * actFuncDelta(myActFunc, myOutput[i]);
  }
  return true;
}

// -----------------------------------------------------------------------------
bool DenseLayer::optimize(const ml::Matrix1d &input,
                          const double learningRate) noexcept {
  // Validate learning rate and input dimensions.
  if (0.0 >= learningRate) {
    printk("invalid learning rate\n");
    return false;
  }
  if (input.size() != weightCount()) {
    printk("input dimension mismatch: expected %u actual %u\n",
           (unsigned)weightCount(), (unsigned)input.size());
    return false;
  }

  // Update parameters using gradient descent to minimize error.
  for (size_t i{}; i < nodeCount(); ++i) {
    // Update bias: bias += error * learning_rate.
    myBias[i] += myError[i] * learningRate;

    // Update weights: larger inputs contribute more to weight changes.
    for (size_t j{}; j < weightCount(); ++j) {
      // Update weight: weight += error * learning_rate * input_value.
      myWeights[i][j] += myError[i] * learningRate * input[j];
    }
  }
  // Return true to indicate success.
  return true;
}
} // namespace ml::dense_layer
// namespace ml::denser
