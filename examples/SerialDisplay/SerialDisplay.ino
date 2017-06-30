// ESP8266 or ARDUINO with 16x2 i2c LCD
// Compatible with the Arduino IDE 1.8.3
// Library https://github.com/lucasmaziero/LiquidCrystal_I2C
// Original Library https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library
// Standard for ESP8266 with 4-SDA 5-SCL or ARDUINO with A4-SDA A5-SCL and LCD 16x2 display
// and access from ESP8266: lcd.begin(sda,scl) or ARDUINO: lcd.begin()
  
/**
 * Displays text sent over the serial port (e.g. from the Serial Monitor) on
 * an attached LCD.
 */
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{
  	// initialize the LCD
  	lcd.begin(); //Init with pin default ESP8266 or ARDUINO
  	//lcd.begin(0, 2); //ESP8266-01 I2C with pin 0-SDA 2-SCL

	lcd.backlight();
  
	// Initialize the serial port at a speed of 9600 baud
	Serial.begin(9600);
}

void loop()
{
	// If characters arrived over the serial port...
	if (Serial.available()) {
		// Wait a bit for the entire message to arrive
		delay(100);
		// Clear the screen
		lcd.clear();

		// Write all characters received with the serial port to the LCD.
		while (Serial.available() > 0) {
			lcd.write(Serial.read());
		}
	}
}
