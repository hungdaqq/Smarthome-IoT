# NodeMCU ESP8266 programming environment Platform.io

PlatformIO is an open-source ecosystem for IoT development that can be used within the Visual Studio Code (VSCode) editor. It provides a platform for developing, building, deploying, and managing software for microcontrollers and other embedded systems.

## Prerequisites

1. Visual Studio Code
2. The PlatformIO extension for Visual Studio Code
3. Operating System: Windows, macOS, Linux, FreeBSD, Linux ARMv6+
4. Python Interpreter: Python 3.6+ or above
5. Access to Serial Ports (USB/UART)
6. Important libraries: ThingsBoard (Arduino SDK), ArduinoJson, PubSubClient, DallasTemperature, OneWire, TM1637, Seeed_Arduino_mbedtls, Led4digit74HC595

## Getting Started

To get started with PlatformIO in VSCode, you'll need to create a new project or open an existing one. Once you have a project open, you can use the PlatformIO commands within the `Project Tasks`. The basic steps are:
1. **Initialize** a new project with `Espressif ESP8266 ESP-12E` board and `Arduino` framework. Or simply just **open** this `ESP8266` folder as the project.
2. **Build** the project.
3. **Upload** the project to your NodeMCU.

## Creating a Project
To create a new project in VSCode, you can go to the PlatformIO Home page (which is automatically shown at startup) and choose `New project` and select the appropriate board. This will create a new directory with the default `src` directory and `platformio.ini` file. You can now start adding your own source code files to the `src` directory and configure your project using the `platformio.ini` file.

## Building and Uploading
Once you've created your project and added your source code, you can build it using the `platformio run` command in the VSCode terminal. This will compile your code and generate a binary file that can be uploaded to your device.  
```
platformio run
```
To upload the binary file to your device, you can connect the NodeMCU to your machine (Serial ports) with a microUSB cable and use the `platformio run --target` upload command in the VSCode terminal. This will transfer the binary file to your device and start running your code.  
```
platformio run --target upload
```

## Additional Resources

For more information on using PlatformIO in Visual Studio Code, please see the [official documentation](https://docs.platformio.org/en/latest/ide/vscode.html). This includes information on using PlatformIO with different boards, libraries, and frameworks.
