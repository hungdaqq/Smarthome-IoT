# Introduction to Thingsboard Platform 

ThingsBoard is an open-source IoT platform for data collection, processing, visualization, and device management. It provides an easy-to-use web-based interface for creating dashboards, managing devices, and storing and analyzing data from connected devices. 
ThingsBoard Live Demo Server is a pre-configured instance of the ThingsBoard IoT platform, designed to give users a quick and easy way to experience the features and functionality of ThingsBoard. The live demo server includes pre-configured devices, dashboards, and visualizations that you can use to start exploring the platform right away.

## Accessing the Live Demo Server
To access the ThingsBoard Live Demo Server, follow these steps:
1. Open a web browser and navigate to https://demo.thingsboard.io/.
2. Enter the your login credentials or use the default one: Username: `tenant@thingsboard.io`, password: `tenant`.

## Exploring the Live Demo Server

Once you have logged in to the live demo server, you can start exploring the platform. Here are a few things you can do:
- Navigate to the "Devices" tab to see the pre-configured devices and their telemetry data.
- Navigate to the "Dashboards" tab to view the pre-created dashboards and visualizations.
- Play around with the visualizations and customize them to suit your needs.

## Device Management
One of the key features of ThingsBoard is the ability to manage and monitor IoT devices. To start using this feature, follow these steps:
1. Navigate to the "Devices" tab.
2. Click on the "Add device" button.
3. Enter the required information, including the device name and type.
4. Click the "Save" button to create the device.

## Data Collection and Visualization
Once you have added devices, you can start collecting data from them and visualizing it in ThingsBoard. To do this, follow these steps:

1. Navigate to the "Devices" tab.
2. Click on the device you want to collect data from.
3. Click on the "Telemetry" tab.
4. Enter the telemetry data in the appropriate fields.
5. Click the "Save" button to store the data.
6. Navigate to the "Dashboards" tab.
7. Create a new dashboard and add visualizations to it, such as charts and gauges.
8. Select the device and telemetry data you want to visualize.

## Using the Rule Engine
To create rules in ThingsBoard, follow these steps:
1. Navigate to the "Rule Chains" tab.
2. Click the "Create Rule Chain" button.
3. Enter a name and description for the rule chain.
4. Click the "Save" button.
5. Click the "Add Rule Node" button to create a new rule.
7. Select the type of rule you want to create, such as "Device Attributes" or "Time Interval".
8. Enter the required information for the rule.
9. Click the "Save" button to create the rule.  

Once you have created rules, you can start using the rule engine to process telemetry data from your devices. To do this, follow these steps:
1. Navigate to the "Devices" tab.
2. Click on the device you want to apply the rule to.
3. Click on the "Telemetry" tab.
4. Enter the telemetry data in the appropriate fields.
5. Click the "Save" button to store the data.
6. Navigate to the "Rule Chains" tab.
7. Click on the rule chain you created earlier.
8. Observe the rule engine processing the telemetry data and triggering the actions defined in the rules.

## Conclusion

The ThingsBoard Live Demo Server is a great way to get started with the ThingsBoard platform and explore its features and functionality. In this Smart Home IoT project, we have already created the `.json` and `.csv` template files for Rule engine, Dashboard, Profiles, Devices and Assets. To learn more about ThingsBoard and how to use it, please refer to the [official documentation](https://docs.thingsboard.io/).
