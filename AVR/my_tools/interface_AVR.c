#include"interface_AVR.h"


// ________________________________________________________________________________Blinks____________________________________________________________________________________________________________

void blink_setup()
{
	blink_ddr|=blink_pin;
}

void blink()
{
	blink_port|=blink_pin;__delay_ms(50);
	blink_port&=~(blink_pin);__delay_ms(50);
}

// ________________________________________________________________________________3x4 KEYPAD____________________________________________________________________________________________________________

// Read rails will be pulled up, when key is pressed GND will be delivered.

void keypad_setup() //4x3
{
	col_DDR|=col_1|col_2|col_3;
	col_port&=~(col_1|col_2|col_3);
	row_DDR&=(~row_1&~row_2&~row_3&~row_4);
	row_port|=(row_1|row_2|row_3|row_4);
}

uint8_t raw_keypad()
{
	// keep 3 lines as Input and 4 as output - speed advantage 
		const uint8_t keys[12]={'1','2','3','4','5','6','7','8','9','*','0','#'};
		uint32_t key_count=0;
		col_port&=!(col_1|col_2|col_3);if((row_pin&row_1)!=row_1)
		{
			col_port|=(col_1|col_2);col_port&=~col_3;__delay_ms(1);if((row_pin&row_1)!=row_1){key_count+=0;return(keys[key_count]);}
			col_port|=(col_2|col_3);col_port&=~col_1;__delay_ms(1);if((row_pin&row_1)!=row_1){key_count+=1;return(keys[key_count]);}
			col_port|=(col_3|col_1);col_port&=~col_2;__delay_ms(1);if((row_pin&row_1)!=row_1){key_count+=2;return(keys[key_count]);}
		}
		col_port&=!(col_1|col_2|col_3);if((row_pin&row_2)!=row_2)
		{
			key_count+=3;
			col_port|=(col_1|col_2);col_port&=~col_3;__delay_ms(1);if((row_pin&row_2)!=row_2){key_count+=0;return(keys[key_count]);}
			col_port|=(col_2|col_3);col_port&=~col_1;__delay_ms(1);if((row_pin&row_2)!=row_2){key_count+=1;return(keys[key_count]);}
			col_port|=(col_3|col_1);col_port&=~col_2;__delay_ms(1);if((row_pin&row_2)!=row_2){key_count+=2;return(keys[key_count]);}
		}
		col_port&=!(col_1|col_2|col_3);if((row_pin&row_3)!=row_3)
		{
			key_count+=6;
			col_port|=(col_1|col_2);col_port&=~col_3;__delay_ms(1);if((row_pin&row_3)!=row_3){key_count+=0;return(keys[key_count]);}
			col_port|=(col_2|col_3);col_port&=~col_1;__delay_ms(1);if((row_pin&row_3)!=row_3){key_count+=1;return(keys[key_count]);}
			col_port|=(col_3|col_1);col_port&=~col_2;__delay_ms(1);if((row_pin&row_3)!=row_3){key_count+=2;return(keys[key_count]);}
		}
		col_port&=!(col_1|col_2|col_3);if((row_pin&row_4)!=row_4)
		{
			key_count+=9;
			col_port|=(col_1|col_2);col_port&=~col_3;__delay_ms(1);if((row_pin&row_4)!=row_4){key_count+=0;return(keys[key_count]);}
			col_port|=(col_2|col_3);col_port&=~col_1;__delay_ms(1);if((row_pin&row_4)!=row_4){key_count+=1;return(keys[key_count]);}
			col_port|=(col_3|col_1);col_port&=~col_2;__delay_ms(1);if((row_pin&row_4)!=row_4){key_count+=2;return(keys[key_count]);}
		}
		__delay_ms(10);
        return 0;
}

uint8_t get_keystroke(uint8_t display_key)
{
	const uint8_t key_msg[]="Key pressed ";
	uint8_t key1=raw_keypad();
	if(key1!=0)
		{ 
			__delay_ms(1);
			if(key1==raw_keypad()) // recheck is same key is pressed
		   {
			PORTB=0x00;while((PINC&(0b00001111))!=0b00001111){} // Debounce - wait until no key is pressed
			if(display_key==1){tx_string_ROM(&key_msg[0]);tx(key1);new_line;} 
			}
			}
    return key1;
}

uint32_t get_pin(uint8_t len_pin) // max temp length is 4 
{
	uint32_t digits_received=0;
	uint32_t temp_pin=0;
	do
	{
		uint8_t digit=0;
		while(digit==0){digit=get_keystroke(1);}
		if(digit=='*'){tx('*');new_line}
		else if(digit=='#'){tx('#');new_line}
		else
		{
			digit-='0';
			temp_pin+=digit*power(10,len_pin-1-digits_received);
			++digits_received;
		}

	} while (digits_received!=len_pin);
	return temp_pin;
}


// ______________________________________________________________________________SSD1306 I2C______________________________________________________________________________________________________________

uint8_t ssd1306_setup_cmd[]={
0xA8,0x3F    //MUX ratio A8,3F
,0xD3,0x00    //display offset D3,00
,0x40    //display start 40
,0xA0    //segment re-map A0/A1
,0xC0   //COM output scan direction C0/C8
,0xDA,0b00110010    //COM pins hardware configuration DA,02
,0x81,0x7F    //contrast 81,7F
,0xA4    //disable entire display ON A4
,0xA6    //normal display A6
,0xD5,0x80    //osc freq D5,80
,0x8D,0x14    //enable charge pump 8D,14
,0xAF    //display ON AF
};

uint8_t ssd1306_setup()
{
ssd1306_cmd(&ssd1306_setup_cmd[0],18);
return 1;
}


uint8_t ssd1306_cmd_single_cmd(uint8_t data)
{
twi_start();if(twi_stats()!=twi_status_start_tx){return twi_stats();}
// bit 1 is flexibility in address(SA0 is 0 in my module), bit 0 is (0-write, 1-read)
twi_write(ssd1306_twi_write); if(twi_stats()!=twi_status_start_tx_again){return twi_stats();}
twi_write(ssd1306_command_flag);if(twi_stats()!=twi_status_data_ack){return twi_stats();}
// 0x80 (Co = 1, D/C = 0): Next byte(s) are command (non-graphics) data, after which another control byte is expected.
// 0x40 (Co = 0, D/C = 1): Next byte(s) are graphs data, after which I2C stop condition is expected. 
twi_write(data);if(twi_stats()!=twi_status_data_ack){return twi_stats();}
twi_stop();
return 1;
}

uint8_t ssd1306_cmd(uint8_t *ptr_data,uint8_t size)
{
twi_start();if(twi_stats()!=twi_status_start_tx){return twi_stats();}
twi_write(ssd1306_twi_write); if(twi_stats()!=twi_status_start_tx_again){return twi_stats();}
do
{
twi_write(ssd1306_command_flag);if(twi_stats()!=twi_status_data_ack){return twi_stats();}
twi_write(*ptr_data);if(twi_stats()!=twi_status_data_ack){return twi_stats();}
ptr_data++;
}
while(--size);
twi_stop();
return 1;
}

uint8_t ssd1306_data(uint8_t *ptr_data,uint8_t size)
{
twi_start();if(twi_stats()!=twi_status_start_tx){return twi_stats();}
twi_write(ssd1306_twi_write); if(twi_stats()!=twi_status_start_tx_again){return twi_stats();}
twi_write(ssd1306_data_flag);if(twi_stats()!=twi_status_data_ack){return twi_stats();}
do
{
twi_write(*ptr_data);if(twi_stats()!=twi_status_data_ack){return twi_stats();}
ptr_data++;
}
while(--size);
twi_stop();
return 1;
}

uint8_t ssd1306_data_ROM(const uint8_t *ptr_data,uint8_t size)
{
twi_start();if(twi_stats()!=twi_status_start_tx){return twi_stats();}
twi_write(ssd1306_twi_write); if(twi_stats()!=twi_status_start_tx_again){return twi_stats();}
twi_write(ssd1306_data_flag);if(twi_stats()!=twi_status_data_ack){return twi_stats();}
do
{
twi_write(*ptr_data);if(twi_stats()!=twi_status_data_ack){return twi_stats();}
ptr_data++;
}
while(--size);
twi_stop();
return 1;
}


// ________________________________________________________________________________________SSD1306 SPI____________________________________________________________________________________________________

uint8_t ssd1306_spi_setup()
{
	ssd1306_ddr|=ssd1306_reset|ssd1306_spi_DC;
	ssd1306_port&=~ssd1306_reset;
	__delay_ms(5);
	ssd1306_port|=ssd1306_reset;
	__delay_ms(5);
	ssd1306_port&=~ssd1306_spi_DC; // low is Command
	// spi_single_tx(0xAF);
	spi_tx(&ssd1306_setup_cmd[0],18);
	return 1;
}



// ________________________________________________________________________________________SSD1306 Patterns____________________________________________________________________________________________________


const uint8_t ssd1306_data1[]={	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
							0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
							0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
							0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

const uint8_t ssd1306_data2[]={	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
							0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
							0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
							0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

const uint8_t ssd1306_data3[]={	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
							0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
							0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
							0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};

// ____________________________________________________________________________________________________________________________________________________________________________________________

	