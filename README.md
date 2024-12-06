# ESP32 E-Paper Digital Frame Project
This project is a digital photo frame using an ESP32 microcontroller and a 5.65” seven-color E-Paper display, optimized to display images with a 7-color palette. The ESP32 retrieves image data from a local server and displays it on the screen, with a power-saving feature that puts the ESP32 into hibernation between updates.

![](https://raw.githubusercontent.com/0015/7-Color-E-Paper-Digital-Photo-Frame/refs/heads/main/misc/Demo.gif)(https://youtu.be/9gdemeaTfyI)

## Project Overview
This setup is designed to efficiently display photos on an E-Paper display, which supports only a limited color palette. To achieve the best image quality, images are converted using Floyd–Steinberg dithering and a custom 7-color palette.

## Hardware
[Note] I have only tested it with this display, so it may not work with other E-Paper 7-color displays.
* [5.65” Seven-Color E-Paper Display 600x448](https://www.seeedstudio.com/5-65-Seven-Color-ePaper-Display-with-600x480-Pixels-p-5786.html)
* [E-Paper Breakout Board for Seeed Studio XIAO](https://www.seeedstudio.com/ePaper-Breakout-Board-p-5804.html)
* [Seeed Studio XIAO ESP32S3](https://www.seeedstudio.com/XIAO-ESP32S3-p-5627.html)
* 1S LiPo Battery 3.7v 
* Raspberry Pi 4 (for local server)

![Hardware](https://raw.githubusercontent.com/0015/7-Color-E-Paper-Digital-Photo-Frame/refs/heads/main/misc/Hardware.png)

## System Architecture
1.  Raspberry Pi with Flask Server:
* Hosts a simple API server that serves image data to the ESP32.
* Monitors a designated folder for new images and automatically processes them to the required format.
2.  ESP32 with E-Paper Display:
* Requests image data from the server.
* Draws the image on the E-Paper display.
* Retrieves the next wake-up interval from the server to manage its power-saving hibernation mode.

## Workflow
1.  Image Conversion: New images added to the Images folder are automatically resized to 600x448 and processed using Floyd–Steinberg dithering to match the 7-color palette.
2.  Flask API: The server sends converted images in byte data format ready for display. The ESP32 requests new images and draws them on the display.
3.  Hibernation Schedule: Between 8 AM and 8 PM, the ESP32 wakes up every hour for updates. After 8 PM, it hibernates until 8 AM the following day.

![Workflow](https://raw.githubusercontent.com/0015/7-Color-E-Paper-Digital-Photo-Frame/refs/heads/main/misc/Workflow.png)


## ESP32 Setup
* _Test_E_Paper_5_65inch_7color: A test program for the E-Paper display.
* E-Paper_Photo_Frame: The main program to connect to the server and update the display dynamically.
    * This code stores the image data obtained from the server into PSRAM. Please change this part according to your needs.

## Raspberry Pi Server Setup
[Note] The reason I used RPI here is that this is a personal local test server that is always on. You can use it on your PC as well, not just RPI.
1. Clone this repository to your Raspberry Pi.
```
git clone https://github.com/0015/7-Color-E-Paper-Digital-Photo-Frame.git
```
2.  Install the dependencies:
```python
pip install -r requirements.txt
```
3.  Set up and start the Flask server:
```python
python flask_server.py
```
The monitor.py script will automatically run alongside. It will watch the Images folder, resize images, apply dithering, and save the result as .h files in the h_files folder.

## Flask API Endpoints
* /get-img-data: Returns the next image data in byte format.
* /status: Returns the status of sent and remaining files.
* /wakeup-interval: Returns the interval (in seconds) until the next wake-up time based on the current time.

## Dependencies
* Flask: For running the API server.
* Watchdog: For monitoring file changes.
* Pillow: For image processing.
* NumPy: For handling numerical operations during image conversion.
