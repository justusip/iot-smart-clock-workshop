#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <Fonts/TomThumb.h>

#include <WiFi.h>
#include <esp_wpa2.h>
#include <esp_wifi.h>
#include <HTTPClient.h>
#include <time.h>

#include <ArduinoJson.h>

#include "Icons.h"

#define arr_len(x) (sizeof(x) / sizeof(*x))

#define WS2812_MATRIX_DATA_PIN 13

#define WIFI_SSID "Superman"
#define WIFI_PASSWORD "ji020717"

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(
        32,
        8,
        WS2812_MATRIX_DATA_PIN,
        NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE + NEO_MATRIX_ZIGZAG,
        NEO_GRB + NEO_KHZ800
);

class Page {
protected:
    void drawBitmap(int16_t x, int16_t y, const uint16_t *bitmap, int16_t w, int16_t h) {
        uint16_t buf[w * h];
        for (uint16_t pixel = 0; pixel < w * h; pixel++) {
            uint8_t r, g, b;
            uint16_t color = pgm_read_word(bitmap + pixel);

            b = (color & 0xF00) >> 8;
            g = (color & 0x0F0) >> 4;
            r = (color & 0x00F) >> 0;

            b = map(b, 0, 15, 0, 31);
            g = map(g, 0, 15, 0, 63);
            r = map(r, 0, 15, 0, 31);

            buf[pixel] = (r << 11) + (g << 5) + b;
        }
        matrix.drawRGBBitmap(x, y, buf, w, h);
    }

public:
    virtual void setup() {};

    virtual void render() = 0;
};

class PageText : public Page {
public:
    const IconSequence *icon;
    char text[128];
    int bgColour;
    int fgColour;

    PageText(const IconSequence *icon, char *text, uint16_t bgColour, uint16_t fgColour) :
            icon(icon),
            bgColour(bgColour),
            fgColour(fgColour) {
        strcpy(this->text, text);
    }

    void render() override {
        int16_t bx2, by2;
        uint16_t bw, bh;
        matrix.getTextBounds(text, 0, 0, &bx2, &by2, &bw, &bh);
        matrix.setTextColor(fgColour);

        bool isTextScrolling = bw > (matrix.width() - 8);
        int displayTime = isTextScrolling ? (matrix.width() - 8) + bw + 1 : 40;
        int cursorX = matrix.width();
        uint16_t frameId = 0;
        for (int i = 0; i < displayTime; i++) {
            matrix.fillScreen(bgColour);

            matrix.setCursor(isTextScrolling ? cursorX : 12, 7);
            // matrix.setCursor(isTextScrolling ? cursorX : 9, 7);
            matrix.print(text);

            if (this->icon != nullptr) {
                this->drawBitmap(0, 0, &this->icon->frames[frameId / 2 * this->icon->width * this->icon->height], 8, 8);
                frameId = (frameId + 1) % (this->icon->count * 2);
            }

            matrix.show();
            
            cursorX--;
            delay(50);
        }
    }
};

class PageTime : public PageText {
private:
    int gmtOffset;
public:
    explicit PageTime(int gmtOffset) :
            // icon_7645 - clock icon
            PageText(&icon_5588, "?", matrix.Color(0, 0, 0), matrix.Color(255, 255, 255)),
            gmtOffset(gmtOffset) {

    }

    void setup() override {
        Serial.println("[PageTime] Resolving time from NTP server...");
        configTime(this->gmtOffset * 60 * 60, 0, "pool.ntp.org");
    }

    void render() override {
        struct tm timeinfo;
        if (!getLocalTime(&timeinfo)) {
            Serial.println("[PageTime] Unable to obtain time.");
            return;
        }
        Serial.printf("[PageTime] Current time: %s\n", this->text);

        strftime(this->text, sizeof(this->text), "%H:%M", &timeinfo);
        PageText::render();
        // delay(2000);

        // strftime(this->text, sizeof(this->text), "%d/%m", &timeinfo);
        // PageText::render();

        // strftime(this->text, sizeof(this->text), "%a", &timeinfo);
        // PageText::render();
    }
};


class PageWeather : public PageText {
public:
    explicit PageWeather() :
            PageText(&icon_2369, "?", matrix.Color(0, 0, 0), matrix.Color(255, 255, 255)) {

    }

    void setup() override {
        Serial.println("[PageWeather] Getting current weather...");

        HTTPClient http;
        http.begin("https://data.weather.gov.hk/weatherAPI/opendata/weather.php?dataType=rhrread");
        int httpCode = http.GET();
        if (httpCode != HTTP_CODE_OK) {
            Serial.printf("[PageWeather] GET Request failed, error: %s\n", http.errorToString(httpCode).c_str());
            return;
        }

        Serial.printf("[PageWeather] GET Request succeed, respond code: %d\n", httpCode);
        String payload = http.getString();
        Serial.println(payload);

        DynamicJsonDocument doc(8192);
        deserializeJson(doc, payload);

        int temperature = doc["temperature"]["data"][0]["value"].as<int>();
        Serial.printf("[PageWeather] Current weather: %d\n", temperature);
        snprintf(this->text, sizeof(this->text), "%dc", temperature);

        http.end();
    }
};

Page *pages[] = {
        new PageTime(8),
        // new PageWeather(),
        // new PageText(&icon_5588, "Have a nice day!", matrix.Color(0, 0, 0), matrix.Color(255, 255, 255)),
        //icon_1769
};
Page *pageNoWifi = new PageText(&icon_26910, "No Wifi", matrix.Color(0, 0, 0), matrix.Color(255, 255, 255));

int pageIdx = 0;

void setup() {
    Serial.begin(115200);
    Serial.printf("\n\n\nWelcome!\n");

    matrix.begin();
    matrix.setTextWrap(false);
    matrix.setBrightness(10);
    matrix.setFont(&TomThumb);

    WiFi.disconnect(true);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void loop() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.printf("[WiFi] Connecting to %s.\n", WIFI_SSID);
        pageNoWifi->render();
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.printf(".");
        }
        Serial.printf("\n");
        Serial.printf("[WiFi] Wifi Connected.\n");
        Serial.printf("[WiFi] Setting up pages...\n");
        for (auto *page: pages) {
            page->setup();
        }
    }
    pages[pageIdx]->render();
    pageIdx = (pageIdx + 1) % arr_len(pages);
}