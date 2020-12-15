#include <Arduino.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Ticker.h>
#include <AsyncMqttClient.h>
#include <ESP8266WebServer.h>
#include "DNSServer.h"
#include <PubSubClient.h>

// Setup Display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

Adafruit_SSD1306 display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

// Setup Wifi
#define WIFI_SSID "***"
#define WIFI_PASSWORD "***"

// Setup MQTT
#define MQTT_HOST "activemq.bytecode.ca"
#define MQTT_PORT 1883
#define MQTT_USER "***"
#define MQTT_PASS "***"
#define MQTT_SUB_TEMP "in/shams/temperature"
#define MQTT_SUB_HUM "in/shams/humidity"

// setup mqtt client
WiFiClient espClient;
PubSubClient client(espClient);

String temperature = "--.--";
String humidity = "--.--";

void displayTempHumid()
{

    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print("T ");
    display.print(temperature.substring(0, 5));
    display.print(" \370C");
    display.setCursor(0, 18);
    display.print("H ");
    display.print(humidity.substring(0, 5));
    display.print(" %\t");
    display.display();
}

void printDisplay(const String message)
{
    delay(1000);
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print(message);
    display.display();
}

void onWifiConnect(const WiFiEventStationModeGotIP &event)
{
    printDisplay("Connected to Wi-Fi");
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected &event)
{
    printDisplay("Disconnected from Wi-Fi");
}

void connectToWifi()
{

    delay(10);
    WiFi.mode(WIFI_STA);
    String ssid = WIFI_SSID;
    WiFi.begin(ssid, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        printDisplay("Connecting to " + ssid);
    }
    randomSeed(micros());
    printDisplay("Connected to " + ssid + ", IP Address " + WiFi.localIP().toString());
}

void reconnect()
{
    // Loop until we're reconnected
    while (!client.connected())
    {
        printDisplay("Attempting MQTT connection...");
        String clientId = "ESP8266Display-";
        clientId += String(random(0xffff), HEX);
        // Attempt to connect
        if (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASS))
        {
            printDisplay("Connected to MQTT");
            client.subscribe(MQTT_SUB_HUM);
            client.subscribe(MQTT_SUB_TEMP);
        }
        else
        {
            printDisplay("MQTT failed, try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
    printDisplay("Connected to MQTT");
}

void callback(char *topic, byte *payload, int length)
{
    String tp = (String)topic;
    if (tp == MQTT_SUB_HUM)
    {
        humidity = (char *)payload;
    }
    else if (tp == MQTT_SUB_TEMP)
    {
        temperature = (char *)payload;
    }
    displayTempHumid();
}

void setup()
{

    /**
     * Setup I2C Pins
     * GPIO0 [PIN 0] <-> SDA (Serial Data)
     * GPIO2 [PIN 2] <-> SCK (Serial Clock)
     */
    Wire.begin(0, 2);

    // Setup I2C Client Address (SSD1306 OLED - 0x3C)
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

    // Clear Display Bugger
    display.clearDisplay();

    // Connect to Wifi
    WiFi.onStationModeGotIP(onWifiConnect);
    WiFi.onStationModeDisconnected(onWifiDisconnect);
    connectToWifi();

    // configure mqtt
    client.setServer(MQTT_HOST, MQTT_PORT);
    client.setCallback(callback);
}

void loop()
{
    if (!client.connected())
    {
        reconnect();
    }
    client.loop();
}