/**
 * @brief Machine learning type definitions.
 */
#pragma once

#include "ctr/vector.hpp"

namespace ml {

/** One-dimensional matrix. */
using Matrix1d = ctr::Vector<double>;

/** Two-dimensional matrix. */
using Matrix2d = ctr::Vector<Matrix1d>;

/**
 * @brief Enumeration of activation functions.
 */
enum class ActFunc {
  Relu, ///< ReLU (Rectified Linear Unit) => y = x if x > 0 else 0.
  Tanh, ///< Tanh (hyperbolic tangent)    => -1 <= y <= 1.
};
} // namespace ml
