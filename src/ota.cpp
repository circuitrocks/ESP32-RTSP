#include "main.h"

/** Flag if OTA is enabled */
boolean otaStarted = false;

/** Limit the progress output on serial */
unsigned int lastProgress = 0;

/**
 * Initialize OTA server
 * and start waiting for OTA requests
 */
void startOTA(void)
{
	ArduinoOTA
		// OTA request received
		.onStart([]() {
			String type;
			if (ArduinoOTA.getCommand() == U_FLASH)
				type = "sketch";
			else // U_SPIFFS
				type = "filesystem";

			// NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
			Serial.println("Start updating " + type);
			lastProgress = 0;
			otaStarted = true;
		})
		.onEnd([]() {
			// OTA is finished
			Serial.println("\nEnd");
		})
		.onProgress([](unsigned int progress, unsigned int total) {
			// Status report during OTA
			if ((lastProgress == 0) || ((progress / (total / 100)) >= lastProgress + 5))
			{
				Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
				lastProgress = (progress / (total / 100));
				if (lastProgress == 0)
				{
					lastProgress = 1;
				}
			}
		})
		.onError([](ota_error_t error) {
			// Error occured during OTA, report it
			Serial.printf("Error[%u]: ", error);
			if (error == OTA_AUTH_ERROR)
				Serial.println("Auth Failed");
			else if (error == OTA_BEGIN_ERROR)
				Serial.println("Begin Failed");
			else if (error == OTA_CONNECT_ERROR)
				Serial.println("Connect Failed");
			else if (error == OTA_RECEIVE_ERROR)
				Serial.println("Receive Failed");
			else if (error == OTA_END_ERROR)
				Serial.println("End Failed");
		});

	// Enable MDNS so device can be seen
	ArduinoOTA.setMdnsEnabled(true);

	// Create a unique name
	// IPAddress ip = WiFi.localIP();
	// String hostName = "ESP32-CAM" + ip.toString();
	char hostName[] = "ESP32-CAM";
	Serial.printf("Device is advertising as ESP32-CAM\n");
	// Set the MDNS advertising name
	ArduinoOTA.setHostname(hostName);
	// Start the OTA server
	ArduinoOTA.begin();
}

/**
 * Stop the OTA server
 */
void stopOTA(void)
{
	ArduinoOTA.end();
}