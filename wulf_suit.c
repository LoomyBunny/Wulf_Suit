/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"

/**
 * NOTE:
 *  Take into consideration if your WS2812 is a RGB or RGBW variant.
 *
 *  If it is RGBW, you need to set IS_RGBW to true and provide 4 bytes per 
 *  pixel (Red, Green, Blue, White) and use urgbw_u32().
 *
 *  If it is RGB, set IS_RGBW to false and provide 3 bytes per pixel (Red,
 *  Green, Blue) and use urgb_u32().
 *
 *  When RGBW is used with urgb_u32(), the White channel will be ignored (off).
 *
 */
#define IS_RGBW false
#define NUM_PIXELS 86

#ifdef PICO_DEFAULT_WS2812_PIN
#define WS2812_PIN PICO_DEFAULT_WS2812_PIN
#else
// default to pin 2 if the board doesn't have a default WS2812 pin defined
#define WS2812_PIN 28
#endif

#define MODE_SWITCH_PIN 0
#define BRIGHT_SWITCH_PIN 11

enum modes{
    EPO_ACTIVE, 
    EPO_DEBOUNCE,
    LEAVING_EPO,
    RED, // have all special cases before red
    TEAL,
    PURPLE,
    WARM_WHITE,
    COOL_WHITE,
    RAINBOW,
    WHITE // This must be the last enum for the rest of the code to work
};

uint32_t rainbowCount = 0;


static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return
            ((uint32_t) (r) << 8) |
            ((uint32_t) (g) << 16) |
            (uint32_t) (b);
}

uint32_t intToRainbow(uint32_t index){
    index = index % 1536;
    if(index<256){
        return urgb_u32(index     , 255       , 0);
    } else if (index < 512){
        return urgb_u32(255       , 511-index , 0);
    } else if (index < 768){
        return urgb_u32(255       , 0         , index - 512);
    } else if (index < 1024){
        return urgb_u32(1023-index, 0         , 255);
    } else if (index < 1280){  
        return urgb_u32(0         , index-1024, 255);
    } else {
        return urgb_u32(0         , 255       , 1535 - index);
    }
}

void pattern_black(uint len){
    for(int i = 0; i < len; i++){
        put_pixel(0);
    }
}

void pattern_color(uint len, uint32_t newColor){
    for(int i = 0; i < len; i++){
        put_pixel(newColor);
    }
}

void setColor(uint mode, uint32_t brightness){
    uint32_t newColor = 0;
    switch (mode)
    {
    case EPO_ACTIVE:
    case EPO_DEBOUNCE:
        break; // Color black
    case RED:
        newColor = urgb_u32(brightness,0,0);
        break;
    //case YELLOW:
    //    newColor = urgb_u32(brightness,brightness,0);
    //    break;
    //case GREEN:
    //    newColor = urgb_u32(0,brightness,0);
    //    break;
    case TEAL:
        newColor = urgb_u32(0,brightness,brightness);
        break;
    //case BLUE:
    //    newColor = urgb_u32(0,0,brightness);
    //    break;
    case PURPLE:
        newColor = urgb_u32(brightness,0,brightness);
        break;
    case WARM_WHITE:
        newColor = urgb_u32(brightness,brightness*.7,brightness*.7);
        break;
    case COOL_WHITE:
        newColor = urgb_u32(brightness*.7,brightness,brightness);
        break;
    case WHITE:
        newColor = urgb_u32(brightness,brightness,brightness);
        break;    
    case RAINBOW:
        newColor = intToRainbow(rainbowCount);
        rainbowCount = (rainbowCount+3);
        break;
    case LEAVING_EPO:
        newColor = urgb_u32(30,30,30);
        break;
    default:
        return; // should never hit, if you do just return
    }
    printf("The new color is %u!\n", newColor);
    pattern_color(2,newColor);
}

int main() {
    //set_sys_clock_48();
    stdio_init_all();
    printf("WS2812 Smoke Test, using pin %d", WS2812_PIN);

    gpio_init(MODE_SWITCH_PIN);
    gpio_init(BRIGHT_SWITCH_PIN);
    gpio_set_dir(MODE_SWITCH_PIN, GPIO_IN);
    gpio_set_dir(BRIGHT_SWITCH_PIN, GPIO_IN);
    gpio_pull_up(MODE_SWITCH_PIN);
    gpio_pull_up(BRIGHT_SWITCH_PIN);

    // todo get free sm
    PIO pio = pio0;
    int sm = 0;
    int mode_count = 0;
    int bright_count = 0;
    int both_count = 0;
    uint32_t mode =  PURPLE;
    int brightness = 1;
    uint offset = pio_add_program(pio, &ws2812_program);

    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    int t = 0;
    while (1) {
        if(!gpio_get(MODE_SWITCH_PIN) && !gpio_get(BRIGHT_SWITCH_PIN)){
            printf("BOTH!\n");
            mode_count = 0;
            bright_count = 0;
            switch (mode)
            {
            case EPO_ACTIVE:
            case LEAVING_EPO:
                if(both_count>150){
                    mode = LEAVING_EPO;
                }
                break;            
            default:
                if(both_count>1){
                    pattern_black(2);
                    mode = EPO_DEBOUNCE;
                }
                break;
            }
            both_count++;
        } else if (!gpio_get(MODE_SWITCH_PIN)) {
            printf("MODE!\n");
            bright_count = 0;
            both_count = 0;
            switch(mode){
                case EPO_ACTIVE:
                    break; //EPO modes, do nothing
                default:
                    if(mode_count==10){
                        mode++;
                        if(mode>WHITE){
                            mode = RED;
                        }
                    }
            }
            mode_count++;
        } else if (!gpio_get(BRIGHT_SWITCH_PIN)) {
            printf("BRIGHT!\n");
            both_count = 0;
            brightness++;
            if(brightness>255*3){
                brightness = 1;
            }
        } else {
            printf("None!!\n");
            both_count = 0;
            bright_count = 0;
            mode_count = 0;
            if(mode==EPO_DEBOUNCE){
                mode = EPO_ACTIVE;
            }
            if(mode==LEAVING_EPO){
                mode = RED;
            }
        }
        setColor(mode,brightness/3);
        sleep_ms(10);
    }
}

