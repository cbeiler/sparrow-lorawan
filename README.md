# Sparrow for LoRaWAN

Sample code for communicating with LoRaWAN Gateways and network servers using
Blues [Sparrow](https://blues.io/products/sparrow-iot-sensor-clusters-over-lora/)
devices. This sample application has been tested with
[The Things Network](https://nam1.cloud.thethings.network/console/) and
[Helium](https://console.helium.com/).

## Getting Started

1. Clone this repo

  ```bash
  git clone https://github.com/blues/sparrow-lorawan.git
  ```

2. Open the `Application/STM32CubeIDE` in the STM32CubeIDE.

3. Open `config.h` and modify `CONFIG_DEVEUI`, `CONFIG_APPEUI`, and `CONFIG_APPKEY` based on your LoRaWAN network server and app settings.

4. Build using `STM32CubeIDE` and deploy to a Sparrow device using an ST-Link programmer.

5. By default, this sample sends dummy data every 10 seconds, as defined in `sensor.c`. Once you have things up and running, modify to add your own app data!


