/**
 * @brief Dense layer interface.
 */
#pragma once

#include "ml/types.hpp"

namespace ml::dense_layer {
/**
 * @brief Dense layer interface.
 */
class Interface {
public:
  /**
   * @brief Delete the dense layer.
   */
  virtual ~Interface() noexcept = default;

  /**
   * @brief Get the number of nodes in the dense layer.
   *
   * @return The number of nodes in the dense layer.
   */
  virtual size_t nodeCount() const = 0;

  /**
   * @brief Get the number of weights per node in the dense layer.
   *
   * @return The number of weights per node in the dense layer.
   */
  virtual size_t weightCount() const = 0;

  /**
   * @brief Get the output values of the dense layer.
   *
   * @return Vector holding the output values of the dense layer.
   */
  virtual const ml::Matrix1d &output() const = 0;

  /**
   * @brief Get the error values of the dense layer.
   *
   * @return Vector holding the error values of the dense layer.
   */
  virtual const ml::Matrix1d &error() const = 0;

  /**
   * @brief Get the bias values of the dense layer.
   *
   * @return Vector holding the bias values of the dense layer.
   */
  virtual const ml::Matrix1d &bias() const = 0;

  /**
   * @brief Get the weights of the dense layer.
   *
   * @return Vector holding the weights of the dense layer.
   */
  virtual const ml::Matrix2d &weights() const = 0;

  /**
   * @brief Perform feedforward with the given input.
   *
   * @param[in] input Input values with which to perform feedforward.
   *
   * @return True if feedforward was performed, or false on error.
   */
  virtual bool feedforward(const ml::Matrix1d &input) = 0;

  /**
   * @brief Perform backpropagation with the given reference values.
   *
   *        This method is appropriate for output layers only.
   *
   * @param[in] reference Reference values with which to perform
   * backpropagation.
   *
   * @return True if backpropagation was performed, or false on error.
   */
  virtual bool backpropagate(const ml::Matrix1d &reference) = 0;

  /**
   * @brief Perform backpropagation with the given next layer.
   *
   *        This method is appropriate for hidden layers only.
   *
   * @param[in] nextLayer The next consecutive layer.
   *
   * @return True if backpropagation was performed, or false on error.
   */
  virtual bool backpropagate(const Interface &nextLayer) = 0;

  /**
   * @brief Perform optimization with the given input.
   *
   * @param[in] input Input values with which to perform optimization.
   * @param[in] learningRate Learning rate to use for optimization.
   *
   * @return True if optimization was performed, or false on error.
   */
  virtual bool optimize(const ml::Matrix1d &input,
                        const double learningRate) = 0;
};
} // namespace ml::dense_layer
