# Livingroom controlling 

## Installation
1. In Thingsboard Live Demo Server, navigate to the "Devices" tab and click on the "Add device" button. Enter the required information, including the device name, type and credential. Click the "Save" button to create the device.
2. Replace `ESP8266/src` by each file in `Livingroom/src` and upload the code to each of your ESP.
3. Connect the parts in line with the following hardware schemas:
4. Navigate to the "Dashboard" tab and inport the `LivingroomDashboard.json` template.
5. Create `Device Allias` as `Single Entity` and choose the Devices whichs has been created at Step 1.
6. Edit the datasource of each widgets/charts/tables.

## Features
1. Temperature monitoring with sensor.
2. Controll light with movement sensor, button, and RPC command.
