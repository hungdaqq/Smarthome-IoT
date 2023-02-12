# Kitchen controlling 
In the kitchen, we will have a USB-powered MCU to control LED light and Buzzer and a battery-powered MCU to collect sensor datas.
## List of hardware
- 02 NodeMCU ESP8266 
- 01 LED
- 01 Flame sensor
- 01 Buzzer (3V or 5V)
- 01 Temperature Sensor DS18B20
- 02 Push buttons (4 pin)
- microUSB cable
- Breadboard
- Female-to-female jumper wires
- Female-to-male jumper wires
- 3.3V power source (for example lithium battery)

## Installation steps
1. In Thingsboard Live Demo Server, navigate to the "Devices" tab and click on the "Add device" button. Enter the required information, including the device name, type and credential. Click the "Save" button to create the device.
2. Replace `ESP8266/src` with each file in `Kitchen/src`, then upload the code to the corresponding MCU.
3. Connect the parts in line with the following hardware schemas:  
For light controlling device:   
![Schema1](https://github.com/hungdaqq/Smarthome-IoT/blob/main/Features/Kitchen/schema1.png)  
For temperature sensor device:  
![Schema2](https://github.com/hungdaqq/Smarthome-IoT/blob/main/Features/Kitchen/battery.png)  
4. Navigate to the "Dashboard" tab and inport the `KitchenDashboard.json` template.
5. Create `Device Allias` as `Single Entity` and choose the Devices which have been created at Step 1.
6. Edit the datasource of each widgets/charts/tables.

## Features
- Temperature monitoring.
- Control LED light with sensor, button, and RPC command.
- Detect fire and turn on warning buzzer.
