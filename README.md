# ESP32-CAM RTSP stream

This repository is for the [Circuitrocks@Learn](https://learn.circuit.rocks/?p=2245) blog how to stream a video from the ESP32-CAM to a RTSP client or web browser.

The source code for both ArduinoIDE and PlatformIO is in the _**src**_ subfolder.

## Compiling with PlatformIO
Just open the folder containing the files with PlatformIO and adjust your settings in platformio.ini. Required libraries are either automatically installed (OneButton) or in the lib folder (Micro-RTSP).

## Compiling with ArduinoIDE
Install the required libraries as explained in the [blog](https://learn.circuit.rocks/?p=2245).
Open the file **`src.ino`** in the _**src**_ folder. Don't be surprised that the file is empty. It is only there for compatibility with ArduinoIDE. 

