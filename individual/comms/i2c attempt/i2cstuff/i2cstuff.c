
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include <stdio.h>
#include <stdlib.h>


static int addr = 0x42;
i2c_inst_t *i2c = i2c0;

void send(uint8_t *msg, int length);

int main() {
    //stdio_init_all;

    gpio_init(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_init(PICO_DEFAULT_I2C_SCL_PIN);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    i2c_init(i2c0, 100000);
    i2c_set_slave_mode(i2c0, true, addr);
    //bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));
    //printf("here");

    uint8_t data[10] = "test";
    uint8_t txdata[1];
    txdata[0] = 97;
    while(true){
        /*
        if (i2c_get_read_available(i2c0) >= 1){
            //i2c_write_blocking(i2c0, addr, data, 5, false);
        }
        */
        //send(&data[0], 4);
        i2c_write_raw_blocking(i2c0, txdata, 1);
        
    }
    
    return 0;
}

void send(uint8_t *msg, int length) {
    uint8_t tmp[length + 1];
    for(int i = 0; i < length; i++){
        tmp[i] = msg[i];
    }
    tmp[length + 1] = '\0';
    i2c_write_blocking(i2c0, addr, tmp, length + 1, false);
}

/*
bool reserved_addr(uint8_t addr) {
    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}

int main(void){
    const uint led_pin = 25;

    // Initialize LED pin
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);

    // Print stuff over serial
    stdio_init_all();

    // i2c_default definition found here: https://raspberrypi.github.io/pico-sdk-doxygen/rp2__common_2hardware__i2c_2include_2hardware_2i2c_8h_source.html
    i2c_init(i2c_default, 100 * 1000); //use i2c0 controller, 100kHz clock

    

    // PICO_DEFAULT_I2C_SDA_PIN definition found here: https://github.com/raspberrypi/pico-sdk/blob/master/src/boards/include/boards/pico.h
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C); // GPIO Pin 4
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C); // GPIO Pin 5
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN); // GPIO Pin 4
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN); // GPIO Pin 5


    i2c_set_slave_mode(i2c_default, true, 0x42);
    sleep_ms(10000);
    printf("\nDude\n");
    while(true){
        printf("\nI2C Bus Scan\n");
        printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");

        for (int addr = 0; addr < (1 << 7); ++addr) {
            if (addr % 16 == 0) {
                printf("%02x ", addr);
            }

            // Perform a 1-byte dummy read from the probe address. If a slave
            // acknowledges this address, the function returns the number of bytes
            // transferred. If the address byte is ignored, the function returns
            // -1.

            // Skip over any reserved addresses.
            int ret;
            uint8_t rxdata;
            if (reserved_addr(addr))
                ret = PICO_ERROR_GENERIC;
            else
                ret = i2c_write_blocking(i2c_default, addr, &rxdata, 1, false);

            printf(ret < 0 ? "." : "@");
            printf(addr % 16 == 15 ? "\n" : "  ");
        }
        printf("Done.\n");
        sleep_ms(5000);
    }
}
*/