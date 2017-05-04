/*
 * 
 * Author: Sushant Mainaly
 * Company: Connyct
 * Website: https://www.connyct.com
 * 
 * License: Beerware - If you use it and you meet me someday buy me beer!
 * If I have used external libraries, the licence above does not apply to them.
 * 
 * Date: May 1st 2017
 */
 
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h>
#include <Hash.h>

/*
 * THE FOLLOWING CONSTANTS WILL HAVE TO BE REPLACED IN ORDER TO USE THIS CODE
 */
#define SSID_NAME           "NAME HERE" // Wifi Name
#define SSID_PASSWORD       "Password Here" // Wifi Password
/* Token and secret key from https://www.connyct.com */
#define CONNYCT_TOKEN       "xxxxxxxxxxxxxx"
#define CONNYCT_SECRET_KEY  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
 /*
  * END OF CONSTANTS TO CHANGE
  */

#define CONNYCT_URL       "connyct.com"
#define WS_PORT           80
#define CONNYCT_WS_LOC    "/api/v1/w/"

#define USE_SERIAL Serial
#define INCOMING_BUFFER_LENGTH 1024

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;
boolean connected = false;


int incomingDataIndex = 0;
String incomingData = "";
boolean completeDataReceived = false;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            connected = false;
            USE_SERIAL.printf("[WSc] Disconnected: %s\n", payload);
            break;
        case WStype_CONNECTED:
            {
                USE_SERIAL.printf("[WSc] Connected to url: %s\n",  payload);
                // send credentials as first message when connected
                webSocket.sendTXT("{\"token\":\""+String(CONNYCT_TOKEN)+"\",\"secret_key\":\""+String(CONNYCT_SECRET_KEY)+"\"}");
            }
            connected = true;
            break;
        case WStype_TEXT:
            USE_SERIAL.printf("[WSc] received: %s\n", payload);
            break;
        case WStype_BIN:
            USE_SERIAL.printf("[WSc] received binary length: %u\n", length);
            hexdump(payload, length);
            break;
    }
}

void setup() {
    USE_SERIAL.begin(115200);
    USE_SERIAL.setDebugOutput(true);
    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    incomingData.reserve(INCOMING_BUFFER_LENGTH);
    
      for(uint8_t t = 4; t > 0; t--) {
          USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
          USE_SERIAL.flush();
          delay(1000);
      }

    // WiFi Name and Password
    WiFiMulti.addAP(SSID_NAME, SSID_PASSWORD);
    
    while(WiFiMulti.run() != WL_CONNECTED) {
        delay(100);
    }

    //connyctserver
    webSocket.begin(CONNYCT_URL,WS_PORT,CONNYCT_WS_LOC);
    webSocket.onEvent(webSocketEvent);
}


void parseAndSendData()
{
  // convert string in buffer to json and send it
  // find index of = 
  int i = 0;
  String dataToSend = "{\n\"";
  int equalsIndex = 0;
  int endIndex = 0;

  for (i=0;i<INCOMING_BUFFER_LENGTH;i++)
  {
    if( incomingData[i] == ';')
    {
      dataToSend += "}";
      break;
    }

    if( incomingData[i] == '\n')
    {
      if (incomingData[i+1] != ';')
      {
        dataToSend+= "\",\n\"";
      }
      else
      {
        dataToSend+= "\"\n";
      }
    }
    else if ( incomingData[i] == '=')
    {
      dataToSend+= "\":\"";
    }
    else
    {
      dataToSend+=incomingData[i];
    }
  }
  Serial.print(dataToSend);
   webSocket.sendTXT(dataToSend);
}


void loop() {
    webSocket.loop();

      while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    incomingData +=inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == ';') 
    {
      completeDataReceived = true;
    }
  }

  
    if (completeDataReceived && connected)
    {
      // need to process data that we received which is in incomingData
      parseAndSendData();
      incomingData="";
      completeDataReceived = false;
    }     
}


