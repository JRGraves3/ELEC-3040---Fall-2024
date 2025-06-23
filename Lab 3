/*========================*/
/* Joseph Graves */
/* ELEC 3050 Lab 3 */
/* 3-bit counter */
/*========================*/

#include "stm32l4xx.h" /* Microcontroller information */

/* Global variable */
int value; /* counter value */
int go; /* depends on the given secret code */
//int code; /* input that is checked against the secret code */

void PinSetup () {
	/* Configure PA7-PA0 as input pins */
	RCC->AHB2ENR |= 0x01; /* Enable GPIOA clock (bit 0) */
	GPIOA->MODER &= ~(0x0000FFFF); /* General purpose input mode */
	
	/* Configure PB[5:3] as output pins */
	RCC->AHB2ENR |= 0x02; /* Enable GPIOB clock (bit 1) */
	GPIOB->MODER &= ~(0x00000FC0); /* Clear PB5-PB3 mode bits */
	GPIOB->MODER |=(0x00000540); /* General purpose output mode */
}

/*----------------------------------------------------------*/
/* Delay function - do nothing for about 1 second */
/*----------------------------------------------------------*/
void delay () {
	int volatile i,j,n;
	for (i=0; i<20; i++) { //outer loop
		for (j=0; j<20000; j++) { //inner loop
			n = j; //dummy operation for single-step test
		} //do nothing
	}
}

/*----------------------------------------------------------*/
/* Count function - increment/decrement the counters */
/*----------------------------------------------------------*/
void count() {
	value++;
	if(value==5){
		value=0;
	}
	GPIOB->ODR = (value << 3); //Write value to PB[5:3]
}

/*----------------------------------------------------------*/
/* Secret function - checks the code given on PA[7:0] */
/*----------------------------------------------------------*/
int secret() {
	uint8_t code;
	int right;
	code = GPIOA->IDR;
	code &= 0xFF;
	if (code == 0x73) {
		right = 1;
	} else {
		right = 0;
	}
	return right;
}

/*----------------------------------------------------------*/
/* Main Method */
/*----------------------------------------------------------*/
int main(void) {
	PinSetup(); // Configure GPIO pins
	
	/* Endless loop */
	while (1) {
		delay();
		go = secret();
		if (go == 1) {
			count();
		}
	}
}
