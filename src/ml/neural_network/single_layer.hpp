//! @note Bra!
/**
 * @brief Header-file for neural network functions
 */
#pragma once

#include "ctr/vector.hpp"
#include "ml/dense_layer/interface.hpp"
#include "ml/neural_network/interface.hpp"

namespace ml::neural_network {

class SingleLayer final : public ml::neural_network::Interface {

public:
  /**
   * @brief Constructor SignleLayer
   *
   * @param [in] hidden The hidden layer in the neural network.
   * @param [in] output The output layer in the neural network.
   * @param [in] trainInput The Input data that the model should train.
   * @param [in] trainOutput The output data that the model should be trained to
   * predict.
   */
  //! @note Denna metod kan med säkerhet markeras noexcept, då ni inte allokerar något minne,
  //!       kallar på några metoder som kastar undantag och gör det inte heller själva.
  explicit SingleLayer(ml::dense_layer::Interface &hiddenLayer,
                       ml::dense_layer::Interface &outputLayer,
                       const ml::Matrix2d &trainInput,
                       const ml::Matrix2d &trainOutput);

  /**
   * @brief Delete the constructor
   */
  ~SingleLayer() noexcept override = default;

  /**
   * @brief Prediction method to predict the network
   *
   * @param [in] input Reference to a double vector containing
   * the data the prediction should be based on.
   *
   * @return The predictet value.
   */
  const ml::Matrix1d &predict(const ml::Matrix1d &input) noexcept override;

  /**
   * @brief Train the model.
   *
   * @param [in] learningRate The speed the model should correct the errors in,
   * 0.01 as default.
   *
   * @return True if traingen is done, or False if not.
   */
  bool train(double learningrate = 0) noexcept;

  /**
   * @brief Check if the prediction is within tolerance for the training set.
   *
   * @return True if prediction is done, or False if not.
   */
  bool isPredictDone() noexcept;

  /**
   * @brief Get the amount of epochs used during training.
   *
   * @return The number of epochs used.
   */
  int getEpochsUsed() const noexcept;

  /**
   * @brief Delete not wanted constructors.
   */
  //! @note Snyggade til lite här.
  SingleLayer()                               = delete; // Delete without info.
  SingleLayer(const SingleLayer &)            = delete; // Delete copy constructor.
  SingleLayer &operator=(const SingleLayer &) = delete;  // Delete copy assignment.
  SingleLayer(SingleLayer &&)                 = delete;  // Delete move constructor.
  SingleLayer &operator=(SingleLayer &&)      = delete; // Delete move assignment.
private:
  // Reference for the hiddenlayer from the interface.
  ml::dense_layer::Interface &myHiddenLayer;

  // Reference for the outputlayer from the interface.
  ml::dense_layer::Interface &myOutputLayer;
  
   // Reference to the training input.
  const ml::Matrix2d &myTrainInput;

  // Reference to the trainingoutput.
  const ml::Matrix2d &myTrainOutput;

  // Indicates the amount of trainingsetups avalible.
  const unsigned myTrainSetCount;

  // To save the amount of epochs used.
  //! @note Initiera alla medlemsvariabler i konstruktorn i stället för här, försök att inte blanda
  //!       för strukturens skull.
  int myEpochsUsed{0};
};
} // namespace ml::neural_network
