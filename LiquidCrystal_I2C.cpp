/**************************************************************************
LIQUIDCRYSTAL_I2C FOR ESP8266 OR ARDUINO
***************************************************************************
AUTHOR: LUCAS MAZIERO - Electrical Engineer
EMAIL: lucas.mazie.ro@hotmail.com
CITY: Santa Maria - Rio Grande do Sul - Brasil
***************************************************************************
Version: 1.1
Date:  30/06/2017
Modified: 02/07/2017
***************************************************************************
CODE BASE: https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library
***************************************************************************
CHANGELOG:
* 30/06/2017 (1.0v):
    -> add degug #if defined(ESP8266)
    -> Add new function "begin(uint8_t sda, uint8_t scl)"
    -> Add new function "init()"
* 02/07/2017 (1.1v):
    -> Add new function "getBacklight()" get status of backlight

***************************************************************************
Copyright(2017) by: Lucas Maziero.
**************************************************************************/

#include "LiquidCrystal_I2C.h"

// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set:
//    DL = 1; 8-bit interface data
//    N = 0; 1-line display
//    F = 0; 5x8 dot character font
// 3. Display on/off control:
//    D = 0; Display off
//    C = 0; Cursor off
//    B = 0; Blinking off
// 4. Entry mode set:
//    I/D = 1; Increment by 1
//    S = 0; No shift
//
// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// LiquidCrystal constructor is called).

LiquidCrystal_I2C::LiquidCrystal_I2C(uint8_t lcd_addr, uint8_t lcd_cols, uint8_t lcd_rows, uint8_t charsize, TwoWire & wire):
	_addr(lcd_addr),
    _displayfunction(0),
    _displaycontrol(0),
    _displaymode(0),
	_cols(lcd_cols),
	_rows(lcd_rows),
	_charsize(charsize),
	_backlightval(LCD_BACKLIGHT),
    _wire(wire)
{}

#if defined(ESP8266)
bool LiquidCrystal_I2C::begin(uint8_t sda, uint8_t scl){
	return _wire.begin(sda,scl)
	    && init();
}

#else
bool LiquidCrystal_I2C::begin(){
	return _wire.begin()
	    && init();
}
#endif

bool LiquidCrystal_I2C::init(){

	_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;

	if (_rows > 1) {
		_displayfunction |= LCD_2LINE;
	}

	// for some 1 line displays you can select a 10 pixel high font
	if ((_charsize != 0) && (_rows == 1)) {
		_displayfunction |= LCD_5x10DOTS;
	}

	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// according to datasheet, we need at least 40ms after power rises above 2.7V
	// before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
	delay(50);

	// Now we pull both RS and R/W low to begin commands
	if (!expanderWrite(_backlightval)) return false;	// reset expanderand turn backlight off (Bit 8 =1)
	delay(1000);

	//put the LCD into 4 bit mode
	// this is according to the hitachi HD44780 datasheet
	// figure 24, pg 46

	// we start in 8bit mode, try to set 4 bit mode
	if (!write4bits(0x03 << 4)) return false;
	delayMicroseconds(4500); // wait min 4.1ms

	// second try
	if (!write4bits(0x03 << 4)) return false;
	delayMicroseconds(4500); // wait min 4.1ms

	// third go!
	if (!write4bits(0x03 << 4)) return false;
	delayMicroseconds(150);

	// finally, set to 4-bit interface
	if (!write4bits(0x02 << 4)) return false;

	// set # lines, font size, etc.
	if (!command(LCD_FUNCTIONSET | _displayfunction)) return false;

	// turn the display on with no cursor or blinking default
	_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	display();

	// clear it off
	if (!clear()) return false;

	// Initialize to default text direction (for roman languages)
	_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;

	// set the entry mode
	if (!command(LCD_ENTRYMODESET | _displaymode)) return false;

	return home();
}

/********** high level commands, for the user! */
bool LiquidCrystal_I2C::clear(){
	if (!command(LCD_CLEARDISPLAY)) return false;// clear display, set cursor position to zero
	delayMicroseconds(2000);  // this command takes a long time!
    return true;
}

bool LiquidCrystal_I2C::home(){
	if (!command(LCD_RETURNHOME)) return false;  // set cursor position to zero
	delayMicroseconds(2000);  // this command takes a long time!
    return true;
}

bool LiquidCrystal_I2C::setCursor(uint8_t col, uint8_t row){
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if (row > _rows) {
		row = _rows-1;    // we count rows starting w/0
	}
	return command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
bool LiquidCrystal_I2C::noDisplay() {
	_displaycontrol &= ~LCD_DISPLAYON;
	return command(LCD_DISPLAYCONTROL | _displaycontrol);
}
bool LiquidCrystal_I2C::display() {
	_displaycontrol |= LCD_DISPLAYON;
	return command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
bool LiquidCrystal_I2C::noCursor() {
	_displaycontrol &= ~LCD_CURSORON;
	return command(LCD_DISPLAYCONTROL | _displaycontrol);
}
bool LiquidCrystal_I2C::cursor() {
	_displaycontrol |= LCD_CURSORON;
	return command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
bool LiquidCrystal_I2C::noBlink() {
	_displaycontrol &= ~LCD_BLINKON;
	return command(LCD_DISPLAYCONTROL | _displaycontrol);
}
bool LiquidCrystal_I2C::blink() {
	_displaycontrol |= LCD_BLINKON;
	return command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
bool LiquidCrystal_I2C::scrollDisplayLeft(void) {
	return command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
bool LiquidCrystal_I2C::scrollDisplayRight(void) {
	return command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
bool LiquidCrystal_I2C::leftToRight(void) {
	_displaymode |= LCD_ENTRYLEFT;
	return command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
bool LiquidCrystal_I2C::rightToLeft(void) {
	_displaymode &= ~LCD_ENTRYLEFT;
	return command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
bool LiquidCrystal_I2C::autoscroll(void) {
	_displaymode |= LCD_ENTRYSHIFTINCREMENT;
	return command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
bool LiquidCrystal_I2C::noAutoscroll(void) {
	_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
	return command(LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
bool LiquidCrystal_I2C::createChar(uint8_t location, uint8_t charmap[]) {
	location &= 0x7; // we only have 8 locations 0-7
	if (!command(LCD_SETCGRAMADDR | (location << 3))) return false;
	for (int i=0; i<8; i++) {
		if (!write(charmap[i])) return false;
	}
    return true;
}

// Turn the (optional) backlight off/on
bool LiquidCrystal_I2C::noBacklight(void) {
	_backlightval=LCD_NOBACKLIGHT;
	return expanderWrite(0);
}

bool LiquidCrystal_I2C::backlight(void) {
	_backlightval=LCD_BACKLIGHT;
	return expanderWrite(0);
}

// Get status of blacklight
bool LiquidCrystal_I2C::getBacklight() {
  return _backlightval == LCD_BACKLIGHT;
}

/*********** mid level commands, for sending data/cmds */

inline bool LiquidCrystal_I2C::command(uint8_t value) {
	return send(value, 0);
}

inline size_t LiquidCrystal_I2C::write(uint8_t value) {
	return send(value, Rs);
}


/************ low level data pushing commands **********/

// write either command or data
bool LiquidCrystal_I2C::send(uint8_t value, uint8_t mode) {
	uint8_t highnib=value&0xf0;
	uint8_t lownib=(value<<4)&0xf0;
	return write4bits((highnib)|mode)
	    && write4bits((lownib)|mode);
}

bool LiquidCrystal_I2C::write4bits(uint8_t value) {
	return expanderWrite(value)
	    && pulseEnable(value);
}

bool LiquidCrystal_I2C::expanderWrite(uint8_t _data){
	_wire.beginTransmission(_addr);
	const auto size = _wire.write((int)(_data) | _backlightval);
    if (size != 1) return false;
	const auto err = _wire.endTransmission();
    return err == 0;
}

bool LiquidCrystal_I2C::pulseEnable(uint8_t _data){
	if (!expanderWrite(_data | En)) return false;	// En high
	delayMicroseconds(1);		// enable pulse must be >450ns

	if (!expanderWrite(_data & ~En)) return false;	// En low
	delayMicroseconds(50);		// commands need > 37us to settle
    return true;
}

bool LiquidCrystal_I2C::load_custom_character(uint8_t char_num, uint8_t *rows){
	return createChar(char_num, rows);
}

bool LiquidCrystal_I2C::setBacklight(uint8_t new_val){
	if (new_val) {
		return backlight();		// turn backlight on
	} else {
		return noBacklight();		// turn backlight off
	}
}

size_t LiquidCrystal_I2C::printstr(const char c[]){
	//This function is not identical to the function used for "real" I2C displays
	//it's here so the user sketch doesn't have to be changed
	return print(c);
}
