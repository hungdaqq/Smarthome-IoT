# Livingroom controlling 
In the livingroom, we will have a USB-powered MCU to control LED light and Heater and a battery-powered MCU to collect sensor datas.
## List of hardware
- 02 NodeMCU ESP8266 
- 02 LEDs
- 01 IR Sensor C-51
- 01 Temperature Sensor DS18B20
- microUSB cable
- Breadboard
- Female-to-female jumper wires
- Female-to-male jumper wires
- 3.3V power source (for example lithium battery)

## Installation steps
1. In Thingsboard Live Demo Server, navigate to the "Devices" tab and click on the "Add device" button. Enter the required information, including the device name, type and credential. Click the "Save" button to create the device. If you have used `Bulk Provisioning` with `.csv` file, please skip this step.
2. Replace `ESP8266/src` with each file in `Livingroom/src`, then upload the code to the corresponding MCU.
3. Connect the parts in line with the following hardware schemas:
4. Navigate to the "Dashboard" tab and inport the `LivingroomDashboard.json` template.
5. Create `Device Allias` as `Single Entity` and choose the Devices which have been created at Step 1.
6. Edit the datasource of each widgets/charts/tables.

## Features
- Temperature monitoring.
- Control LED light with Infrared Sensor (IR sensor), button, and RPC command.
- Adjust Heater temperature.
