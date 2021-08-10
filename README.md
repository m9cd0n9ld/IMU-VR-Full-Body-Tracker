# IMU-VR-Full-Body-Tracker
 Inside-out full body tracker for Steam VR based on ESP32 with BNO080/085 IMU.  
 Support up to 2 BNO08X IMUs per ESP32 via I2C communication.  
 No base station is required, completely wireless using WiFi UDP.  
 Should be compatible with any VR headset that supports Steam VR.  
 <p align="middle">
 <img src="media\preview_feet.gif" width="40%">
 <img src="media\preview_shoulder.gif" width="40%">
 </p>
 
## Hardware Example
 **LILYGO® TTGO T7 V1.5 Mini32 ESP32-WROVER-B** with **GY-BNO08X**  
 <p align="middle">
 <img src="media/ttgo-t7-v1.5_gy-bno08x.jpg" width="30%">
 <img src="media/gy-bno08x_extension.jpg" width="30%">
 <p/>
 <p align="middle">
 <img src="media/tracker.jpg" width="30%">
 <img src="media/extension.jpg" width="30%">
 <img src="media/tracker_extension.jpg" width="30%">
 <p/>
 <p/>
 <p align="middle">
 <img src="media/ttgo-t7-v1.5_gy-bno08x_connection.png" width="50%">
 <p/>

## Instruction
 1. Extract steamvr.driver.zip and copy imuFBT folder to (Steam directory)\steamapps\common\SteamVR\drivers.  
 2. Extract esp32.sketch.zip, edit the settings (pin settings, etc.) in esp32_settings.h file.  
 3. Upload the sketch to ESP32 microcontroller with Arduino IDE (SparkFun_BNO080_Arduino_Library Arduino library is required).  
 4. Extract desktop.server.zip and run imufbtserver.exe (might trigger antivirus false positive, known problem for unsigned PyInstaller exe files).  
 5. Set the tracker sensor frame to driver frame (X right, Y up, Z back) XYZ intrinsic rotation based on the mounting orientation, body segment length, tracker position and offset settings when standing in T-pose** in **Settings** and **Offset settings** tab.  
 6. Activate required tracker devices to be output to SteamVR. In some SteamVR game/program, the hand controllers might lose tracking or not working if too many tracker devices are activated.  
 7. Set the tracker floor level offset (tracker min Y value) if necessary.  
 8. Enable override feet tracker data with lower leg tracker data if want to use lower leg tracker as foot role. Feet tracker device must still be activated.  
 <p align="middle">
 <img src="media/desktop_app_settings_1.png" width="40%">
 <img src="media/desktop_app_settings_2.png" width="40%">
 <img src="media/desktop_app_settings_3.png" width="40%">
 <img src="media/desktop_app_offset_settings.png" width="40%">
 <p/>
 
 9. Plug in USB cable to ESP32 and set the WiFi credential in **WiFi Settings** tab.  
 <p align="middle">
 <img src="media/desktop_app_wifi_settings.png" width="40%">
 <p/>
 
 10. Once WiFi credential is configured, unplug the USB cable and power cycle the ESP32 to establish WiFi connection.  
 11. The trackers will be listed in both **Devices** and **Role assignment** tabs.  
 12. Set the roles of the trackers in **Role assignment** tab.  
 <p align="middle">
 <img src="media/desktop_app_choose_role.png" width="40%">
 <p/>
 
 13. Once everything is setup properly, the trackers with correct roles assignment will be shown in **Devices** tab.  
 <p align="middle">
 <img src="media/desktop_app_devices_list.png" width="40%">
 <p/>
 
 14. Mount the trackers according to the following diagram. For feet/leg tracking, chest tracker, waist tracker, thigh trackers and lower leg trackers are required. Feet trackers are optional. For elbow tracking, chest tracker, shoulder trackers and upper arm trackers are required.  
 <p align="middle">
 <img src="media/T-pose_skeleton_diagram.png" width="60%">
 <img src="media/tracker_kinematic_chain.png" width="60%">
 <p/>
 
 15. Open SteamVR.  
 16. The trackers will be shown in SteamVR environment at weird position and orientation on startup.  
 17. Stand in T-pose**, then press **Calibrate** button in **Devices** tab to realign/calibrate the trackers.  
 <p align="middle">
 <img src="media/steamvr_sensors_alignment.gif" width="60%">
 <p/>
 
 18. Done!  
 <p align="middle">
 <img src="media/final_result.gif" width="60%">
 <p/>
 
 19. When desktop app is closed, the settings will be saved to config file stored in **My Documents** directory. The config file will be automatically loaded on next instance.  
 
 **Stand straight look forward with feet pointing forward and hip-width apart in T-pose with palm facing down.
