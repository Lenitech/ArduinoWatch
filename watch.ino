// define log
#define DEBUG_EN 1
#if DEBUG_EN == 1
#define log(x) Serial.println(x)
#else
#define log(x)
#endif
// we use u8g to draw graphics on the screen
// more about u8g library, please see https://github.com/olikraus/u8glib/wiki
#include "U8glib.h"
// construct the u8g object for our OLED display
// the display controller chip is SSD1306, it has been built into the screen.
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);
typedef enum {
    HOME_SCREEN,
} SCREEN_TOKEN;

void setup() {
    // init serial port for log, this must be in the first line of setup() to
    // ensure log() works in functions below
    Serial.begin(115200);
    log(__func__);
    initDisplay();
}

void loop() {
    static u8g_uint_t y = 64;
    displayLoop(HOME_SCREEN, 0, y==0?y=64:--y);
    delay(100);
}
void initDisplay() {
    log(__func__);
    // assign default color value
    u8g.setColorIndex(1);         // pixel on
}
// display related procedures
void displayLoop(SCREEN_TOKEN screen, u8g_uint_t x, u8g_uint_t y) {
    u8g.firstPage();
    switch (screen) {
        case HOME_SCREEN:
            u8g.setFont(u8g_font_fub14);   
            do {
                u8g.drawStr(x, y, "hello world");
            } while (u8g.nextPage());
            break;
    }
}













