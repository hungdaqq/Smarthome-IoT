# Bedroom and Bathroom temperature monitoring
In the bedroom and bathroom, we will have two battery-powered MCU to collect temperature sensor data.

## Installation
1. In Thingsboard Live Demo Server, navigate to the "Devices" tab and click on the "Add device" button. Enter the required information, including the device name `Bathroom Temperature Sensor`, type `Temperature Sensor` and credential token `BathroomTemperatureSensor`. Click the "Save" button to create the device. Do the similar step with bedroom. If you have used `Bulk Provisioning` with `.csv` file, please skip this step.
2. Replace ESP8266/src with each file in Bedroom&Bathroom/src, then upload the code to the corresponding MCU.
3. Connect the parts in line with the following hardware schemas:
