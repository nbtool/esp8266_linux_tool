/* gpio example

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

#include "driver/gpio.h"

#include "esp_log.h"
#include "esp_system.h"

static const char *TAG = "main";

#define GPIO_D4             2
#define GPIO_D5             14
#define GPIO_D6             12
#define GPIO_D7             13
#define GPIO_D8             15

#define GPIO_SCK            GPIO_D5   
#define GPIO_MISO           GPIO_D6
#define GPIO_MOSI           GPIO_D7
#define GPIO_NSS            GPIO_D4


#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_SCK) | (1ULL<<GPIO_MOSI) | (1ULL<<GPIO_NSS))
#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_MISO))


char digital[10] = {0xFC,0x60,0xDA,0xF2,0x66,0xB6,0xBE,0xE0,0xFE,0xF6};
char position[3] = {0x80,0x40,0x20};

void app_show_digital(int d){
    for(int i=0;i<16;i++){
        gpio_set_level(GPIO_MOSI, (d>>i) & 0x01);
        gpio_set_level(GPIO_SCK, 0);
        gpio_set_level(GPIO_SCK, 1);
    }

    gpio_set_level(GPIO_NSS, 0);
    gpio_set_level(GPIO_NSS, 1);
}

void app_show_num(int num){
    if(num < 0 || num > 999)return;

    static int show_pos = 0;
    int gsb[3];//个十百
    gsb[0] = digital[num%10];//个
    gsb[1] = digital[num/10%10];//十
    gsb[2] = digital[num/100];//百
    app_show_digital(gsb[show_pos] << 8 | position[show_pos]);

    show_pos++;
    if(show_pos == 3)show_pos = 0;
}

char app_read_key(void){
    char data = 0;
    for(int i=0;i<8;i++){
        gpio_set_level(GPIO_SCK, 0);
        gpio_set_level(GPIO_SCK, 1);
        char bit = gpio_get_level(GPIO_MISO) == 0?0:1;
        data <<= 1;
        data |= bit;
    }
    return data;
}

int show_num = 0;
static void gpio_task_example(void *arg)
{
    while (1) {
        app_show_num(show_num);
        ESP_LOGI(TAG, "key: %X\n", app_read_key());
        vTaskDelay(9 / portTICK_RATE_MS);
    }
}

void app_main(void)
{
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO15/16
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    //start gpio task
    xTaskCreate(gpio_task_example, "gpio_task_example", 4096, NULL, 20, NULL);

    while (1) {
        vTaskDelay(1000 / portTICK_RATE_MS);
        show_num++;
        if(show_num == 1000)show_num = 0;
    }
}


