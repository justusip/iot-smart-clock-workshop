## 8. Upload Sample Program

In this section, you can program the ESP32 to display messages on the LED matrix using the Arduino IDE and C++
programming language.

### 8.1. Installing Arduino IDE

1. Go to the Arduino website at https://www.arduino.cc/en/software
2. Download the Arduino IDE for your operating system (Windows, Mac, or Linux).
3. Install the Arduino IDE by following the on-screen instructions.

### 8.2. Installing ESP32 Support

1. Open the Arduino IDE.
2. Go to File > Preferences.
3. In the "Additional Boards Manager URLs" field, enter the following
   URL: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
4. Click the "OK" button.
5. Go to Tools > Board > Boards Manager.
6. Search for "ESP32" and click the "Install" button for the "ESP32 by Espressif Systems" board
7. Wait for the installation to complete.

### 8.3. Installing Adafruit_NeoMatrix Library

1. Go to Sketch > Include Library > Manage Libraries.
2. In the Library Manager, search for "Adafruit_NeoMatrix".
3. Click on the "Adafruit_NeoMatrix" library.
4. Select the latest version of the library (if multiple versions are available).
5. Click the "Install" button.
6. Wait for the installation to complete.

### 8.4. Uploading your first sketch to ESP32

1. Start a new sketch.
2. Enter the following code:
   ```C++
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
   ```
3. Connect the ESP32 to your computer via a USB-C cable.
4. Click the Upload button on the top left of the Arduino IDE (A circle button with an arrow pointing to the right).
5. Wait for the Arduino IDE to flash the program into ESP32.
6. After the program is flashed, the ESP32 board will restart automatically and execute the program. You should see
   the text “Hello!” showing on the screen.


