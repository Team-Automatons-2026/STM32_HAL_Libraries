





#include "main.h"

#### #include "BTS7960.h" // Include library 

#### BTS B1;  // Define a object 



int main(void)
{

  HAL_Init();
  
  SystemClock_Config();
  
  MX_GPIO_Init();
  
  MX_TIM1_Init();
  
  /* USER CODE BEGIN 2 */

####   InitBTS(&B1 , &htim1, TIM_CHANNEL_1, 2 , &htim1,TIM_CHANNEL_2, 2 , 20000); //Enter the timer and Channel number for intialization

  while (1)
  {
  
  #### RotateMotor(&B1,40); // Calling function and Giving PWM Value from 0 - 255 

  }

}


