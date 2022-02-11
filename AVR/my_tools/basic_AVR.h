#ifndef basic_H 
#define	basic_H

// #include <xc.h> // include processor files - each processor file is guarded.
#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000
#include <util/delay.h>


//------------------------------------------------------------------- 

uint8_t power(uint8_t _base,uint8_t _power);

//-------------------------------------------------------------------

#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1
#define FIFO_BUFFER_SIZE 100 

void USART_setup();
#define new_line while(!(UCSR0A & (1<<UDRE0))){};UDR0='\n';
void tx(uint8_t ip_byte);
void tx_str(uint8_t *ip_array,uint8_t size);
void tx_str_ROM(const uint8_t *ip_array,uint8_t size);
void tx_string(uint8_t *ip_array) ;// slow, use tx_str if length is known
void tx_string_ROM(const uint8_t *ip_array); // slow, use tx_str_ROM if length is known
void tx_int(uint16_t int_ip);

extern volatile uint8_t rx_buffer_empty;
extern volatile uint8_t rx_buffer_full;  //overflow and full will be handled similarly.

typedef struct{
	uint8_t data[FIFO_BUFFER_SIZE];
	uint8_t read_from;
	uint8_t write_at;
	uint8_t active_size;
}FIFO_typedef;

FIFO_typedef rx_buffer;


void rx_put();
uint8_t rx_get();




//-------------------------------------------------------------------
 
void millis_setup();
extern volatile uint32_t t_ms;
void __delay_ms(uint16_t ms);
void pwm_fast(uint8_t duty,uint8_t clock_scaler);
void pwm_phased(uint8_t length,uint8_t clock_scaler);
void timer1_delay();

//-------------------------------------------------------------------
#define twi_status_start_tx 0x08   
#define twi_status_start_tx_again 0x18
#define twi_status_SLA_W_tx_nack 0x20
#define twi_status_data_ack 0x28
#define twi_status_data_nack 0x30
#define twi_status_arbitration_lost 0x38 

void twi_setup();
void twi_start();
void twi_stop();
void twi_write(uint8_t data);
uint8_t twi_read_ack();
uint8_t twi_read_nack();
uint8_t twi_stats();
//-------------------------------------------------------------------


uint8_t EEPROM_write(uint16_t address,uint8_t data);
uint8_t EEPROM_read(uint16_t address);

void EEPROM_display();  // slow and inefficient - for debugging only


//-------------------------------------------------------------------

#define SPI_DDR DDRB
#define SS      PINB2
#define MOSI    PINB3
#define MISO    PINB4
#define SCK     PINB5

void spi_init();

void spi_single_tx(uint8_t data);
uint8_t spi_single_rx(uint8_t data); // This data will be Transmitted

void spi_tx(uint8_t *data,uint8_t size);
void spi_rx(uint8_t *data,uint8_t size); // This data will be Transmitted

//-------------------------------------------------------------------

void boot_basic_setup();
void boot_UART_setup();
uint8_t boot_catch_handle();

//-------------------------------------------------------------------
//-------------------------------------------------------------------


#endif


