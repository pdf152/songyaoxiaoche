#ifndef __oled_H_
#define __oled_H_

#include "main.h"
#include "gpio.h"


/* --------------------OLED模式选择-------------------- */
#define OLED_AGREEMENT_MODE 0	/*协议选择
									0为SPI软件
									1为SPI硬件
									2为IIC软件
									3为IIC硬件*/

#define OLED_SHOW_MODE 0	/*显示方式选择
								0为直接输出
								1为储存入数组一起输出*/

#if OLED_AGREEMENT_MODE == 0

/* --------------------OLED_SPI软件定义-------------------- */
/* -----引脚定义----- */
#define OLED_SCL_TYPE GPIOB         //D0
#define OLED_SCL_PIN GPIO_PIN_3 

#define OLED_SDIN_TYPE GPIOB
#define OLED_SDIN_PIN GPIO_PIN_5   //D1

#define OLED_RES_TYPE GPIOB
#define OLED_RES_PIN GPIO_PIN_4

#define OLED_DC_TYPE  GPIOB
#define OLED_DC_PIN  GPIO_PIN_6

#define OLED_CS_TYPE  GPIOB
#define OLED_CS_PIN  GPIO_PIN_7

/* -----端口定义----- */
#define OLED_SCLK_Clr() HAL_GPIO_WritePin(OLED_SCL_TYPE, OLED_SCL_PIN, GPIO_PIN_RESET) // CLK
#define OLED_SCLK_Set() HAL_GPIO_WritePin(OLED_SCL_TYPE, OLED_SCL_PIN, GPIO_PIN_SET)

#define OLED_SDIN_Clr() HAL_GPIO_WritePin(OLED_SDIN_TYPE, OLED_SDIN_PIN, GPIO_PIN_RESET) // DIN
#define OLED_SDIN_Set() HAL_GPIO_WritePin(OLED_SDIN_TYPE, OLED_SDIN_PIN, GPIO_PIN_SET)

#define OLED_RST_Clr() HAL_GPIO_WritePin(OLED_RES_TYPE, OLED_RES_PIN, GPIO_PIN_RESET) // RES
#define OLED_RST_Set() HAL_GPIO_WritePin(OLED_RES_TYPE, OLED_RES_PIN, GPIO_PIN_SET)

#define OLED_DC_Clr() HAL_GPIO_WritePin(OLED_DC_TYPE, OLED_DC_PIN, GPIO_PIN_RESET) // DC
#define OLED_DC_Set() HAL_GPIO_WritePin(OLED_DC_TYPE, OLED_DC_PIN, GPIO_PIN_SET)

#define OLED_CS_Clr() HAL_GPIO_WritePin(OLED_CS_TYPE, OLED_CS_PIN, GPIO_PIN_RESET) // CS
#define OLED_CS_Set() HAL_GPIO_WritePin(OLED_CS_TYPE, OLED_CS_PIN, GPIO_PIN_SET) 




#elif OLED_AGREEMENT_MODE == 1
								/* --------------------OLED_SPI硬件定义-------------------- */
#include "spi.h"
#define OLEDspi hspi1
/* -----引脚定义----- */
#define OLED_RES_TYPE GPIOB
#define OLED_RES_PIN GPIO_PIN_4

#define OLED_DC_TYPE  GPIOB
#define OLED_DC_PIN  GPIO_PIN_6

#define OLED_CS_TYPE  GPIOB
#define OLED_CS_PIN  GPIO_PIN_7

/* -----端口定义----- */
#define OLED_RST_Clr() HAL_GPIO_WritePin(OLED_RES_TYPE, OLED_RES_PIN, GPIO_PIN_RESET) // RES
#define OLED_RST_Set() HAL_GPIO_WritePin(OLED_RES_TYPE, OLED_RES_PIN, GPIO_PIN_SET)

#define OLED_DC_Clr() HAL_GPIO_WritePin(OLED_DC_TYPE, OLED_DC_PIN, GPIO_PIN_RESET) // DC
#define OLED_DC_Set() HAL_GPIO_WritePin(OLED_DC_TYPE, OLED_DC_PIN, GPIO_PIN_SET)

#define OLED_CS_Clr() HAL_GPIO_WritePin(OLED_CS_TYPE, OLED_CS_PIN, GPIO_PIN_RESET) // CS
#define OLED_CS_Set() HAL_GPIO_WritePin(OLED_CS_TYPE, OLED_CS_PIN, GPIO_PIN_SET)



#elif OLED_AGREEMENT_MODE == 2
								/* --------------------OLED_IIC软件定义-------------------- */
								/* -----引脚定义----- */
#define OLED_SCLK_TYPE GPIOB		// SCL
#define OLED_SCLK_PIN GPIO_PIN_6	// SDA

#define OLED_SDIN_TYPE  GPIOB
#define OLED_SDIN_PIN  GPIO_PIN_7


/* -----端口定义----- */
#define OLED_SCLK_Clr() HAL_GPIO_WritePin(OLED_SCLK_TYPE, OLED_SCLK_PIN, GPIO_PIN_RESET) // SCL
#define OLED_SCLK_Set() HAL_GPIO_WritePin(OLED_SCLK_TYPE, OLED_SCLK_PIN, GPIO_PIN_SET)

#define OLED_SDIN_Clr() HAL_GPIO_WritePin(OLED_SDIN_TYPE, OLED_SDIN_PIN, GPIO_PIN_RESET) // SDA
#define OLED_SDIN_Set() HAL_GPIO_WritePin(OLED_SDIN_TYPE, OLED_SDIN_PIN, GPIO_PIN_SET)


#elif OLED_AGREEMENT_MODE == 3
								/* --------------------OLED_IIC硬件定义-------------------- */
#include "i2c.h"
/* -----端口定义----- */
#define hi2cX &hi2c1
#endif


#define SIZE 16
#define XLevelL 0x00
#define XLevelH 0x10
#define Max_Column 128
#define Max_Row 64
#define Brightness 0xFF
#define X_WIDTH 128
#define Y_WIDTH 64

#define OLED_CMD 0  //写命令
#define OLED_DATA 1 //写数据


/* -----函数原型----- */
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr);
void OLED_Clear(void);
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len);
void OLED_ShowString(uint8_t x, uint8_t y, char* chr);
void OLED_ShowCHinese(uint8_t x, uint8_t y, char* s);
void OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t BMP[]);
void OLED_horizontal_scroll(void);
void OLED_vertical_scroll(void);
void OLED_Init(void);

#if OLED_SHOW_MODE == 1
void OLED_Refresh(void);
void OLED_DrawPoint(uint8_t x, uint8_t y);
void OLED_ClearPoint(uint8_t x, uint8_t y);
#endif

#endif /*__oled_SPI_H_*/


