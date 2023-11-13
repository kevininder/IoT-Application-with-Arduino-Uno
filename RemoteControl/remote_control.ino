/* Control light switches via Wi-Fi with Arduino Ethernet Shield. */

#include "SPI.h"
#include "Ethernet.h"
#include "WebServer.h"
#include "Streaming.h"

const byte NUM_OF_SW = 3;
const byte swPins[] = {7, 8, 9};
const char *swNames[] = {"LED RED", "LED YELLOW", "LED GREEN"};

boolean ledState = LOW;
unsigned long startTime;
static byte mac[] = {0xF0, 0x7B, 0xCB, 0x4B, 0x7C, 0x9F};
IPAddress ip(192, 168, 50, 177);
IPAddress subnet(255, 255, 255, 0);
IPAddress gateway(192, 168, 50, 1);

WebServer webserver("", 80);

P(htmlHead) = "<!doctype html><html>"
"<html lang='en'>"
"<meta charset='UTF-8'>"
"<meta name='viewport' content='width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=0'>"
"<title>kk in the fab</title>"
"<head>";

P(myCSS) = "<style>"
"body {background-color: rgb(185, 185, 185); font-family: Arial}"
"a {margin: 15px 10px; text-decoration: none}"
".panel {display: flex; flex-direction: column; background-color: beige; width: 190px; border-radius: 5px; padding: 5px 5px}"
".slot {display: flex; align-items: center}"
".sw {position: relative; display: inline-block; width: 60px; height: 34px}"
".sw input {opacity: 0}"
".slider-before {position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: rgb(218, 218, 218); border-radius: 34px}"
".circle-before {position: absolute; content: ""; height: 26px; width: 26px; left: 4px; bottom: 4px; background-color: white; border-radius: 50%}"
".slider-after {position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: rgb(0, 211, 0); border-radius: 34px}"
".circle-after {position: absolute; content: ""; height: 26px; width: 26px; left: 30px; bottom: 4px; background-color: white; border-radius: 50%}"
"</style>";

P(htmlFoot) = "</body></html>";

void showPage(WebServer &server) {
  server.printP(htmlHead);
  server.printP(myCSS);
  server.println("</head><body><h2>Remote Control</h2><div class='panel'>");

  for (byte i = 0; i < NUM_OF_SW; i++) {
    server.println("<div class='slot'><label class='sw'><input type='checkbox'>");
    server << "<a href='sw?id=" << i << "'>";
    if (digitalRead(swPins[i])) {
      server.print("<span class='slider-after'><span class='circle-after'></span></span></label>");
    } else {
      server.print("<span class='slider-before'><span class='circle-before'></span></span></label>");
    }
    server << swNames[i] << "</a></div>";
  }

  server.println("</div>");
  server.printP(htmlFoot);
}

void defaultCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
  server.httpSuccess();

  if (type != WebServer::HEAD) {
    showPage(server);
  }
}

void getCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  URLPARAM_RESULT rc;
  char name[3], value[3];
  byte rawNum, pinNum;

  server.httpSuccess();

  if (type == WebServer::GET) {
    while (strlen(url_tail)) {
      rc = server.nextURLparam(&url_tail, name, 3, value, 3);

      if (!strcmp(name,"id")) {
        rawNum = atoi(value);
        pinNum = swPins[rawNum];
        Serial.print("id num: ");
        Serial.println(rawNum);
        Serial.print("pin num: ");
        Serial.println(pinNum);
        digitalWrite(pinNum, !digitalRead(pinNum)); 
      }
    }
    
   showPage(server);
  }
}

void setup() {
  Serial.begin(9600);

  for (byte i = 0; i < NUM_OF_SW; i++) {
    pinMode(swPins[i], OUTPUT);
  }
  
  Ethernet.begin(mac, ip, gateway, subnet);
  webserver.setDefaultCommand(&defaultCmd);
  webserver.addCommand("sw", &getCmd);
  webserver.begin();
}

void loop() {
  webserver.processConnection();
}