#include "buttons/buttons.hpp"
#include "display/display.hpp"
#include "ml/dense_layer/dense_layer.hpp"
#include "ml/neural_network/single_layer.hpp"
#include "ml/types.hpp"
#include <cstdint>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

extern "C" int main(void) {
  display_init();
  buttons_init_irq();
  display_set_value(0);

  uint8_t lastDigit = 0;

  constexpr size_t inputCount{3U};
  constexpr size_t hiddenCount{3U};
  constexpr size_t outputCount{1U};

  constexpr size_t epochCount{10000};
  constexpr double learningRate{0.1};

  const ml::Matrix2d trainInputSets{
      ml::Matrix1d{0.0, 0.0, 0.0}, ml::Matrix1d{0.0, 0.0, 1.0},
      ml::Matrix1d{0.0, 1.0, 0.0}, ml::Matrix1d{0.0, 1.0, 1.0},
      ml::Matrix1d{1.0, 0.0, 0.0}, ml::Matrix1d{1.0, 0.0, 1.0},
      ml::Matrix1d{1.0, 1.0, 0.0}, ml::Matrix1d{1.0, 1.0, 1.0}};

  const ml::Matrix2d trainOutputSets{ml::Matrix1d{0.0}, ml::Matrix1d{1.0},
                                     ml::Matrix1d{2.0}, ml::Matrix1d{3.0},
                                     ml::Matrix1d{4.0}, ml::Matrix1d{5.0},
                                     ml::Matrix1d{6.0}, ml::Matrix1d{7.0}};

  ml::dense_layer::DenseLayer hiddenLayer{hiddenCount, inputCount};
  ml::dense_layer::DenseLayer outputLayer{outputCount, hiddenCount};
  ml::neural_network::SingleLayer network{hiddenLayer, outputLayer,
                                          trainInputSets, trainOutputSets};

  // Train the model once before starting the logic loop, return -1 on failure.
  if (!network.train(epochCount, learningRate)) {
    return -1;
  }

  ml::Matrix1d trainInput{0.0, 0.0, 0.0};

  while (1) {
    // Continuously monitor the buttons.
    trainInput[0] = static_cast<double>(button0_get());
    trainInput[1] = static_cast<double>(button1_get());
    trainInput[2] = static_cast<double>(button2_get());

    // The output vector only consists of one value, which is the digit to
    // display.
    const double output{network.predict(trainInput)[0]};

    // Extract the digit, round to the nearest integer.
    const uint8_t digit{static_cast<uint8_t>(output + 0.5)};

    // Update the displayed digit on change.
    if (digit != lastDigit) {
      lastDigit = digit;
      display_set_value(digit);
    }
    k_msleep(10);
  }
}
