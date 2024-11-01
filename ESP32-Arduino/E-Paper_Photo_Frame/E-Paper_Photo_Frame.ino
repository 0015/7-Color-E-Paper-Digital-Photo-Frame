/*
  ESP32 E-Paper Digital Frame Project
  https://github.com/0015/7-Color-E-Paper-Digital-Photo-Frame
  
  5.65" Seven-Color eInk
  https://www.seeedstudio.com/5-65-Seven-Color-ePaper-Display-with-600x480-Pixels-p-5786.html

  XIAO eInk Expansion Board
  https://wiki.seeedstudio.com/XIAO-eInk-Expansion-Board/

  XIAO ESP32-S3
  https://www.seeedstudio.com/XIAO-ESP32S3-p-5627.html

  ESP32 Arduino Core
  Version: 3.0.7
*/

// Image byte data is stored in PSRAM, so need to enable PSRAM.

#include "EPD_7_Colors.h"
#include <WiFi.h>
#include <HTTPClient.h>
#define S_To_uS_Factor 1000000ULL      //Conversion factor for micro seconds to seconds 

const char* ssid = "your_wifi_ssid";        // Your WiFi SSID
const char* password = "your_wifi_password";  // Your WiFi Password
const char* getImageUrl = "http://your-server-ip:9999/get-img-data";
const char* wakeupIntervalUrl = "http://your-server-ip:9999/wakeup-interval";

void setup() {
  Serial.begin(115200);

  pinMode(BUSY_Pin, INPUT);
  pinMode(RES_Pin, OUTPUT);
  pinMode(DC_Pin, OUTPUT);
  pinMode(CS_Pin, OUTPUT);

  SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
  SPI.begin();

  connectWiFi();
  mainTask();
}

void loop() {
  delay(60000);
}

void connectWiFi() {
  int retryCount = 0;
  WiFi.mode(WIFI_STA);   // Set WiFi to station mode
  WiFi.begin(ssid, password);

  // Attempt to connect for a set number of retries
  while (WiFi.status() != WL_CONNECTED && retryCount < 10) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    retryCount++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to WiFi!");
  } else {
    Serial.println("Failed to connect to WiFi after 10 attempts, stopping WiFi.");
    hibernate(3600);
  }
}

void mainTask() {
 if (WiFi.status() == WL_CONNECTED) {
    // Start downloading the image data
    downloadImageData();
    // Fetch the wakeup interval and go into deep sleep
    hibernate(fetchWakeupInterval());
  } else {
    hibernate(3600);  // Go to sleep for 1 hour if no WiFi connection
  }
}

void downloadImageData() {
  HTTPClient http;

  if (!http.begin(getImageUrl)) {
    Serial.println("Failed to initialize HTTP connection");
    return;
  }

  // Make the request
  int httpResponseCode = http.GET();

  if (httpResponseCode > 0 && httpResponseCode == HTTP_CODE_OK) {
    String payload = http.getString();

    // Count the number of elements based on commas in the payload
    int elementCount = 1;
    for (int i = 0; i < payload.length(); i++) {
      if (payload[i] == ',') elementCount++;
    }

    // Allocate memory for the uint8_t array
    uint8_t* dataBuffer = (uint8_t*)ps_malloc(elementCount);
    if (dataBuffer == nullptr) {
      Serial.println("Failed to allocate memory in PSRAM");
      return;
    }

    // Parse the payload string and fill the array
    int index = 0;
    int startPos = 0;

    for (int i = 0; i < payload.length(); i++) {
      if (payload[i] == ',' || i == payload.length() - 1) {
        // Get the substring for the current hex value
        String hexValue = payload.substring(startPos, i);
        hexValue.trim();  // Trim whitespace around the hex string

        // Convert hex string to uint8_t and store it in the array
        dataBuffer[index++] = (uint8_t)strtol(hexValue.c_str(), nullptr, 16);
        // Update start position for the next hex value
        startPos = i + 1;
      }
    }

    EPD_init();
    PIC_display((const uint8_t*)dataBuffer);
    EPD_sleep();

    free(dataBuffer);  // Free the allocated memory if no longer needed
    dataBuffer = nullptr;

  } else {
    Serial.printf("HTTP GET request failed, error: %s\n", http.errorToString(httpResponseCode).c_str());
  }

  http.end();  // Close connection
}

int fetchWakeupInterval() {
  HTTPClient http;

  if (!http.begin(wakeupIntervalUrl)) {
    Serial.println("Failed to initialize HTTP connection");
    return 3600;  // Default to 1 hour if there’s an error
  }

  int httpResponseCode = http.GET();
  if (httpResponseCode > 0) {
    String payload = http.getString();
    int interval = payload.substring(payload.indexOf(':') + 1, payload.indexOf('}')).toInt();  // Extract the integer value
    http.end();
    return interval;
  } else {
    Serial.printf("Error fetching interval: %s\n", http.errorToString(httpResponseCode).c_str());
    http.end();
    return 3600;  // Default to 1 hour if there’s an error
  }
}

void hibernate(int interval) {
  WiFi.disconnect(true);   // Disconnect from any connection attempt
  WiFi.mode(WIFI_OFF);     // Turn off the WiFi
  delay(1000);
  esp_sleep_enable_timer_wakeup(interval * S_To_uS_Factor);  // Convert to microseconds
  esp_deep_sleep_start();
}