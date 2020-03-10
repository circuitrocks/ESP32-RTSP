#include "main.h"
#ifdef ENABLE_RTSPSERVER

// Use this URL to connect the RTSP stream, replace the IP address with the address of your device
// rtsp://192.168.0.109:8554/mjpeg/1

/** Forward dedclaration of the task handling RTSP */
void rtspTask(void *pvParameters);

/** Task handle of the RTSP task */
TaskHandle_t rtspTaskHandler;

/** WiFi server for RTSP */
WiFiServer rtspServer(8554);

/** Stream for the camera video */
CStreamer *streamer = NULL;
/** Session to handle the RTSP communication */
CRtspSession *session = NULL;
/** Client to handle the RTSP connection */
WiFiClient rtspClient;
/** Flag from main loop to stop the RTSP server */
boolean stopRTSPtask = false;

/**
 * Starts the task that handles RTSP streaming
 */
void initRTSP(void)
{
	// Create the task for the RTSP server
	xTaskCreate(rtspTask, "RTSP", 4096, NULL, 1, &rtspTaskHandler);

	// Check the results
	if (rtspTaskHandler == NULL)
	{
		Serial.println("Create RTSP task failed");
	}
	else
	{
		Serial.println("RTSP task up and running");
	}
}

/**
 * Called to stop the RTSP task, needed for OTA
 * to avoid OTA timeout error
 */
void stopRTSP(void)
{
	stopRTSPtask = true;
}

/**
 * The task that handles RTSP connections
 * Starts the RTSP server
 * Handles requests in an endless loop
 * until a stop request is received because OTA
 * starts
 */
void rtspTask(void *pvParameters)
{
	uint32_t msecPerFrame = 50;
	static uint32_t lastimage = millis();

	// rtspServer.setNoDelay(true);
	rtspServer.setTimeout(1);
	rtspServer.begin();

	while (1)
	{
		// If we have an active client connection, just service that until gone
		if (session)
		{
			session->handleRequests(0); // we don't use a timeout here,
			// instead we send only if we have new enough frames

			uint32_t now = millis();
			if (now > lastimage + msecPerFrame || now < lastimage)
			{ // handle clock rollover
				session->broadcastCurrentFrame(now);
				lastimage = now;
			}

			// Handle disconnection from RTSP client
			if (session->m_stopped)
			{
				Serial.println("RTSP client closed connection");
				delete session;
				delete streamer;
				session = NULL;
				streamer = NULL;
			}
		}
		else
		{
			rtspClient = rtspServer.accept();
			// Handle connection request from RTSP client
			if (rtspClient)
			{
				Serial.println("RTSP client started connection");
				streamer = new OV2640Streamer(&rtspClient, cam); // our streamer for UDP/TCP based RTP transport

				session = new CRtspSession(&rtspClient, streamer); // our threads RTSP session and state
				delay(100);
			}
		}
		if (stopRTSPtask)
		{
			// User requested RTSP server stop
			if (rtspClient)
			{
				Serial.println("Shut down RTSP server because OTA starts");
				delete session;
				delete streamer;
				session = NULL;
				streamer = NULL;
			}
			// Delete this task
			vTaskDelete(NULL);
		}
		delay(10);
	}
}
#endif
