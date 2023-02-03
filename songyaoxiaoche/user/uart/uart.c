#include "uart.h"

/**
  * @brief  重定向printf()函数
  * @param  无
  * @retval 无
  */
int fputc(int ch, FILE* f)
{
  HAL_UART_Transmit(&huart1, (uint8_t*)&ch, 1, 0xffff);
  return ch;
}


void Transmit_uart()
{
  printf("十一呀");
}

