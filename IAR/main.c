
#include <stm8s.h>

typedef enum {
	longpress,
	shortpress,
        poweroff,
        none
} btnpress_t;


#define CCR3_Val  ((uint16_t)0)
#define CCR4_Val  ((uint16_t)0)
#define INTERRUPT_PIN GPIO_PIN_4

#define button GPIO_ReadInputPin(GPIOD, GPIO_PIN_4)

static void TIM1_Config(void);
void delay(uint16_t delay);
void delay_ms(uint16_t ms);

void handleButton(btnpress_t btnEvent);
void handleLEDStage(void);
void handleLEDSwitch(void);
void handlePowerOff(void);
void handlePowerOn(void);
void setLED_CCR(uint16_t value);

btnpress_t buttonLoop();

uint16_t CCR3 = 0;

volatile uint8_t buttonHit = 0;

#define LED_STAGES 7;
uint16_t LED1_STAGES[] = {1,8,50,200,700,1200,1800};
uint16_t LED2_STAGES[] = {1,8,50,200,700,1200,1800};

uint8_t LED_TYPE = 0;
uint8_t LED_STAGE = 0;
uint8_t LED_PowerOff = 1;

/**
  * @brief External Interrupt PORTD Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI_PORTD_IRQHandler, 6)
{
  buttonHit = 1;
}

int main( void )
{
  btnpress_t res = none;
  //PD_CR2 = INTERRUPT_PIN;
  
  GPIO_Init(GPIOD, GPIO_PIN_4, GPIO_MODE_IN_FL_IT);

  /* Initialize the Interrupt sensitivity */
  EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOD, EXTI_SENSITIVITY_RISE_ONLY);
  
  rim();
  //EXTI_SetTLISensitivity(EXTI_TLISENSITIVITY_RISE_ONLY);
  
  //GPIO_Init(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_3 | GPIO_PIN_4), GPIO_MODE_OUT_PP_LOW_FAST);
  
  TIM1_Config();
      //TIM1_SetCompare4(CCR3);
    //delay(50);
    CCR3++;
    //if(CCR3  == 1000) CCR3 = 0;
  while(1)
  {
    if(buttonHit)
    {
      buttonHit = 0;
       res = buttonLoop();
       //handleButton(res);
       while(button);
       delay_ms(50);
    }
    
    //delay_ms(1000);
    //TIM1_SetCompare4(100);
    //delay_ms(1000);
    //TIM1_SetCompare4(0);
    
  }  
}

void handleButton(btnpress_t btnEvent)
{
  
  if(btnEvent == shortpress)
  {
    LED_STAGE++;
    if(LED_STAGE == 7) LED_STAGE = 0;
    handleLEDStage();
  }
  if(btnEvent == longpress)
  {
    handleLEDSwitch();    
  }
  if(btnEvent == poweroff)
  {
    handlePowerOff(); 
  }
  
}

void handleLEDStage(void)
{
  if(LED_TYPE == 1)
  {
    setLED_CCR(LED1_STAGES[LED_STAGE]);
  }
  else
  {
    setLED_CCR(LED2_STAGES[LED_STAGE]);
  }
}

void handleLEDSwitch(void)
{
  if(LED_TYPE == 1) LED_TYPE = 0;
  else LED_TYPE = 1;
  handleLEDStage();
}

void setLED_CCR(uint16_t value)
{
  if(LED_TYPE == 1)
  {
    TIM1_SetCompare3(0);
    TIM1_SetCompare4(value);
  }
  else 
  {
    TIM1_SetCompare4(0);
    TIM1_SetCompare3(value); 
  }
}

void handlePowerOff(void)
{
  uint16_t CCR = 0;
  uint16_t timeout = 0;
  uint8_t step = 0;
  
  if(LED_TYPE == 1) CCR = LED1_STAGES[LED_STAGE];
  else CCR = LED2_STAGES[LED_STAGE];
 
  step = CCR / 4;
  /*
  
  if(step == 0) step = 1;
  
  while(timeout != CCR)
  {
    if(CCR > 0) CCR--;
    setLED_CCR(CCR); 
    delay_ms(5);
    timeout++;
  }  */
  

  setLED_CCR(step);
  delay_ms(250);
  
  TIM1_SetCompare3(0);
  TIM1_SetCompare4(0);
  LED_STAGE = 0;
  
  LED_PowerOff = 1;
  if(LED_TYPE == 1) LED_TYPE = 0;
  else LED_TYPE = 1;
  delay_ms(2500);
  
  
  halt();
}

void handlePowerOn(void)
{
  
}


void delay(uint16_t delay)
{
 while(delay--); 
}

void delay_ms(uint16_t ms)
{
  while(ms--)
  {
    delay(240); 
  }
}

btnpress_t buttonLoop()
{
    uint16_t timeout = 0;
    while(timeout != 2500 && button != 0)
    {
      delay_ms(1);
      if(timeout == 1500) handleButton(longpress);
      timeout++;
    }
    
    if(timeout < 1000 && timeout > 2) handleButton(shortpress);
    if(timeout == 2500) handleButton(poweroff);
    
    return longpress;
}



static void TIM1_Config(void)
{

   TIM1_DeInit();

  /* Time Base configuration */
  /*
  TIM1_Period = 4095
  TIM1_Prescaler = 0
  TIM1_CounterMode = TIM1_COUNTERMODE_UP
  TIM1_RepetitionCounter = 0
  */

  TIM1_TimeBaseInit(0, TIM1_COUNTERMODE_UP, 2500, 0);

  /* Channel 3 and 4 Configuration in PWM mode */
  
  /*
  TIM1_OCMode = TIM1_OCMODE_PWM2
  TIM1_OutputState = TIM1_OUTPUTSTATE_ENABLE
  TIM1_OutputNState = TIM1_OUTPUTNSTATE_ENABLE
  TIM1_Pulse = CCR1_Val
  TIM1_OCPolarity = TIM1_OCPOLARITY_LOW
  TIM1_OCNPolarity = TIM1_OCNPOLARITY_HIGH
  TIM1_OCIdleState = TIM1_OCIDLESTATE_SET
  TIM1_OCNIdleState = TIM1_OCIDLESTATE_RESET
  
  */

  /*TIM1_Pulse = CCR3_Val*/
  TIM1_OC3Init(TIM1_OCMODE_PWM2, TIM1_OUTPUTSTATE_ENABLE, TIM1_OUTPUTNSTATE_ENABLE,
               CCR3_Val, TIM1_OCPOLARITY_LOW, TIM1_OCNPOLARITY_HIGH, TIM1_OCIDLESTATE_SET,
               TIM1_OCNIDLESTATE_RESET);

  /*TIM1_Pulse = CCR4_Val*/
  TIM1_OC4Init(TIM1_OCMODE_PWM2, TIM1_OUTPUTSTATE_ENABLE, CCR4_Val, TIM1_OCPOLARITY_LOW,
               TIM1_OCIDLESTATE_SET);

  /* TIM1 counter enable */
  TIM1_Cmd(ENABLE);

  /* TIM1 Main Output Enable */
  TIM1_CtrlPWMOutputs(ENABLE);
}


void assert_failed(uint8_t* file, uint32_t line) 
{
  /* User can add his own implementation to report the file name and line number,
    ex: printf(''Wrong parameters value: file %s on line %d\r\n'', file, line) */
}
