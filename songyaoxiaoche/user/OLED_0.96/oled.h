#ifndef __oled_H_
#define __oled_H_

#include "main.h"
#include "gpio.h"


/* --------------------OLEDģʽѡ��-------------------- */
#define OLED_AGREEMENT_MODE 0	/*Э��ѡ��
									0ΪSPI���
									1ΪSPIӲ��
									2ΪIIC���
									3ΪIICӲ��*/

#define OLED_SHOW_MODE 0	/*��ʾ��ʽѡ��
								0Ϊֱ�����
								1Ϊ����������һ�����*/

#if OLED_AGREEMENT_MODE == 0

/* --------------------OLED_SPI�������-------------------- */
/* -----���Ŷ���----- */
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

/* -----�˿ڶ���----- */
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
								/* --------------------OLED_SPIӲ������-------------------- */
#include "spi.h"
#define OLEDspi hspi1
/* -----���Ŷ���----- */
#define OLED_RES_TYPE GPIOB
#define OLED_RES_PIN GPIO_PIN_4

#define OLED_DC_TYPE  GPIOB
#define OLED_DC_PIN  GPIO_PIN_6

#define OLED_CS_TYPE  GPIOB
#define OLED_CS_PIN  GPIO_PIN_7

/* -----�˿ڶ���----- */
#define OLED_RST_Clr() HAL_GPIO_WritePin(OLED_RES_TYPE, OLED_RES_PIN, GPIO_PIN_RESET) // RES
#define OLED_RST_Set() HAL_GPIO_WritePin(OLED_RES_TYPE, OLED_RES_PIN, GPIO_PIN_SET)

#define OLED_DC_Clr() HAL_GPIO_WritePin(OLED_DC_TYPE, OLED_DC_PIN, GPIO_PIN_RESET) // DC
#define OLED_DC_Set() HAL_GPIO_WritePin(OLED_DC_TYPE, OLED_DC_PIN, GPIO_PIN_SET)

#define OLED_CS_Clr() HAL_GPIO_WritePin(OLED_CS_TYPE, OLED_CS_PIN, GPIO_PIN_RESET) // CS
#define OLED_CS_Set() HAL_GPIO_WritePin(OLED_CS_TYPE, OLED_CS_PIN, GPIO_PIN_SET)



#elif OLED_AGREEMENT_MODE == 2
								/* --------------------OLED_IIC�������-------------------- */
								/* -----���Ŷ���----- */
#define OLED_SCLK_TYPE GPIOB		// SCL
#define OLED_SCLK_PIN GPIO_PIN_6	// SDA

#define OLED_SDIN_TYPE  GPIOB
#define OLED_SDIN_PIN  GPIO_PIN_7


/* -----�˿ڶ���----- */
#define OLED_SCLK_Clr() HAL_GPIO_WritePin(OLED_SCLK_TYPE, OLED_SCLK_PIN, GPIO_PIN_RESET) // SCL
#define OLED_SCLK_Set() HAL_GPIO_WritePin(OLED_SCLK_TYPE, OLED_SCLK_PIN, GPIO_PIN_SET)

#define OLED_SDIN_Clr() HAL_GPIO_WritePin(OLED_SDIN_TYPE, OLED_SDIN_PIN, GPIO_PIN_RESET) // SDA
#define OLED_SDIN_Set() HAL_GPIO_WritePin(OLED_SDIN_TYPE, OLED_SDIN_PIN, GPIO_PIN_SET)


#elif OLED_AGREEMENT_MODE == 3
								/* --------------------OLED_IICӲ������-------------------- */
#include "i2c.h"
/* -----�˿ڶ���----- */
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

#define OLED_CMD 0  //д����
#define OLED_DATA 1 //д����


/* -----����ԭ��----- */
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


