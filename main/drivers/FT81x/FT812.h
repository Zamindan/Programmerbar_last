#ifndef FT812_H

#include <stdio.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"
#include <inttypes.h>


#include <stdio.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LCD_TAG "LCD"

// Memory map (Start addresses)
#define RAM_G 0x000000
#define ROM_FONT 0x1E0000
#define ROM_FONT_ADDR 0x2FFFFC
#define RAM_DL 0x300000
#define RAM_REG 0x302000
#define RAM_CMD 0x308000

// Register Address Definitions
#define REG_TAP_MASK 0x302028
#define REG_HCYCLE 0x30202C
#define REG_HOFFSET 0x302030
#define REG_HSIZE 0x302034
#define REG_HSYNC0 0x302038
#define REG_HSYNC1 0x30203C
#define REG_VCYCLE 0x302040
#define REG_VOFFSET 0x302044
#define REG_VSIZE 0x302048
#define REG_VSYNC0 0x30204C
#define REG_VSYNC1 0x302050
#define REG_DLSWAP 0x30205A
#define REG_ROTATE 0x302058
#define REG_OUTBITS 0x30205C
#define REG_DITHER 0x302060
#define REG_SWIZZLE 0x30206A
#define REG_CSPREAD 0x302068
#define REG_PCLK_POL 0x30206C
#define REG_PCLK 0x302070
#define REG_TAG_X 0x30207A
#define REG_TAG_Y 0x302078
#define REG_TAG 0x30207C
#define REG_VOL_PB 0x302080
#define REG_VOL_SOUND 0x30208A
#define REG_SOUND 0x302088
#define REG_PLAY 0x30208C
#define REG_GPIO_DIR 0x302090
#define REG_GPIO 0x302094
#define REG_GPIOX_DIR 0x302098
#define REG_GPIOX 0x30209C
#define REG_INT_FLAGS 0x3020A8
#define REG_INT_EN 0x3020AC
#define REG_INT_MASK 0x3020B0
#define REG_PLAYBACK_START 0x3020B4
#define REG_PLAYBACK_LENGTH 0x3020B8
#define REG_PLAYBACK_READPTR 0x3020BC
#define REG_PLAYBACK_FREQ 0x3020C0
#define REG_PLAYBACK_FORMAT 0x3020C4
#define REG_PLAYBACK_LOOP 0x3020C8
#define REG_PLAYBACK_PLAY 0x3020CC
#define REG_PWM_HZ 0x3020D0
#define REG_PWM_DUTY 0x3020D4
#define REG_MACRO_0 0x3020D8
#define REG_MACRO_1 0x3020DC
#define REG_CMD_READ 0x3020F8
#define REG_TOUCH_TAG3_XY 0x302140
#define REG_TOUCH_TAG3 0x302144
#define REG_TOUCH_TAG4_XY 0x302148
#define REG_TOUCH_TAG4 0x30214C
#define REG_TOUCH_TRANSFORM_A 0x302150
#define REG_TOUCH_TRANSFORM_B 0x302154
#define REG_TOUCH_TRANSFORM_C 0x302158
#define REG_TOUCH_TRANSFORM_D 0x30215C
#define REG_TOUCH_TRANSFORM_E 0x302160
#define REG_TOUCH_TRANSFORM_F 0x302164
#define REG_TOUCH_CONFIG 0x302168
#define REG_CTOUCH_TOUCH4_X 0x30216C
#define REG_BIST_EN 0x302174
#define REG_TRIM 0x302180
#define REG_ANA_COMP 0x30218A
#define REG_SPI_WIDTH 0x30218B
#define REG_TOUCH_DIRECT_XY 0x30218C
#define REG_CTOUCH_TOUCH2_XY 0x30218C
#define REG_TOUCH_DIRECT_Z12 0x302190
#define REG_CTOUCH_TOUCH3_XY 0x302190
#define REG_DATESTAMP 0x30256A
#define REG_CMDB_SPACE 0x30257A
#define REG_CMDB_WRITE 0x30257B

#define CLOCK_10_MHz 10000000
#define CLOCK_30_MHz 30000000

// Host Command
#define ACTIVE 0x00
#define STANDBY 0x41
#define SLEEP 0x42
#define PWRDOWN 0x43
#define CLKEXT 0x44
#define CLKINT 0x48
#define PD_ROMS 0x49
#define RST_PULSE 0x68
#define PINDRIVE 0x70
#define PIN_PD_STATE 0x71

// Read/Write Commands
#define WRITE 0x41
#define READ 0x80

// Drawing Primitives
#define BITMAPS 1
#define POINTS 2
#define LINES 3        
#define LINE_STRIP 4   
#define EDGE_STRIP_R 5 
#define EDGE_STRIP_L 6 
#define EDGE_STRIP_A 7 
#define EDGE_STRIP_B 8 
#define RECTS 9        

uint32_t BEGIN(uint8_t prim);

uint32_t CLEAR_COLOR_RGB(uint8_t red, uint8_t green, uint8_t blue);


uint32_t CLEAR(uint8_t c, uint8_t s, uint8_t t);


uint32_t COLOR_RGB(uint8_t red, uint8_t green, uint8_t blue);


uint32_t POINT_SIZE(uint16_t size);


uint32_t DISPLAY();

uint32_t END();


uint32_t VERTEX2II(uint16_t x, uint16_t y, uint8_t handle, uint8_t cell);

#endif