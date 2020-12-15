#include <Arduino.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <Wire.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

Adafruit_SSD1306 display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

void displayTempHumid()
{
    delay(2000);

    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print("T ");
    display.print("19.06");
    display.print(" \370C");
    display.setCursor(0, 18);
    display.print("H ");
    display.print("10.90");
    display.print(" %\t");
    display.display();
}

void setup()
{

    /**
     * SET
     * GPIO0 [PIN 0] <-> SDA (Serial Data)
     * GPIO2 [PIN 2] <-> SCK (Serial Clock)
     */
    Wire.begin(0, 2);

    /**
     * SSD1306 OLED Display I2C Address - 0x3C
     */
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

    /**
     * CLEAR BUFFER
     */
    display.clearDisplay();
}

void loop()
{
    displayTempHumid();
}