// ESP8266 or ARDUINO with 16x2 i2c LCD
// Compatible with the Arduino IDE 1.8.3
// Library https://github.com/lucasmaziero/LiquidCrystal_I2C
// Original Library https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library
// Standard for ESP8266 with 4-SDA 5-SCL or ARDUINO with A4-SDA A5-SCL and LCD 16x2 display
// and access from ESP8266: lcd.begin(sda,scl) or ARDUINO: lcd.begin()
  
#include <LiquidCrystal_I2C.h>

uint8_t bell[8]  = {0x4, 0xe, 0xe, 0xe, 0x1f, 0x0, 0x4};
uint8_t note[8]  = {0x2, 0x3, 0x2, 0xe, 0x1e, 0xc, 0x0};
uint8_t clock[8] = {0x0, 0xe, 0x15, 0x17, 0x11, 0xe, 0x0};
uint8_t heart[8] = {0x0, 0xa, 0x1f, 0x1f, 0xe, 0x4, 0x0};
uint8_t duck[8]  = {0x0, 0xc, 0x1d, 0xf, 0xf, 0x6, 0x0};
uint8_t check[8] = {0x0, 0x1 ,0x3, 0x16, 0x1c, 0x8, 0x0};
uint8_t cross[8] = {0x0, 0x1b, 0xe, 0x4, 0xe, 0x1b, 0x0};
uint8_t retarrow[8] = {	0x1, 0x1, 0x5, 0x9, 0x1f, 0x8, 0x4};

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{
  	// initialize the LCD
  	lcd.begin(); //Init with pin default ESP8266 or ARDUINO
  	//lcd.begin(0, 2); //ESP8266-01 I2C with pin 0-SDA 2-SCL

	lcd.backlight();

	lcd.createChar(0, bell);
	lcd.createChar(1, note);
	lcd.createChar(2, clock);
	lcd.createChar(3, heart);
	lcd.createChar(4, duck);
	lcd.createChar(5, check);
	lcd.createChar(6, cross);
	lcd.createChar(7, retarrow);
	lcd.home();

	lcd.print("Hello world...");
	lcd.setCursor(0, 1);
	lcd.print(" i ");
	lcd.write(3);
	lcd.print(" arduinos!");
	delay(5000);
	displayKeyCodes();
}

// display all keycodes
void displayKeyCodes(void) {
	uint8_t i = 0;

	while (1) {
		lcd.clear();
		lcd.print("Codes 0x");
		lcd.print(i, HEX);
		lcd.print("-0x");
		lcd.print(i + 16, HEX);
		lcd.setCursor(0, 1);

		for (int j = 0; j < 16; j++) {
			lcd.write(i + j);
		}
		i += 16;

		delay(4000);
	}
}

void loop()
{
	// Do nothing here...
}

