// #include <stdio.h> // printf , delete later
#include<unistd.h> // file descipters - get 5 I/O system calls + its marcos
#include<fcntl.h> 
#include<time.h> // clock_t
#include<stdlib.h>// rand()
#include<pthread.h> // run parallel threads
#include<signal.h> // to catch SIGINT 


#define height 40 // dimensions for the box, as full screen gets to big.
#define width 150
#define start_point write(1,"\e[10;40H", 8); //start point for the box (40,10)

struct input_keyboard {  // to catch key strokes from keyboard file system
   __uint64_t  sec;
   __uint64_t  usec;
   __uint16_t  type;
   __uint16_t  code;
   __uint32_t  value;
};

__uint16_t button=106; // store final key ID ( will be updated by get_keyboard() )

// _____________________________________Sustitute functions_______________________________________________________________

// long int (unsigned 64 bits) is the pointer storage size
#define __sizeof(var) (char *)(&var+1)-(char*)(&var)  // get size of variable (&var+1) catchs the whole variable 

__uint8_t __strlen(__uint8_t *str) // get string length upto 255
{
  __uint8_t strlen=0;
  while(*str!='\0'){str+=1;strlen+=1;}
  return strlen;
}

void __strcat(__uint8_t *str1,__uint8_t *str2)  // append the second string on the end of first string, the size of first string should have enough length (check what happends when its not) 
{
	while(*str1!='\0'){str1++;}
	while(*str2!='\0')
	{
		*str1=*str2;
		++str1;
		++str2;
	}
	*str1='\0';
}

void charcat(__uint8_t *str, __uint8_t char1) // append char at the end of the string
{
 while(*str!='\0'){str+=1;}
 *str=char1;
 str+=1;
 *str='\0';
} 

void int2str(__uint8_t *str,__uint32_t int1) // append the number at the end of the string
{
	while(*str!='\0'){str+=1;}
	__uint8_t temp_digit[10]={0};
	__int8_t i=0;
	while(int1!=0)
	{
		temp_digit[i]=(int1%10)+48;
		int1/=10;
		i+=1;
	}
	i-=1;
	for(i;i>=0;i-=1,str+=1)
	{
		*str=temp_digit[i];
	}	
	*str='\0';
}

void __delay_ms(__uint32_t ms) // delay in milliseconds
{
	clock_t start_time=clock();
	while(clock()<start_time+ms*1000){}
}

void SIGINT_handle()
{
	
	write(1,"\e[?25h",6); // enable cursor blink
	system("stty echo"); // enable echo back 
	exit(0); // success exit from program
}

void *get_keyboard() // read the input file for keyboard and filter to update value of button
{
	struct input_keyboard key;
	__uint32_t keyboard_file =open("/dev/input/event3",O_RDONLY);
	while(1)
	{
		read(keyboard_file,&key,__sizeof(key));	 //type 1 is for state change of keyboard -----  value  0-released,1-pressed,2-held
		if(key.type==1)
		{
			if(key.code==103||key.code==105||key.code==106||key.code==108)
				{button=key.code;}
		}

	}
}

// ____________________________________________________________________________________________________

void point_cursor(__uint32_t X,__uint32_t Y) // chagne the position of the cursor to X,Y
{
	__uint8_t cursor[10]="\e[";
	__uint8_t *ptr_cursor=&cursor[0];
	int2str(cursor,Y);
	charcat(ptr_cursor,';');
	int2str(cursor,X);
	charcat(ptr_cursor,'H');
	write(1,ptr_cursor, __strlen(ptr_cursor));
}	

// colors for box and snake
// Black:   30
// Red:     31
// Green:   32
// Yellow:  33
// Blue:    34
// Magenta: 35
// Cyan:    36
// White:   37

void draw_box(__uint8_t color)   // make the outline of the game 
{
	char head_msg[20]="Snake Game";
	write(1,"\e[2J", 4);
	write(1,"\e[9;107H", 8);
	write(1,"\e[33m",5); //yellow
	write(1,head_msg,sizeof(head_msg));

	__uint8_t string_color[5]="\e[";
	int2str(&string_color[0],color);
	charcat(&string_color[0],'m');
	write(1,&string_color[0],5); 
	
	start_point
	for (int x=0;x<height;x++)
	{
		write(1,"\e[1B\e[1D", 8);
		write(1,"|", 1);
	}
	for (int x=0;x<width;x++)
	{
		write(1,"_", 1);
	}
	start_point
	for (int x=0;x<width;x++)
	{
		write(1,"_", 1);
	}
	for (int x=0;x<height;x++)
	{
		write(1,"\e[1B\e[1D", 8);
		write(1,"|", 1);
	}
}

void draw_snake(__uint32_t *ptr_snake,__uint8_t color)  // draw the whole snake 
{
	__uint8_t string_color[5]="\e[";
	int2str(&string_color[0],color);
	charcat(&string_color[0],'m');
	write(1,&string_color[0],5); 

	while(*ptr_snake!='\0')
	{
		point_cursor(*ptr_snake,*(ptr_snake+1));
		write(1,"\u2720", 4);
		ptr_snake+=2;
	}
}


void cook_food(__uint32_t *ptr_food,__uint32_t *ptr_snake) // get location of new food and display it,called when the snake eats the existing food
{

*ptr_food=0; // delete old food
__uint32_t *saved_ptr_snake=ptr_snake; //save the snake pointer

while(*ptr_food==0)
{
*ptr_food=rand()%147; // random number (40,188)
*ptr_food+=41;

*(ptr_food+1)=rand()%39; // random number (10,50)
*(ptr_food+1)+=11;

ptr_snake=saved_ptr_snake;

while(*ptr_snake!='\0')
{
	if(*ptr_snake==*ptr_food && *(ptr_snake+1)==*(ptr_food+1)){*ptr_food=0;} // food should not be on the snake
	ptr_snake+=2;
}
}

point_cursor(*ptr_food,*(ptr_food+1)); // draw the new food at its location
write(1,"\e[32m",5); //green
write(1,"\u2622",4); // radioactive symbol
// check from where rand comes and compare with libsodium
}

__uint8_t len_2D_array(__uint32_t *array_head,__uint8_t inner_size)  // get length of 2D array, if length of sub array is fixed and known
{
	__uint8_t len=0,cmp_len=0;
	while(*array_head!='\0'){len+=1;array_head+=1;}
	cmp_len=len;
	len/=inner_size;
	if(cmp_len==(len*inner_size)){return len;}
	else {return 0;}	
}

void shift_snake(__uint32_t *ptr_snake,__uint8_t snake_length) // right shift the variable holding snake locations
{
	__uint32_t *snake_head=ptr_snake;
	ptr_snake+=((snake_length-1)*2)+1;
	while(ptr_snake>=snake_head)
	{
		*(ptr_snake+2)=*ptr_snake;
		ptr_snake-=1;
	}
}

// ____________________________________________________________________________________________________

int main()
{
	pthread_t thread_id;
	pthread_create(&thread_id,NULL,get_keyboard,NULL); // initilize the get_keyboard thread
	
	write(1,"\e[?25l",6); // cursor will be invisible
	system("stty -echo"); // echo off - inputs will be displayed on the screen

	signal(SIGINT,SIGINT_handle);  // transfer SIGINT (CTRL+C) to the fucntion

	srand ( time(NULL) ); // provide seed to rand

	__uint32_t food[2]={0,0}; //  food 

	__uint32_t snake_starting_x=(width+40)/2; // initial location of snake 
	__uint32_t snake_starting_y=(height+10)/2;

	__uint32_t snake[30][2]={  // initial size of snake
		{snake_starting_x,snake_starting_y}
		,{snake_starting_x-1,snake_starting_y}
		,{snake_starting_x-2,snake_starting_y}
		,{snake_starting_x-3,snake_starting_y}
		};

	__uint32_t snake_head[2]={0,0}; // head tail need to be tracked to control snake 
	__uint32_t snake_tail[2]={0,0};
	__uint8_t snake_length=len_2D_array(&snake[0][0],2);


	__uint8_t snake_movement=3; //keep track of snake movement - 0,1,2,3,4 stop,up,down,right,left

	draw_snake(&snake[0][0],35); // draw the complete initial snake , later only head and tail will be redrawn
	draw_box(37);
	cook_food(&food[0],&snake[0][0]);

	while (1)
	{
	

	switch(button) // get new head and move it according to last movement and user input
	{
		case 103:  //up
		if(snake_movement!=2){snake_head[0]=snake[0][0];snake_head[1]=snake[0][1]-1;snake_movement=1;} // cannot take 180 degree turn
		else{snake_head[0]=snake[0][0];snake_head[1]=snake[0][1]+1;}
		break;
		case 108:  //down
		if(snake_movement!=1){snake_head[0]=snake[0][0];snake_head[1]=snake[0][1]+1;snake_movement=2;}
		else{snake_head[0]=snake[0][0];snake_head[1]=snake[0][1]-1;}
		break;
		case 106:  //right
		if(snake_movement!=4){snake_head[0]=snake[0][0]+1;snake_head[1]=snake[0][1];snake_movement=3;}
		else{snake_head[0]=snake[0][0]-1;snake_head[1]=snake[0][1];}
		break;
		case 105:  //left
		if(snake_movement!=3){snake_head[0]=snake[0][0]-1;snake_head[1]=snake[0][1];snake_movement=4;}
		else{snake_head[0]=snake[0][0]+1;snake_head[1]=snake[0][1];}
		break;
	}

	point_cursor(snake_head[0],snake_head[1]); // draw new head
	write(1,"\e[35m",5); //magenta
	write(1,"\u2720", 4);
	
	snake_length=len_2D_array(&snake[0][0],2); // get snake length
	snake_tail[0]=snake[snake_length-1][0]; snake_tail[1]=snake[snake_length-1][1]; //get tail


	shift_snake(&snake[0][0],snake_length); // left shift the snake array
	snake[0][0]=snake_head[0];snake[0][1]=snake_head[1]; //assign the new head to the snake

	if(snake_head[0]==food[0] && snake_head[1]==food[1]) // snake ate the food
	{	
		// do not remove the tail
		cook_food(&food[0],&snake[0][0]);
	}
	else // snake did'nt eat the food
	{
		point_cursor(snake_tail[0],snake_tail[1]); // remove old tail
		write(1," ", 1);
		snake[snake_length][0]='\0';snake[snake_length][1]='\0';
	}

	//Box bounds X(40-190)   Y(10-50) 
	if(snake_head[0]==40 || snake_head[0]==189 ||snake_head[1]==10 || snake_head[1]==50)
	{
		// snake went out of the box.
		draw_box(31);
		draw_snake(&snake[0][0],35); 
		sleep(1.5);
		write(1,"\e[?25h",6); // enable cursor blink
		system("stty echo"); // enable echo back 
		exit(0); // success exit from program
	}

	__uint32_t *ptr_snake=&snake[0][0];
	ptr_snake+=2;
	while(*ptr_snake!='\0')
	{
		if(snake_head[0]==*ptr_snake && snake_head[1]==*(ptr_snake+1))
		{
			// snake crashed in itself
			draw_snake(&snake[0][0],31); 
			sleep(1.5);
			write(1,"\e[?25h",6); // enable cursor blink
			system("stty echo"); // enable echo back 
			exit(0); // success exit from program
		}
		ptr_snake+=2;
	}

	if(snake_length==20)
	{
		// you have won the game
		draw_box(32);
		draw_snake(&snake[0][0],32);
		point_cursor(115,30);
		write(1,"WINNER WINNER GHEEYA DINNER!!",29); 
		sleep(2);
		write(1,"\e[?25h",6); // enable cursor blink
		system("stty echo"); // enable echo back 
		exit(0); // success exit from program
	}


	__delay_ms(50); // loop time, also determines the snake speed. 
	}

	return 0;
}

