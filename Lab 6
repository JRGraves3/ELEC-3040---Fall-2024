#include "stm32l4xx.h"

#define NUM_ROWS 4
#define NUM_COLS 4

static uint32_t rowPins[NUM_ROWS] = {0x04, 0x08, 0x10, 0x20}; // PA[2:5]
static uint32_t colPins[NUM_COLS] = {0x100, 0x200, 0x400, 0x800}; // PA[8:11]

static int RESET_COUNT = 0;

// setups
void PinSetup(void);
void InterruptSetup(void);
void TimerSetup(void);

// handlers
void TIM6_DAC_IRQHandler(void);
void EXTI0_IRQHandler(void);

// functions
uint8_t Read_Keypad(void);
void delay(void);
void debounce(void);
void Display4BitValue(uint8_t value);
void Toggle_Timer(void);
int Timer_Stopped(void);
void Update_Display(void);
void Reset_Display(void);

void PinSetup() {
	// Enable GPIOA and GPIOB clocks
	RCC->AHB2ENR |= 0x03;
	
	// Configure GPIOA pins
	GPIOA->MODER &= ~0xFF0FF0;
	GPIOA->MODER |= 0x550000;
	
	GPIOA->PUPDR &= ~0xFF0;
	GPIOA->PUPDR |= 0x550;
	
	// Configure GPIOB pins as outputs for display
	GPIOB->MODER &= ~(0x00003FC3);
	GPIOB->MODER |= 0x00001540;
	
	GPIOA->ODR = 0x0 << 2;
}

void InterruptSetup() {
	// enable clock
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	
	// configure interrupt for PB0
	SYSCFG->EXTICR[0] &= ~(SYSCFG_EXTICR1_EXTI0);
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PB;
	
	// set falling edge trigger and unmask line 0
	EXTI->FTSR1 |= EXTI_FTSR1_FT0;
	EXTI->PR1 |= EXTI_PR1_PIF0;
	EXTI->IMR1 |= EXTI_IMR1_IM0;
	
	// enable EXTI0 interrupt
	NVIC_ClearPendingIRQ(EXTI0_IRQn);
	NVIC_EnableIRQ(EXTI0_IRQn);
	
	// enable interrupts globally
	__enable_irq();
}

void TimerSetup() {
	// Enable TIM6 clock
	RCC->APB1ENR1 |= 0x00000010; // TIM6
	
	// configure the timer
	TIM6->PSC = 999; // set prescaler to get 1 second tick
	TIM6->ARR = 3999; // set auto-reload value for 1 second
	
	// enable TIM6 interrupt
	TIM6->DIER |= TIM_DIER_UIE;
	
	// enable TIM6 interrupt in NVIC
	NVIC_EnableIRQ(TIM6_DAC_IRQn);
}

void delay() {
	for(int i = 0; i < 20; i++) {
		for(int j = 0; j < 6389; j++) {
			volatile int n = j;
		}
	}
}

void debounce() {
	// simple delay loop
	for(int i = 0; i < 2; i++) {
		for(int j = 0; j < 20000; j++) {
			volatile int n = j;
		}
	}
}

void Update_Display() {
	static uint8_t count = 0; // maintain the current count
	if(RESET_COUNT == 1) {
		count = 0;
		GPIOB->ODR &= ~(0xF << 3);
	} else {
		count = (count + 1) % 10; // increment and wrap around at 10
		
		// convert count to BCD and output to the 4-bit LED display
		GPIOB->ODR = (GPIOB->ODR & ~(0xF << 3)) | ((count & 0x0F) << 3);
	}
}

void Reset_Display() {
	// reset the 4-bit LED display to 0
	GPIOB->ODR &= ~(0xF << 3);
}

void EXTI0_IRQHandler() {
	if (EXTI->PR1 & EXTI_PR1_PIF0) {
		//debounce();
		int key = Read_Keypad();
		if (key == 0x2) { // Start/Stop button
			RESET_COUNT = 0;   
			Toggle_Timer(); // Implement to toggle TIM6 on/off
    } else if (key == 0x0 && Timer_Stopped()) { // Clear button, only if timer is stopped
			RESET_COUNT = 1; 
			Update_Display();
      Reset_Display(); // Implement to reset the LED display to 0
		}
		GPIOA->ODR = 0x0 << 2;
    EXTI->PR1 |= EXTI_PR1_PIF0;  // Clear interrupt flag
		NVIC_ClearPendingIRQ(EXTI0_IRQn);
  }
}

uint8_t Read_Keypad(void) {
	const uint8_t keyMap[NUM_ROWS][NUM_COLS] = {
		{1, 2, 3, 0xA},
		{4, 5, 6, 0xB},
		{7, 8, 9, 0xC},
		{0xE, 0, 0xF, 0xD}
	};
	
	// initialize all columns to high before starting the scan
	for (uint8_t col = 0; col < NUM_COLS; col++) {
		GPIOA->BSRR = colPins[col]; // set column high
	}
	
	// Loop through columns
	for (uint8_t col = 0; col < NUM_COLS; col++) {
		GPIOA->BSRR = (colPins[col] << 16); // reset current column to low
		
		// loop through rows
		for (uint8_t row = 0; row < NUM_ROWS; row++) {
			if (!(GPIOA->IDR & rowPins[row])) {
				debounce();
				// confirm button is pressed after debounce
				if (!(GPIOA->IDR & rowPins[row])) {
					// reset current column to high before returning
					GPIOA->BSRR = colPins[col];
					return keyMap[row][col]; // return the key value
				}
			}
		}
		// reset current column to high after checking all rows
		GPIOA->BSRR = colPins[col];
	}
	return 0xFF; // indicate no key pressed
}

void TIM6_DAC_IRQHandler(void) {
	if(TIM6->SR & TIM_SR_UIF) { // check if update interrupt flag is set
		TIM6->SR &= ~TIM_SR_UIF; // clear update interrupt flag
		if(!(TIM6->CR1 & TIM_CR1_CEN)) {
			return; // do not update the display if timer is stopped
		}
		Update_Display();
	}
}

void Toggle_Timer(void) {
	if (TIM6->CR1 & TIM_CR1_CEN) {
    TIM6->CR1 &= ~TIM_CR1_CEN; // Disable TIM6
  } else {
    TIM6->CNT = 0; // Reset the counter
    TIM6->CR1 |= TIM_CR1_CEN; // Enable TIM6
  }
}

int Timer_Stopped(void) {
	// Check is TIM6 is currently disabled
	return !(TIM6->CR1 & TIM_CR1_CEN);
}

int main(void) {
	PinSetup();
	InterruptSetup();
	TimerSetup();
	
	while(1) {}
}
