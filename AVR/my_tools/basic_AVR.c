#include"basic_AVR.h"


/*
 * register - will be heavily used
 * static - static storage duration
 * const - cannot be modified through code
 * volatile - compiler cannot perform optimization or reorder access
 
------------------------------------------------------------------------------------------------
It is more efficient to count the loop from the maximum value to zero (decrement). 
------------------------------------------------------------------------------------------------
Several loops implemented one by one lead to a long list of iterations.loop jamming may help to increase the code.use do-while if possible
------------------------------------------------------------------------------------------------
“const” is used to tell the compiler that the data is to be "read-only" and increases opportunities for optimization. It does not identify where the data should be stored.  

uint8_t string[12] PROGMEM = {"hello world!"};  
pgm_read_byte(&string[10]);

There are more macros and functions in the <avr/pgmspace.h> system header file for
storing and retrieving different types of data to/from program space
------------------------------------------------------------------------------------------------
For global data, use the static keyword whenever possible. 
A static function is easier to optimize, because its name is invisible outside of the file in which it is declared and it will not be called from any other files. 
If the function is called multiple times, it will not be optimized to an inline function.(inline functions are sustituted in the code space and not called)
------------------------------------------------------------------------------------------------
Well coded assembly instructions are always the best optimized code,use assembly macros. “Inline Assembler Cookbook” section in avr-libc user manual.
------------------------------------------------------------------------------------------------
we can see that faster code is generated with a predecrement conditional statement.
------------------------------------------------------------------------------------------------
unrolling loops (tradeoff between speed and size optimization). If -O3 option is enabled, the compiler will unroll the loop automatically 
------------------------------------------------------------------------------------------------
“if-else”, always put the most probable conditions in the first place.
Using “switch-case” may eliminate the drawbacks of “if-else”, because for a “switchcase”, the compiler usually generates lookup tables with index and jump to the correct place directly.
Brach if-else to reduce the execution time for a worst case condition.(this will increase the code size) 
------------------------------------------------------------------------------------------------
Source - https://ww1.microchip.com/downloads/en/AppNotes/doc8453.pdf
------------------------------------------------------------------------------------------------
*/

/*

If possible, put your constant tables in the lower 64 KB and use pgm_read_byte_near() or pgm_read_word_near()
instead of pgm_read_byte_far() or pgm_read_word_far() since it is more efficient that way, and you can still use the upper 64K for executable code. 


*/

// ______________________________________________________________________________________GENERIC______________________________________________________________________________________________________

uint8_t power(uint8_t _base,uint8_t _power)
{
	uint8_t result=_base;
	if(_power==0)
	{
		return 1;
	}
	else
	{
	do
		{
			result*=_base;
			--_power;
		} while (_power!=0);	
	return result;
	}
}



// ______________________________________________________________________________________SPECIFIC______________________________________________________________________________________________________

//---------------------USART----------------------------------



void USART_setup()
{
	//PRR&=11111101; //PRUSART0 = 0 to turn off power reduction  bit of USART0 which turns off its clock.
    sei();
	UBRR0=MYUBRR;
	UCSR0B=(1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);
	UCSR0C=0b00000110;
}

// #define new_line while(!(UCSR0A & (1<<UDRE0))){};UDR0='\n';

inline void tx(uint8_t ip_byte)
{
	while(!(UCSR0A & (1<<UDRE0))){}
	UDR0=ip_byte;
}

#define new_line while(!(UCSR0A & (1<<UDRE0))){};UDR0='\n';

void tx_str(uint8_t *ip_array,uint8_t size)
{
	do
	{
		tx(*ip_array);//inline
		++ip_array;
		--size;
	}
	while(size!=0);
}

void tx_str_ROM(const uint8_t *ip_array,uint8_t size)
{
	do
	{
		tx(*ip_array);//inline
		++ip_array;
		--size;
	}
	while(size!=0);
}


void tx_string(uint8_t *ip_array) // slow, use tx_str if length is known
{
	do
	{
		tx(*ip_array);//inline
		++ip_array;
	}
	while(*ip_array!='\0');
}

void tx_string_ROM(const uint8_t *ip_array) // slow, use tx_str_ROM if length is known
{
	do
	{
		tx(*ip_array);//inline
		++ip_array;
	}
	while(*ip_array!='\0');
}

void tx_int(uint16_t int_ip)
{	
	if(int_ip==0)
	{
	tx('0'); // will be replaced as tx is inline
	}
	else
	{
	uint8_t temp_digit[10],total_digits=0;
    uint8_t *ptr_temp_digit=&temp_digit[0];
	
    do
	{
	*ptr_temp_digit=(int_ip%10)+48;
	int_ip=int_ip/10;
    ++ptr_temp_digit;
    ++total_digits;
	}while(int_ip!=0);
	do
	{
		--ptr_temp_digit;
		tx(*ptr_temp_digit);
		--total_digits;
	}
	while(total_digits!=0);
	}
}


// As read , write and size at 8 bit wide this buffer can be maximun 255 length.


volatile uint8_t rx_buffer_empty=1;
volatile uint8_t rx_buffer_full=0;  //overflow and full will be handled similarly.

FIFO_typedef rx_buffer={{},0,0,0};

void rx_put()
{
if(rx_buffer_full==0)
{
	rx_buffer_empty=0;
	rx_buffer.data[rx_buffer.write_at]=UDR0;
	++rx_buffer.write_at;
	++rx_buffer.active_size;
	if(rx_buffer.active_size==FIFO_BUFFER_SIZE){rx_buffer_full=1;}
	if(rx_buffer.write_at==FIFO_BUFFER_SIZE){rx_buffer.write_at=0;}
}
}

uint8_t rx_get()
{	
	if(rx_buffer_empty==0)
	{
		rx_buffer_full=0;
		// ++rx_buffer.read_from;
		--rx_buffer.active_size;
		if(rx_buffer.active_size==0){rx_buffer_empty=1;}
		if(rx_buffer.read_from==FIFO_BUFFER_SIZE){rx_buffer.read_from=0;}
		return(rx_buffer.data[rx_buffer.read_from++]);
	}
	else{return 0;}
}



// ------------------------------------TIMER---------------------------------------------------
void millis_setup()
{
	//setup timer2 to over flow every 1ms.
    OCR2A=249;
    TCCR2A=0b00000001;
    TCCR2B=0b00000100; // 16Mhz/64= 250Khz  
    TIMSK2=(1<<OCIE2A);
    SREG|=0b10000000; // sei()   
}

volatile uint32_t t_ms=0;

void __delay_ms(uint16_t ms)
{
uint32_t current_time=t_ms;
while(t_ms-current_time<ms){}
}

void timer1_delay()
{
    TCNT1=49911; //-15625
//  OCR1A=0x3D09;//15625 for 1 second
    TCCR1A=0b00000000; //Normal mode 
    TCCR1B=0b00000101; //1024 pre scalar (64us at 16Mhz)
    TCCR1C=0x00;
//    TIMSK1=(1<<TOIE1);
//    SREG|=0b10000000; // sei()
////    TIFR1  // OCF1A 
    while((TIFR1&(1<<TOV1))==0){}
    TIFR1=(1<<TOV1);
    TCCR1B=0x00; //stop clock 
}


//--------------------------------------PWM----------------------------------------------------
// PWM generation using T0 
// Fast and phase correct both for OCA0 -> PIN 12(Uno pin 6)
// T0 will be occupied, OCB0 -> Pin 11(Uno pin 5) can also be used. 
void pwm_fast(uint8_t duty,uint8_t clock_scaler) 
{
    //duty is 0-255
    //Clock scaler(N) is 0-OFF/1-ON/2-8/3-63/4-256/5-1024/6-T0 falling/7-T0 rising
    OCR0A=duty;
    TCCR0A=0b10000011;//toggle OC0A
    TCCR0B=(clock_scaler<<0x00);    
}

void pwm_phased(uint8_t length,uint8_t clock_scaler)
{
 OCR0A=length;
 TCCR0A=0b10000001;
TCCR0B=(clock_scaler<<0x00); 
}


// ____________________________________________________________________________________________________________________________________________________________________________________________
// _____________________________________________________________________________________I2C(TWI-two wire interface)_______________________________________________________________________________________________________

void twi_setup()
{
    sei();
    TWSR=0x00;	//status has prescaler
    TWBR=0x0C;	//bitrate + prescaler decide the speed (400Khz) 
    TWCR=(1<<TWIE);	//control
}
inline void twi_start()
{
    TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN); //start condition
    while((TWCR&(1<<TWINT))==0){}
}
inline void twi_stop()
{
    TWCR=(1<<TWINT)|(1<<TWSTO)|(1<<TWEN); //stop condition
}
inline void twi_write(uint8_t data)
{
    TWDR=data;  //data
    TWCR=(1<<TWINT)|(1<<TWEN);
    while((TWCR&(1<<TWINT))==0){}
}

inline uint8_t twi_read_ack()
{
    TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWEA);
    while((TWCR&(1<<TWINT))==0){}
    return TWDR;
}

inline uint8_t twi_read_nack()
{
    TWCR=(1<<TWINT)|(1<<TWEN);
    while((TWCR&(1<<TWINT))==0){}
    return TWDR;
}


// status codes 
// 0x08 - start tx'ed
// 0x18 - repeated start tx'ed
// 0x20 SLA+W tx'ed, not ack received
// 0x28 data byte tx'ed, ack received
// 0x30 data byte tx'ed, not ack received
// 0x38 arbitration lost in SLA+W or data bytes




inline uint8_t twi_stats()
{
    uint8_t temp_status=TWSR&0xF8; // mask lower 3 bits
    return temp_status;

}
//	TWAR=	//address needed when used as slave 

// ______________________________________________________________________________________________EEPROM______________________________________________________________________________________________

uint8_t EEPROM_write(uint16_t address,uint8_t data)
{
    while((EECR&=0x01)==0x01){}
    EEDR=data;
    EEAR=address;
    EECR=0b00000100;
    EECR|=0b000000010;
    return 1;
}
uint8_t EEPROM_read(uint16_t address)
{
    while((EECR&=0x01)==0x01){}
    EEAR=address;
    EECR=0b00000001;
    timer1_delay();
    return EEDR;
}

void EEPROM_display()  // slow and inefficient - for debugging only
{
    const uint8_t temp_message[]="\n______________EEPROM DUMP______________\n";
    tx_string_ROM(&temp_message[0]);
    uint16_t temp_address=1023;
    do
    {
       if(temp_address%16==0){new_line}
	   while((EECR&=0x01)==0x01){}
	   EEAR=temp_address;
	   EECR=0b00000001;
	   _delay_ms(1);
	   tx_int(temp_address);tx('-');tx_int(EEDR);
	   tx(' ');tx('|');tx(' ');
	   --temp_address;
    }
    while(temp_address!=0);
    new_line
    
}

//________________________________________________________________________SPI________________________________________________________

#define SPI_DDR DDRB
#define SS      PINB2
#define MOSI    PINB3
#define MISO    PINB4
#define SCK     PINB5

void spi_init()
{
DDRB|=(1<<SS)|(1<<MOSI)|(1<<SCK);//|(1<<DORD);	
// SPCR=(1<<SPE)|(1<<MSTR)|(1<<SPR1)|(1<<SPR0);
// SPCR=(1<<SPE)|(1<<MSTR);
// SPCR=(1<<SPE)|(1<<MSTR)|(1<<SPR0);
SPCR=(1<<SPE)|(1<<MSTR)|(1<<SPR1);

// SPCR=(1<<SPE)|(1<<MSTR)|(1<<SPR1)|(1<<SPR0)|(1<<CPOL);
// SPCR=(1<<SPE)|(1<<MSTR)|(1<<SPR1)|(1<<SPR0)|(1<<CPOL)|(1<<CPHA);

}

inline void spi_single_tx(uint8_t data)
{
	SPI_DDR &= ~(1 << SS);
	SPDR=data;
	tx('s');  
	while(!(SPSR&&(1<<SPIF)));
	tx('e');
	SPI_DDR |= (1 << SS);
}

inline uint8_t spi_single_rx(uint8_t data) // This data will be Transmitted
{
	SPI_DDR &= ~(1 << SS);
	SPDR=data;
	while(!(SPSR&&(1<<SPIF)));
	SPI_DDR |= (1 << SS);
	return SPDR;
}

void spi_tx(uint8_t *data,uint8_t size) // check this 
{
	SPI_DDR &= ~(1 << SS);
	do
	{
	SPDR=*data;
    ++data;
    tx('s'); 
	while(!(SPSR&&(1<<SPIF)));
	--size;
	tx('e');
	}
	while(size!=0);
	SPI_DDR |= (1 << SS);
}

void spi_rx(uint8_t *data,uint8_t size) // This data will be Transmitted
{
	SPI_DDR &= ~(1 << SS);
	do
	{
	SPDR=*data; 
	while(!(SPSR&&(1<<SPIF)));
	*data=SPDR;
    ++data;
	--size;
	}
	while(size!=0);
	SPI_DDR |= (1 << SS);
}