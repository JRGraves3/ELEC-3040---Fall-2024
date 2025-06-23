/*========================*/
/* Joseph Graves */
/* ELEC 3050 Lab 4 */
/*========================*/

#include "stm32l4xx.h" /* Microcontroller information */

/* Global variable */
static int value1, value2; /* counter values */
static unsigned char run = 1; // Controls if the counters are counting
static unsigned char up = 0; // Controls which direction counter A is counting
static unsigned char PB3 = 0;
static unsigned char PB4 = 0;

/*----------------------------------------------------------*/
/*                    Pin Setup Function                    */
/*----------------------------------------------------------*/
void PinSetup () {
	
	
	//GPIOA->MODER &= ~(0x0000003C); /* General purpose input mode */
	
	/* Configure PA[12:5] as output pins */
	/* PA[8:5] is Counter A, PA[12:9] is Counter B */
	RCC->AHB2ENR |= 0x01; /* Enable GPIOA clock (bit 0) */
	GPIOA->MODER &= ~(0x03FFFC3C); /* Clear PA[12:5] mode bits */
	GPIOA->MODER |= (0x01555400); /* General purpose output mode for PA[12:5] */
	
	/* Configure PB4, PB3 as output pins to drive LEDs */
	RCC->AHB2ENR |= 0x02; /* Enable GPIOB clock (bit 1) */
	GPIOB->MODER &= ~(0x000003C0); /* Clear PB4-PB3 mode bits */
	GPIOB->MODER |= (0x00000140); /* General purpose output mode */
}

void interruptSetupPA1() {
	/* Configure PA1 as interrupt source */
	RCC->AHB2ENR |= 0x01; /* Enable GPIOA clock (bit 0) */
	
	SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI1; // Clear EXTI1 bit
	//SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI1_PA; // Set PA1 as interrupt source
	
	EXTI->RTSR1 |= EXTI_FTSR1_FT1; // falling edge triggered EXTI1
	EXTI->PR1 |= EXTI_PR1_PIF1; // clear EXTI1
	EXTI->IMR1 |= EXTI_IMR1_IM1; // enable EXTI1
	
	NVIC_ClearPendingIRQ(EXTI1_IRQn); // Clear NVIC bits
	NVIC_EnableIRQ(EXTI1_IRQn); // Enable IRQ
	
	SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI2; // clear EXTI2 bit
	//SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI2_PA; // set PA2 as interrupt source
	
	EXTI->RTSR1 |= EXTI_FTSR1_FT2; // falling edge triggered EXTI2
	EXTI->PR1 |= EXTI_PR1_PIF2; //clear EXTI2
	EXTI->IMR1 |= EXTI_IMR1_IM2; // Enable EXTI2
	
	NVIC_ClearPendingIRQ(EXTI2_IRQn); // clear NVIC bits
	NVIC_EnableIRQ(EXTI2_IRQn); // Enable IRQ
	
	__enable_irq();
}


/*void interruptSetupPA2() {
	// Configure PA2 as interrupt source 
	RCC->AHB2ENR |= 0x01; // Enable GPIOA clock (bit 0) 
	
	SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI2; // clear EXTI2 bit
	//SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI2_PA; // set PA2 as interrupt source
	
	EXTI->RTSR1 |= EXTI_FTSR1_FT2; // falling edge triggered EXTI2
	EXTI->PR1 = EXTI_PR1_PIF2; //clear EXTI2
	EXTI->IMR1 |= EXTI_IMR1_IM2; // Enable EXTI2
	
	NVIC_ClearPendingIRQ(EXTI2_IRQn); // clear NVIC bits
	NVIC_EnableIRQ(EXTI2_IRQn); // Enable IRQ
} */

/*----------------------------------------------------------*/
/*     Delay function - do nothing for about 0.5 seconds    */
/*----------------------------------------------------------*/
void delay () {
	int volatile i,j,n;
	for (i=0; i<20; i++) { //outer loop
		for (j=0; j<5750; j++) { //inner loop
			n = j; //dummy operation for single-step test
		} //do nothing
	}
}


/*----------------------------------------------------------*/
/*    Count function - increment/decrement the counters     */
/*----------------------------------------------------------*/
void count() {
	if(run != 0) {
		if(up==1){
			value1++;
			if(value1==10){
				value1=0;
			}
		}else{
			value1--;
			if(value1==-1){
				value1=9;
			}
		}
		value2++;
		if(value2==10){
				value2=0;
		}
	}
	GPIOA->ODR = (value1 << 5) + (value2 << 9); //Write value1 to PA[8:5] and value2 to PA[12:9]
}


/*----------------------------------------------------------*/
/*   Interrupt Service Routine 1 - Toggles "run" (EXTI1)    */
/*----------------------------------------------------------*/
void EXTI1_IRQHandler() {
	//__disable_irq();
	
	if(PB3 == 1) {
		PB3 = 0;
	} else {
		PB3 = 1;
	}
	
	if(up == 1) {
		up = 0;
	} else {
		up = 1;
	}
	EXTI->PR1 = EXTI_PR1_PIF1; // clear EXTI1 bit
	NVIC_ClearPendingIRQ(EXTI1_IRQn); // clear NVIC bits
	//__enable_irq();
}


/*----------------------------------------------------------*/
/*    Interrupt Service Routine 2 - Toggles "up" (EXTI2)    */
/*----------------------------------------------------------*/
void EXTI2_IRQHandler() {
	//__disable_irq();
	
	if(PB4 == 1) {
		PB4 = 0;
	} else {
		PB4 = 1;
	}
	
	if(run == 1) {
		run = 0;
	} else {
		run = 1;
	}
	EXTI->PR1 = EXTI_PR1_PIF2; // clear EXTI2 bit
	NVIC_ClearPendingIRQ(EXTI2_IRQn); // clear NVIC bits
	//__enable_irq();
}



/*----------------------------------------------------------*/
/*                       Main Method                        */
/*----------------------------------------------------------*/
int main(void) {
	PinSetup(); // Configure GPIO Pins
	interruptSetupPA1(); // Configure Interrupt PA1
	//interruptSetupPA2(); // Configure Interrupt PA2
	//__enable_irq();
	/* Endless Loop */
	while(1) {
		delay();
		
		count();
		GPIOB->ODR = (PB3<<3) + (PB4<<4);
	}
}
