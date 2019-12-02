# Arduino-Cam

## Requirements

* Node.js v12.8.1
* Git
* Visual Studio Code (PlatformIO installed)

## Node.js setup

Clone the repository and install the dependencies.

```bash
git clone https://github.com/axel0070/Arduino-Cam.git
cd Arduino-Cam
npm install
```
Start the application
```bash
npm start
```
Open [http://localhost:3000](http://localhost:3000).
## ESP32 setup

Import the the ESP32 folder as a project into Visual Studio Code

Open `Arduino-Cam\ESP32\src\main.cpp` and write your local ip, the SSID and the password of the Wifi network where your device is connected.

## Informations

Sometime we can have a stream of 1 fps, and sometime we need 20-40 secondes for 1 frame, and most of the time it never send the frame.

I advise to reboot (using button) until it work for a proper demo.