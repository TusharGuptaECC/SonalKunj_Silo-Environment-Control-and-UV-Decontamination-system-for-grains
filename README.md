# SonalKunj_Silo (Environment Control and UV Decontamination system for grains ECUDSG)

SonalKunj_Silo is an Environment Control and UV Decontamination system designed for storing food grains in warehouses. The system leverages embedded systems and IoT technology to monitor and regulate the warehouse environment, ensuring optimal conditions for grain storage. It includes features such as temperature and humidity control, gas concentration monitoring, UV decontamination, and security measures.

- **Features**
  - Temperature monitoring and control
  - Humidity monitoring and control
  - Gas concentration monitoring and alert system
  - UV decontamination of grains
  - Security and human disaster management system for fumigation problems
- **Purpose**
The SonalKunj_Silo system addresses the challenges faced in traditional food grain storage facilities in India, where improper infrastructure and hygiene lead to significant grain wastage. By implementing the SonalKunj_Silo system, the lifetime of food grains can be improved by regulating the storage environment. This helps prevent pest infestation, maintain appropriate temperature and humidity levels, and ensure proper fumigation.

- **Getting Started**
To get started with the SonalKunj_Silo system, follow these steps:

  - Install the necessary hardware components, including temperature and humidity sensors, gas concentration sensors, UV lights, and the main control PCB.
  - Configure the system parameters such as temperature and humidity thresholds according to the specific grain storage requirements.
  - Connect the system to the internet to enable data transmission to Google Sheets for further analytics and monitoring.
  - Ensure proper authentication and access control to restrict system access to authorized personnel only.
  - Set up the UV decontamination intervals and ensure worker safety by measuring the concentration of PH3 gas after fumigation.

Please refer to the GitHub Wiki for detailed documentation, tutorials, and additional information about the SonalKunj_Silo system.

- **Current Limitations**
The project is currently in development, and the following aspects require further attention:

  - Switching between automatic and manual modes may exhibit some unexpected behavior.
  - The project currently provides data logging to Google Sheets but does not include a web dashboard for remote control.
  - Additional equipment, such as relays, may be required to control temperature and humidity regulation units (TCU and HCU).
  - Fumigation protection has not been implemented in the initial development phase due to the unavailability of required PH3 sensors.

 - **Development Cycle Schedule**
   - Phase I: 01/02/2023 to 31/03/2023
   - Phase II: Coming soon
For more details, refer to the [GitHub Wiki](https://github.com/TusharGuptaECC/SonalKunj_Silo-Environment-Control-and-UV-Decontamination-system-for-grains/wiki) and the project's documentation.
