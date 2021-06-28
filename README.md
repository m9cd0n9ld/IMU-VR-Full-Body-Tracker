# IMU-VR-Full-Body-Tracker
 Inside-out full body tracker for Steam VR based on ESP32 with BNO080/085 IMU.  
 No base station is required, completely wireless using WiFi.  
 Compatible with any VR headset that supports Steam VR.  
 Requires minimum 1 set of ESP32 + BNO080/085 to track waist, hand (controllers) and head (headset) for 4 point body tracking in Steam VR.  
 Requires minimum 5 sets of ESP32 + BNO080/085 to track waist, thigh, leg, hand (controllers) and head (headset) for 6 point body tracking in Steam VR.  
 Optional extra 1 set of ESP32 + BNO080/085 to track chest for improved skeleton kinematic model accuracy.  
 
## Hardware Example
 **LILYGO® TTGO T7 V1.5 Mini32 ESP32-WROVER-B** with **GY-BNO08X**  
 ![TTGO T7 V1.5 with GY-BNO08X](<img src="./media/ttgo-t7-v1.5_gy-bno08x.jpg" width="375" height="500">)  
 ![Tracker](<img src="./media/tracker.jpg" width="375" height="500">)  
 
## Installation
 Extract steamvr.driver.zip and copy imuFBT folder to (Steam directory)\steamapps\common\SteamVR\drivers  
 
## Usage
 1. Extract esp32.sketch.zip, edit the settings (pin settings, etc.) in esp32_settings.h file.  
 2. Upload the sketch to ESP32 microcontroller with Arduino IDE (SparkFun_BNO080_Arduino_Library Arduino library is required).  
 3. Extract desktop.server.zip and run imufbtserver.exe (might trigger antivirus false positive, typical problem for unsigned exe files).  
 4. Set the sensor frame to driver frame (X right, Y up, Z back) XYZ intrinsic rotation based on the mounting orientation, and body parts measurement and sensor position settings in **Settings** tab.  
 5. Enable chest sensor if applicable.  
 ![Settings](<img src="./media/desktop_app_settings.png" width="246" height="300">)  
 6. Plug in USB cable to ESP32 and set the WiFi credential in **WiFi Settings** tab.  
 ![WiFi settings](<img src="./media/desktop_app_wifi_settings.png " width="246" height="300">)  
 7. Once WiFi credential is configured, unplug the USB cable and power cycle the ESP32 to establish WiFi connection.  
 8. The trackers will be listed in both **Devices** and **Role assignment** tabs.  
 9. Set the roles of the trackers in **Role assignment** tab.  
 ![Roles assignment](<img src="./media/desktop_app_choose_role.png" width="246" height="300">)  
 10. Once everything is setup properly, the trackers with correct roles assignment will be shown in **Devices** tab.  
 ![Devices](<img src="./media/desktop_app_devices_list.png" width="246" height="300">)  
 11. Open SteamVR.  
 12. The trackers will be shown in SteamVR environment at weird position and orientation on startup.  
 ![Before alignment](<img src="./media/steamvr_sensors_not_aligned.png" width="500" height="500">)  
 13. Stand straight with feet hip-width apart and look forward, then reset SteamVR view to realign/calibrate the trackers, or press **Calibrate** button in **Devices** tab to realign/calibrate the trackers without resetting SteamVR view.  
 ![Alignment](<img src="./media/steamvr_sensors_alignment.gif" width="500" height="500">)  
 14. Done!  
 ![Final](<img src="./media/final_result.gif" width="500" height="280">)  
 
 
 