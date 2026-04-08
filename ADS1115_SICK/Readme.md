#include "main.h"
#include "ads1115.h"



int main(void)
{

  if (ADS1115_Init(&hi2c1, ADS1115_DATA_RATE_128, ADS1115_PGA_TWOTHIRDS) == HAL_OK) {
  
      HAL_Delay(100);
	  
  } else {
  
      Error_Handler();
  }
  float ch0_mV = 0;
  char msg[100];
  float total_dis = 1000.0f;
  float ch0_mV ,ch1_mV , ch2_mV = 0;****

while(1){
  ADS1115_readSingleEnded(ADS1115_MUX_AIN0, &ch0_mV);
	float voltage0 = ch0_mV / 1000.0f;
	float dist0   = ((voltage0 - 0.19f)/4.97f)  *  total_dis; //Sick 1  
  int dist_int0  = (int)dist0;
	int dist_dec0  = (int)((dist0  - dist_int0) * 10);     // 1 decimal place
  
  int len = snprintf(msg, sizeof(msg)," D0:%d.%d CM \r\n", dist_int0);
  HAL_UART_Transmit(&huart2, (uint8_t*)msg, len, HAL_MAX_DELAY);
  HAL_Delay(100);          
  
  

}


}
