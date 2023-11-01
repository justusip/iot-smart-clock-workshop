#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <Fonts/TomThumb.h>

// Initialize an Adafruit NeoMatrix instance
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(
    32, // Matrix width in pixels.
    8,  // Matrix height in pixels.
    13, // ESP32 pin number for NeoPixel data out.
    // Matrix layout settings - add together NEO_MATRIX_* values to declare orientation, rotation, etc.
    NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE + NEO_MATRIX_ZIGZAG,
    // NeoPixel LED type settings
    NEO_GRB + NEO_KHZ800
);

// The setup function is called once at startup
void setup() {
    // Setting preferences for the matrix instance
    matrix.begin();
    matrix.setTextWrap(false);
    matrix.setBrightness(30);
    matrix.setFont(&TomThumb);

    // Print out a simple "Hello!" message
    matrix.print("Hello!");
    matrix.show();
}

// The loop function is called repeatedly after the setup function is finished.
void loop() {

}