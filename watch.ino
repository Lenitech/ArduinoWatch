// define log
#define DEBUG_EN 1
#if DEBUG_EN == 1
#define log(x) Serial.println(x)
#else
#define log(x)
#endif
#define SCREEN_REFRESH_PERIOD 200
// we use u8g to draw graphics on the screen
// more about u8g library, please see https://github.com/olikraus/u8glib/wiki
#include "U8glib.h"
// for timer
#include <elapsedMillis.h>
elapsedMillis timeElapsed;
// construct the u8g object for our OLED display
// the display controller chip is SSD1306, it has been built into the screen.
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);
typedef enum {
    HOME_SCREEN,
} SCREEN_TOKEN;
/*
    For MPU6050
*/
#include <arduino_mpu.h>
#define PL(x) Serial.println(x)
#define P(x) Serial.print(x) 

#include <inv_mpu.h>
#include <inv_mpu_dmp_motion_driver.h>

#define DEFAULT_MPU_HZ 50

volatile bool dataReady = false;
void interrupt(){
  dataReady = true;
}

short gyro[3], accel[3], sensors;
unsigned char more;
long quat[4];
unsigned long sensor_timestamp;
unsigned short gyro_rate, gyro_fsr;
unsigned char accel_fsr;
float gyro_sens;
unsigned short accel_sens;
void setup() {
    // init serial port for log, this must be in the first line of setup() to
    // ensure log() works in functions below
    Serial.begin(115200);
    log(__func__);
    initDisplay();
    initMpu6050();
}

u8g_uint_t y = 64;
void loop() {
    if (timeElapsed > SCREEN_REFRESH_PERIOD) {
        timeElapsed = 0;
        displayLoop(HOME_SCREEN, 0, y);
    }
  if(dataReady){
    dmp_read_fifo(gyro, accel, quat, &sensor_timestamp, &sensors, &more);
    if(!more) dataReady=false;
  }
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

void initMpu6050() {
  struct int_param_s params;
  params.pin = 2;
  params.cb = interrupt;
  PL(mpu_init(&params));
  PL(mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL));
  PL(mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL));
  PL(mpu_set_gyro_fsr(500));
  PL(mpu_set_accel_fsr(2));

  PL(dmp_load_motion_driver_firmware());
  PL(mpu_set_dmp_state(1));
  PL(dmp_enable_feature(DMP_FEATURE_TAP | DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_SEND_CAL_GYRO | DMP_FEATURE_GYRO_CAL));
  PL(dmp_set_fifo_rate(DEFAULT_MPU_HZ));

  mpu_get_gyro_sens(&gyro_sens);
  mpu_get_accel_sens(&accel_sens);

  PL(dmp_register_tap_cb(tapEvent));
  PL("Running...");
  mpu_get_sample_rate(&gyro_rate); P("FIFOrate: ");P(gyro_rate);PL("Hz");
  mpu_get_gyro_fsr(&gyro_fsr); P("Gyro FSR: +/- ");P(gyro_fsr);PL("DPS");
  mpu_get_accel_fsr(&accel_fsr); P("Accel FSR: +/- ");P(accel_fsr);PL("G");
}

void tapEvent(unsigned char direction, unsigned char count){
  P("Tap tap Count: "); P(count);
  P(" | Direction: "); PL(direction);
    switch (direction) {
        case 3:
        case 4:
            y>64?y=64:y-=count;
            break;
        case 5:
        case 6:
            y>64?y=64:y+=count;
            break;
            
    }
}














