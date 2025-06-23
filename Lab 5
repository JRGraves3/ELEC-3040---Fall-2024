// My third try at this lab


#include "stm32l4xx.h"
#include <stdbool.h>
#include <math.h>

void PinSetup(void);
void InterruptSetup(void);
void debounce(void);
void display(void);
void count(void);
void delay(void);
uint8_t Read_Keypad(void);
void EXTI0_IRQHandler(void);
void Display4BitValue(uint8_t value);
void DisplayCounter(void);
 
// Global Variables
// Keypad constants
#define NUM_ROWS 4
#define NUM_COLS 4
static uint32_t rowPins[NUM_ROWS] = {0x04, 0x08, 0x10, 0x20}; //PA[2-5]
static uint32_t colPins[NUM_COLS] = {0x100, 0x200, 0x400, 0x800}; // PA[8-11]
// uint32_t adc_in;
// static volatile uint8_t key = 0x9;

// static int RESET_COUNT = 0;
// static int updown = 0;

static volatile uint8_t keyPressed = 0xFF;
static volatile uint8_t counter = 0;

// static unsigned char show = 0; // 0 for keyVal, 1 for counter
// static unsigned char keyVal = 0;
// static unsigned char counter = 0;
static int row;
static int col;
// static unsigned int rowNum;
// static unsigned int colNum;
// static unsigned int run = 1;
static unsigned int num;
unsigned int i, j, k, n;
/*(static unsigned int key_map[4][4] = {
	{0x01,0x02,0x03,0x0A},
	{0x04,0x05,0x06,0x0B},
	{0x07,0x08,0x09,0x0C},
	{0x0E,0x00,0x0F,0x0D},
};*/


// Pin setup
void PinSetup() {
	RCC->AHB2ENR |= 0x01; // enable gpioa clock
	GPIOA->MODER &= ~(0x00FF0FF0);
	GPIOA->MODER |= 0x00000550;
	// xxxx xxxx xxxx xxxx xxxx xxxx xxxx xxxx
	// xxxx xxxx 0101 0101 xxxx 0000 0000 xxxx
	GPIOA->OTYPER &= ~(0xF0FF);
	GPIOA->OSPEEDR &= (0xFF00FFFF);
	GPIOA->PUPDR &= ~(0x00FF0FF0);
	GPIOA->PUPDR |= 0x00000550; // pull up bits
	GPIOA->BRR |= 0x00000F00;
	
	RCC->AHB2ENR |= 0x02; // enable gpiob clock
	GPIOB->MODER &= ~(0x0003FC0);
	GPIOB->MODER |= 0x00001540;
}

// interrupt setup
void InterruptSetup() {
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; // enable syscfg clock
	
	GPIOB->MODER &= ~(0x00000003);
	
	SYSCFG->EXTICR[0] &= ~(SYSCFG_EXTICR1_EXTI0); // clear EXTI0 bit
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PB;
	
	EXTI->FTSR1 |= EXTI_FTSR1_FT0; // falling edge EXTI0
	// EXTI->PR1 = EXTI_PR1_PIF0; // clear EXTI0 pending bit
	EXTI->IMR1 |= EXTI_IMR1_IM0; // enable EXTI0
	
	NVIC_ClearPendingIRQ(EXTI0_IRQn);
	NVIC_EnableIRQ(EXTI0_IRQn);
	
	__enable_irq();
}

// debounce
void debounce() {
    // Simple delay loop
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 20000; j++) {
            volatile int n = j;
        }
    }
}

// display
void display() {
	if(keyPressed != 0xFF) {
		Display4BitValue(keyPressed);
		keyPressed = 0xFF;
	} else {
		DisplayCounter();
	}
	GPIOB->ODR = (counter << 3);
}

void Display4BitValue(uint8_t value) {
	GPIOB->ODR = (GPIOB->ODR & ~(0xF << 3)) | ((value & 0x0F) << 3);
}

// count
void count() {
	if(counter < 9) {
		counter++;
	} else {
		counter = 0;
	}
}

void DisplayCounter(void) {
	GPIOB->ODR = (GPIOB->ODR & ~(0xF << 3)) | ((counter & 0x0F) << 3);
}

// delay ~1 sec
void delay() {
	for (i=0; i<20; i++) { //outer loop
		for (j=0; j<6389; j++) { //inner loop
			n = j; //dummy operation for single-step test
		} //do nothing
	}
}

// read keypad
uint8_t Read_Keypad() {
	const uint8_t keyMap[NUM_ROWS][NUM_COLS] = {
		{1, 2, 3, 0xA},
		{4, 5, 6, 0xB},
		{7, 8, 9, 0xC},
		{0xE, 0, 0xF, 0xD}
	};
	
	for (uint8_t col = 0; col < NUM_COLS; col++) {
		GPIOA->BSRR = colPins[col]; // set column high
	}
	
	// Loop through columns
	for (col = 0; col < NUM_COLS; col++) {
		GPIOA->BSRR = (colPins[col] << 16); 
		
		for (uint8_t row = 0; row < NUM_ROWS; row++) {
			if (!(GPIOA->IDR & rowPins[row])) {
				debounce();
				if (!(GPIOA->IDR & rowPins[row])) {
					GPIOA->BSRR = colPins[col];
					return keyMap[row][col];
				}
			}
		}
		GPIOA->BSRR = colPins[col];
	}
	return 0xFF;
}

// row
/*int readRow() {
	GPIOA->MODER &= ~(0x00000FF0);
	GPIOA->MODER |= (0x00000000);
	GPIOA->ODR = 0;
	GPIOA->PUPDR &= ~(0x00000FF0);
	GPIOA->PUPDR |= (0x00000550);
	
	for(k = 0; k < 4; k++);
	
	int input = GPIOA->IDR&0xF;
	switch(input) {
		case 0xE:
			return 0;
		case 0xD:
			return 1;
		case 0xB:
			return 2;
		case 0x7:
			return 3;
		default:
			return -1;
	}
}*/

// col
/*int readCol() {
	GPIOA->MODER &= ~(0x00FF0000);
	GPIOA->MODER |= (0x00000000);
	GPIOA->ODR = 0;
	GPIOA->PUPDR &= ~(0x00FF0000);
	GPIOA->PUPDR |= (0x00550000);

	for(k = 0; k < 4; k++);
	
	int input = GPIOA->IDR&0xF0;
	switch(input) {
		case 0xE0:
			return 0;
		case 0xD0:
			return 1;
		case 0xB0:
			return 2;
		case 0x70:
			return 3;
		default:
			return -1;
	}
}*/

// Interrupt handler
void EXTI0_IRQHandler() {
	/*EXTI->PR1 |= 0x0001;
	
	row = readRow();
	col = readCol();
	
	if((row != -1) && (col != -1)) {
		keyVal = key_map[row][col];
		show = true;
		display();
	}
	GPIOA->MODER &= ~(0x00FF0FF0);
	GPIOA->MODER |= 0x00550000;
	GPIOA->PUPDR &= ~(0x00FF0FF0);
	GPIOA->PUPDR |= 0x00000550;
	
	EXTI->PR1 |= 0x0001;
	
	NVIC_ClearPendingIRQ(EXTI0_IRQn);
	NVIC_EnableIRQ(EXTI0_IRQn);*/
	
	if(EXTI->PR1 & EXTI_PR1_PIF0) {
		debounce();
		keyPressed = Read_Keypad();
		EXTI->PR1 |= EXTI_PR1_PIF0;
	}
	
}

// main
int main(void) {
	PinSetup();
	InterruptSetup();
	
	while(1) {
		display();
		count();
		delay();
	}
}
