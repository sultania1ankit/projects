/*
 * RTOS_try1.c
 * Created: 18-03-2020 13:40:50
 * Author : ColdMango
 */ 

//#include<xc.h>
//#include<avr/eeprom.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000
#include <util/delay.h>

#include"/home/ankit_sultania/MPLABXProjects/m328_tools/basic_AVR.h"
#include"/home/ankit_sultania/MPLABXProjects/m328_tools/interface_AVR.h"

	
//________________________________Functions___________________________________________________

ISR(TIMER2_COMPA_vect)
{
    ++t_ms;
}

ISR(USART_RX_vect)
{
    rx_put();
}
/*<<
//-------------------------setup-----------------------------
 
 * millis_setup()
 * twi_setup()
 * EEPROM_display()  // slow and inefficient - for debugging only
 * uint8_t ssd1306_setup()
 
//-------------------------basic-----------------------------
 * uint8_t power(uint8_t _base,uint8_t _power)
 * void blink()
  
 * void USART_init()
 * new_line while(!(UCSR0A & (1<<UDRE0))){};UDR0='\n';
 * void tx(uint8_t ip_byte)
 * void tx_str(uint8_t *ip_array,uint8_t size)
 * void tx_str_ROM(const uint8_t *ip_array,uint8_t size)
 * void tx_string(uint8_t *ip_array) // slow, use tx_str if length is known
 * void tx_string_ROM(const uint8_t *ip_array) // slow, use tx_str_ROM if length is known
 * void tx_int(uint32_t int_ip)
 
 * void timer1_delay()
 
 * void pwm_fast(uint8_t duty,uint8_t clock_scaler)
 * void pwm_phased(uint8_t length,uint8_t clock_scaler)
 
//------------------------peripheral-------------------------
    
 *uint8_t raw_keypad()
 *uint8_t get_keystroke(uint8_t display_key)
 *uint32_t get_pin(uint8_t len_pin) // max temp length is 4 

 *uint8_t EEPROM_write(uint16_t address,uint8_t data)
 *uint8_t EEPROM_read(uint16_t address)

// TWI (start,stop,write,read_ack,read_nack,status)
 * uint8_t ssd1306_single_cmd(uint8_t data)
 * uint8_t ssd1306_cmd(uint8_t *ptr_data,uint8_t size)
 * uint8_t ssd1306_data(uint8_t *ptr_data,uint8_t size)
 * uint8_t ssd1306_data_ROM(const uint8_t *ptr_data,uint8_t size)


*/
//__________________________________Main_________________________________________________
int16_t main()
{	
//    ---------------------------------------------------------------------    
    const uint8_t intro[]="| RTOS UART Debug en-tier-face |\n";
//    ---------------------------------------------------------------------
    millis_setup();
    blink_setup();
    blink();
    USART_setup();
//    keypad_setup();

//    twi_setup();
//    ssd1306_setup();
//    spi_init();
//    ssd1306_spi_setup();
//    ---------------------------------------------------------------------
    tx_string_ROM(&intro[0]); 
	while (1)
    {
//        asm volatile("nop");
        switch(rx_get())
        {
        case '1':blink();break;
        case '2':blink();blink();break;
        case 13:tx('.');break;
        }  
        _delay_ms(100);
    } // while 1 end 
    
} // main end

//_________________________________End__________________________________________________

