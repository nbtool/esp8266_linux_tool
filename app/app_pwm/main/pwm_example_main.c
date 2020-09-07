/* pwm example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"
#include "esp_system.h"
#include "esp_err.h"

#include "esp8266/gpio_register.h"
#include "esp8266/pin_mux_register.h"

#include "driver/pwm.h"


#define PWM_1_OUT_IO_NUM    14

// PWM period 10us(100Khz), same as depth
#define PWM_PERIOD    (20000)

static const char *TAG = "pwm_example";

// pwm pin number
const uint32_t pin_num[1] = {
    PWM_1_OUT_IO_NUM
};

// dutys table, (duty/PERIOD)*depth
uint32_t duties[1] = {
    18700
};

// phase table, (phase/180)*depth
int16_t phase[1] = {
    0
};

void app_main()
{
    pwm_init(PWM_PERIOD, duties, 1, pin_num);
    pwm_set_channel_invert(0x1 << 0);
    pwm_set_phases(phase);
    pwm_start();

    int t = 0;
    while (1) {
        if(t%3 == 0){
            duties[0] = duties[0] == 18000 ? 18700:18000;
            pwm_set_duty(0,duties[0]);
            pwm_start();
        }
        t++;
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

