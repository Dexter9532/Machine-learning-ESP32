# Machine Learning ESP32-S3 TFT

Requierments:
 - https://docs.zephyrproject.org/latest/develop/getting_started/index.html
 - https://learn.adafruit.com/adafruit-esp32-s3-tft-feather/overview

Clone repo inside zephyrproject:
```
cd /path/to/zephyrproject &&
git clone git@github.com:Dexter9532/Machine-learning-ESP32.git
```

Build image:
```
west build -p always \
  -b adafruit_feather_esp32s3_tft_reverse/esp32s3/procpu \
  -d name/build \
  name/
```

Flash image:
```
cd name/build && west flash
```

## Debug:

To view output run:
```
west espressif monitor
```

## Clang-format

The project uses clang-format.

Run format check:
```
find src -type f \( -name "*.c" -o -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \) \
  -exec clang-format --dry-run -Werror {} +
```

Run formatter:
```
find src -type f \( -name "*.c" -o -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \) \
  -exec clang-format -i {} +
```
