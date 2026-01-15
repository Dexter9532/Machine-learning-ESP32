/**
 * @brief Interface for neural network functions
 */
#pragma once
#include "ctr/vector.hpp"

namespace ml::neural_network {

class Interface {

public:
  /**
   * @brief Delete the constructor
   */
  virtual ~Interface() noexcept = default;

  /**
   * @brief Prediction method to predict the network
   *
   * @param [in] input Reference to a double vector containing
   * the data the prediction should be based on.
   *
   * @return The predictet value.
   */
  virtual const ml::Matrix1d &predict(const ml::Matrix1d &input) = 0;
};

} // namespace ml::neural_network
