# Arduino-Cam

![Website Interface](https://github.com/axel0070/Arduino-Cam/blob/master/Screenshots/IFD2.png)

Using an [ArduCAM](https://www.gotronic.fr/art-camera-arducam-ov5642-25252.htm) we built a simple web interface powered by NodeJS which offer to the user to controls the ArduCAM, you can launch a videostream, make a capture, detect [movement](https://boutique.semageek.com/fr/1249-detecteur-de-mouvement-pir-avec-reglages.html?search_query=PIR&results=21)...

The box where the ArduCAM is contain was printed using a 3D printer, the design has been done on Fusion 360.

![ArduCAM box](https://github.com/axel0070/Arduino-Cam/blob/master/Screenshots/Tower.png)

## Requirements

* Node.js v12.8.1
* Git
* Visual Studio Code (PlatformIO installed)
* ESP32 connected to OV5642

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

Import the ESP32 folder as a project into Visual Studio Code

Open `Arduino-Cam\ESP32\src\main.cpp` and write your local ip, the SSID and the password of the Wifi network where your device is connected.

## Informations

Sometimes we can have a stream of 1 fps, and sometimes we need 20-40 seconds for 1 frame, and most of the time it never sends the frame.

I advise to reboot (using button) until it works for a proper demo.
