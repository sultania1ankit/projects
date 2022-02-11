#ifndef interface_H
#define interface_H

// #include <avr/io.h>
#include"basic_AVR.h"
  

// #advisory TODO: I need to finish this

#define blink_ddr DDRD  
#define blink_port PORTD
#define blink_pin 0b10000000
// #warning PORT not changed


void blink_setup();
void blink();
// ---------------------------------------------------

//Read
#define row_port PORTC
#define row_pin PINC
#define row_DDR DDRC
#define row_1 0b00000001
#define row_2 0b00001000
#define row_3 0b00000010
#define row_4 0b00000100
//Write 
#define col_port PORTD
#define col_DDR DDRD
#define col_1 0b10000000
#define col_2 0b01000000
#define col_3 0b00100000

void keypad_setup(); //4x3
uint8_t raw_keypad();
uint8_t get_keystroke(uint8_t display_key);
uint32_t get_pin(uint8_t len_pin); // max temp length is 4 
// ---------------------------------------------------


#define ssd1306_twi_write 0b01111000
#define ssd1306_twi_read 0b01111001
#define ssd1306_command_flag 0x80
#define ssd1306_data_flag 0x40

// TWI (start,stop,write,read_ack,read_nack,status)
uint8_t ssd1306_setup();
uint8_t ssd1306_single_cmd(uint8_t data);
uint8_t ssd1306_cmd(uint8_t *ptr_data,uint8_t size);
uint8_t ssd1306_data(uint8_t *ptr_data,uint8_t size);
uint8_t ssd1306_data_ROM(const uint8_t *ptr_data,uint8_t size);

#define ssd1306_ddr DDRB
#define ssd1306_port PORTB
#define ssd1306_reset 0b00000001
#define ssd1306_spi_DC 0b00000010

uint8_t ssd1306_spi_setup();

extern const uint8_t ssd1306_data1[];
extern const uint8_t ssd1306_data2[];
extern const uint8_t ssd1306_data3[];

//__________________________________________________________________________________________

#endif
