#include "oled.h"
#include "oledfont.h"



#if OLED_SHOW_MODE == 1
uint8_t xx[144][8];
#endif


#if OLED_AGREEMENT_MODE == 0		//0Ϊ���SPI,1ΪӲ��SPI
/**
 * @brief	��OLEDд���������
 * @param	dat:Ҫд�������/����
 * @param	cmd:0(OLED_CMD)Ϊд����,1Ϊд����
 * @retval	��
 */
void OLED_WR_Byte(uint8_t dat, uint8_t cmd)
{
	uint8_t i;
	if (cmd)
		OLED_DC_Set(); // DC����,д����
	else
		OLED_DC_Clr(); // DC����,д����
	//OLED_CS_Clr();	   //ʹ��������
	for (i = 0; i < 8; i++)
	{
		OLED_SCLK_Clr();	 //ʱ��������
		if (dat & 0x80)		 //��ȡ���λ����
			OLED_SDIN_Set(); //��1
		else
			OLED_SDIN_Clr(); //��0
		OLED_SCLK_Set();	 //ʱ����������
		dat <<= 1;			 //��λ
	}
	//OLED_CS_Set(); //ʹ��������
	// OLED_DC_Set();
}

#elif OLED_AGREEMENT_MODE == 1

/**
 * @brief	��OLEDд���������
 * @param	dat:Ҫд�������/����
 * @param	cmd:0(OLED_CMD)Ϊд����,1Ϊд����
 * @retval	��
 */
void OLED_WR_Byte(uint8_t dat, uint8_t cmd)
{
	if (cmd)
		OLED_DC_Set(); // DC����,д����
	else
		OLED_DC_Clr(); // DC����,д����
	//OLED_CS_Clr();	   //ʹ��������
	HAL_SPI_Transmit(&OLEDspi, &dat, sizeof(dat), 1000);//����Ӳ��SPIд����
	//OLED_CS_Set(); //ʹ��������
}

#elif OLED_AGREEMENT_MODE == 2
/**
  * @brief  IIC ��ʼ
  * @param  ��
  * @retval ��
  */
void IIC_Start()
{
	OLED_SDIN_Set();
	OLED_SCLK_Set();//ʱ�����ø�
	OLED_SDIN_Clr();//�������½���
}

/**
  * @brief  IIC ֹͣ
  * @param  ��
  * @retval ��
  */
void IIC_Stop()
{
	//OLED_SCLK_Clr();
	OLED_SDIN_Clr();
	OLED_SCLK_Set();//ʱ�����ø�
	OLED_SDIN_Set();//������������

}

/**
  * @brief  ����IIC��Ӧ�����
  * @param  ��
  * @retval ��
  */
void IIC_Wait_Ack()
{
	OLED_SCLK_Clr();
	OLED_SDIN_Set();
	OLED_SCLK_Set();
	OLED_SCLK_Clr();
}

/**
  * @brief  IIC д�ֽ�
  * @param  ��
  * @retval ��
  */
void Write_IIC_Byte(uint8_t dat)
{
	uint8_t i;
	//OLED_SCLK_Clr();
	for (i = 0;i < 8;i++)
	{
		OLED_SCLK_Clr();
		if (dat & 0x80)
		{
			OLED_SDIN_Set();
		}
		else OLED_SDIN_Clr();
		OLED_SCLK_Set();
		OLED_SCLK_Clr();
		dat = dat << 1;
	}
}

/**
 * @brief	��OLEDд���������
 * @param	dat:Ҫд�������/����
 * @param	cmd:0(OLED_CMD)Ϊд����,1Ϊд����
 * @retval	��
 */
void OLED_WR_Byte(uint8_t dat, uint8_t cmd)
{
	if (cmd)
	{
		IIC_Start();
		Write_IIC_Byte(0x78);	//D/C#=0; R/W#=0
		IIC_Wait_Ack();
		Write_IIC_Byte(0x40);	//write data
		IIC_Wait_Ack();
		Write_IIC_Byte(dat);
		IIC_Wait_Ack();
		IIC_Stop();
	}
	else
	{
		IIC_Start();
		Write_IIC_Byte(0x78);	//Slave address,SA0=0
		IIC_Wait_Ack();
		Write_IIC_Byte(0x00);	//write command
		IIC_Wait_Ack();
		Write_IIC_Byte(dat);
		IIC_Wait_Ack();
		IIC_Stop();
	}
}
#elif OLED_AGREEMENT_MODE == 3
uint8_t clear[128] = { 0 };
void OLED_WR_Byte(uint8_t dat, uint8_t cmd)
{
	if (cmd)
	{
		HAL_I2C_Mem_Write(hi2cX, 0x78, 0x40, I2C_MEMADD_SIZE_8BIT, &dat, 1, 100);
	}
	else
	{
		HAL_I2C_Mem_Write(hi2cX, 0x78, 0x00, I2C_MEMADD_SIZE_8BIT, &dat, 1, 100);
	}
}
#endif




/**
 * @brief	ָ��OLED��λ������
 * @param	x(�е�ַ0-127)
 * @param	y(ҳ��ַ0-7)
 * @retval	��
 */
void OLED_Set_Pos(uint8_t x, uint8_t y)
{
	OLED_WR_Byte(0xb0 + y, OLED_CMD);					//����Ŀ����ʾλ�õ�ҳ��ʼ��ַ
	OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);	//������ʾλ�á��иߵ�ַ
	OLED_WR_Byte((x & 0x0f) | 0x00, OLED_CMD);			//������ʾλ�á��е͵�ַ
}

#if OLED_SHOW_MODE == 1
/**
 * @brief	��ʾ�Դ������
 * @param	��
 * @retval	��
 */
void OLED_Refresh(void)
{
	uint8_t i, n;
	for (i = 0;i < 8;i++)
	{
		OLED_WR_Byte(0xb0 + i, OLED_CMD);
		OLED_WR_Byte(0x00, OLED_CMD);
		OLED_WR_Byte(0x10, OLED_CMD);
		for (n = 0;n < 128;n++)
			OLED_WR_Byte(xx[n][i], OLED_DATA);
	}
}

/**
 * @brief	��ָ���������һ����
 * @param	x(0~127)
 * @param	y(0~63)
 * @retval	��
 */
void OLED_DrawPoint(uint8_t x, uint8_t y)
{
	uint8_t i, m, n;
	i = y / 8;
	m = y % 8;
	n = 1 << m;
	xx[x][i] |= n;
}

/**
 * @brief	��ָ������ɾ��һ����
 * @param	x(0~127)
 * @param	y(0~63)
 * @retval	��
 */
void OLED_ClearPoint(uint8_t x, uint8_t y)
{
	uint8_t i, m, n;
	i = y / 8;
	m = y % 8;
	n = 1 << m;
	xx[x][i] = ~xx[x][i];
	xx[x][i] |= n;
	xx[x][i] = ~xx[x][i];
}



#endif



/**
 * @brief	����
 * @param	��
 * @retval	��
 */
void OLED_Clear(void)
{
	uint8_t i, n;
#if OLED_SHOW_MODE ==0
	for (i = 0; i < 8; i++)					//��0-7ҳ����
	{
		OLED_WR_Byte(0xb0 + i, OLED_CMD); 	//����ҳ��ַ��0~7��
		OLED_WR_Byte(0x00, OLED_CMD);	  	//������ʾλ�á��е͵�ַ
		OLED_WR_Byte(0x10, OLED_CMD);	  	//������ʾλ�á��иߵ�ַ

#if OLED_AGREEMENT_MODE ==3
		HAL_I2C_Mem_Write(&hi2c1, 0x78, 0x40, I2C_MEMADD_SIZE_8BIT, clear, 128, 1000);
#else
		for (n = 0; n < 128; n++)
			OLED_WR_Byte(0x00, OLED_DATA);
#endif

	}

#else
	for (i = 0;i < 8;i++)
	{
		for (n = 0;n < 132;n++)
		{
			xx[n][i] = 0;//���Դ�����
		}
	}
	OLED_Refresh();//��ʾ
#endif
}




/**
 * @brief	��ָ��λ����ʾһ���ַ�
 * @param	x(��)
 * @param	y(ҳ)
 * @param	chr(�ַ���Ӧ��ASCII��)
 * @retval	��
 */
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr)
{
	uint8_t c = 0, i = 0;
	c = chr - 32; 										//�õ�ƫ�ƺ��ֵ
	// if (x > Max_Column - 1)								//�ж�x�Ƿ񳬹�OLED���128
	// {
	// 	x = 0;
	// 	y = y + 2;
	// }
#if OLED_SHOW_MODE == 0
	OLED_Set_Pos(x, y);									//ָ��OLED�ĵ�һҳλ������
	for (i = 0; i < 8; i++)
		OLED_WR_Byte(F8X16[c * 16 + i], OLED_DATA);
	OLED_Set_Pos(x, y + 1);								//ָ��OLED�ĵڶ�ҳλ������
	for (i = 0; i < 8; i++)
		OLED_WR_Byte(F8X16[c * 16 + i + 8], OLED_DATA);
#else
	for (i = 0; i < 8; i++)
		xx[x + i][y] = F8X16[c * 16 + i];
	for (i = 0; i < 8; i++)
		xx[x + i][y + 1] = F8X16[c * 16 + i + 8];

#endif
}




/**
 * @brief	����m��n�η�
 * @param	m
 * @param	n
 * @retval	result:
 */
uint32_t oled_pow(uint8_t m, uint8_t n)
{
	uint32_t result = 1;
	while (n--)
		result *= m;
	return result;
}


/**
 * @brief	��ʾlen������
 * @param	x(��)
 * @param	y(ҳ)
 * @param	num(����)
 * @retval	��
 */
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len)
{
	uint8_t t, temp;
	uint8_t enshow = 0;
	for (t = 0; t < len; t++)
	{
		temp = (num / oled_pow(10, len - t - 1)) % 10;		//��ȡ��λ����
		if (enshow == 0 && t < (len - 1))
		{
			if (temp == 0)
			{
				OLED_ShowChar(x + (SIZE / 2) * t, y, ' ');
				continue;
			}
			else
				enshow = 1;
		}
		OLED_ShowChar(x + (SIZE / 2) * t, y, temp + '0');
	}

}


/**
 * @brief	��ʾһ���ַ���
 * @param	x(��)
 * @param	y(ҳ)
 * @param	chr(�ַ���)
 * @retval	��
 */
void OLED_ShowString(uint8_t x, uint8_t y, char* chr)
{
	uint8_t j = 0;
	while (chr[j] != '\0')
	{
		OLED_ShowChar(x, y, chr[j]);
		x += 8;
		if (x > 120)
		{
			x = 0;
			y += 2;
		}
		j++;
	}
}

/**
 * @brief	��ʾһ���ַ���
 * @param	x(��)
 * @param	y(ҳ)
 * @param	no(�������)
 * @retval	��
 */
void OLED_ShowCHinese(uint8_t x, uint8_t y, char* s)
{
	uint8_t t;
	uint16_t HZnum, k;
	while (*s != 0)
	{
		HZnum = sizeof(Hzk) / sizeof(typFNT_GB16);
		for (k = 0;k < HZnum;k++)
		{
			if ((Hzk[k].Index[0] == *(s)) && (Hzk[k].Index[1] == *(s + 1)))
			{
#if OLED_SHOW_MODE == 0
				OLED_Set_Pos(x, y);
				for (t = 0; t < 16; t++)
				{
					OLED_WR_Byte(Hzk[k].Msk[t], OLED_DATA);
				}
				OLED_Set_Pos(x, y + 1);
				for (t = 0; t < 16; t++)
				{
					OLED_WR_Byte(Hzk[k].Msk[t], OLED_DATA);
				}
#else
				for (t = 0; t < 16; t++)
				{
					xx[x + t][y] = Hzk[k].Msk[t];
				}
				for (t = 0; t < 16; t++)
				{
					xx[x + t][y + 1] = Hzk[k].Msk[t + 16];
				}
#endif
			}
		}
		s += 2;
		x += 16;
	}

}

/**
 * @brief	OLED��ʾBMPͼƬ
 * @param	x0
 * @param	y0
 * @param	x1
 * @param	y1
 * @param	BMP[] (ͼƬ)
 * @retval	��
 */


void OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t BMP[])
{
	uint32_t j = 0;
	uint8_t x, y;
	if (y1 % 8 == 0)
		y = y1 / 8;
	else
		y = y1 / 8 + 1;

#if OLED_SHOW_MODE == 0
	for (y = y0; y < y1; y++)
	{
		OLED_Set_Pos(x0, y);
		for (x = x0; x < x1; x++)
		{
			OLED_WR_Byte(BMP[j++], OLED_DATA);
		}
	}
#else
	for (y = y0; y < y1; y++)
	{
		for (x = x0; x < x1; x++)
		{
			xx[x][y] = BMP[j];
			j++;
		}
	}
#endif
}



/**
 * @brief	OLEDˮƽ����
 * @param	��
 * @retval	��
 */
void OLED_horizontal_scroll()
{
	OLED_WR_Byte(0x2E, OLED_CMD); //�رչ���
	OLED_WR_Byte(0x26, OLED_CMD); //ˮƽ��������ҹ��� 26/27
	OLED_WR_Byte(0x00, OLED_CMD); //�����ֽ�
	OLED_WR_Byte(0x00, OLED_CMD); //��ʼҳ 0
	OLED_WR_Byte(0x07, OLED_CMD); /*����ʱ����
									0x00: 5 frams
									0x01: 64 frams
									0x02: 128 frams
									0x03: 256 frams
									0x04: 3 frams
									0x05: 4 frams
									0x06: 25 fram
									0x07: 2 fram */
	OLED_WR_Byte(0x07, OLED_CMD); //��ֹҳ 7
	OLED_WR_Byte(0x00, OLED_CMD); //�����ֽ�
	OLED_WR_Byte(0xFF, OLED_CMD); //�����ֽ�
	OLED_WR_Byte(0x2F, OLED_CMD); //��������
}

/**
 * @brief	OLEDˮƽ��ֱ����
 * @param	��
 * @retval	��
 */
void OLED_vertical_scroll()
{
	OLED_WR_Byte(0x2e, OLED_CMD); //�رչ���
	OLED_WR_Byte(0x29, OLED_CMD); //ˮƽ��ֱ��ˮƽ�������� 29/2a
	OLED_WR_Byte(0x00, OLED_CMD); //�����ֽ�
	OLED_WR_Byte(0x00, OLED_CMD); //��ʼҳ 0
	OLED_WR_Byte(0x07, OLED_CMD); /*����ʱ����
									0x00: 5 frams
									0x01: 64 frams
									0x02: 128 frams
									0x03: 256 frams
									0x04: 3 frams
									0x05: 4 frams
									0x06: 25 fram
									0x07: 2 fram */
	OLED_WR_Byte(0x07, OLED_CMD); //��ֹҳ 1
	OLED_WR_Byte(0x01, OLED_CMD); /*��ֱ����ƫ����
									0x01��0*3F ��Ӧ 1��63*/
	OLED_WR_Byte(0x2F, OLED_CMD); //��������
}



/**
 * @brief	��ʼ��OLED
 * @param	��
 * @retval	��
 */
void OLED_Init(void)
{
#if OLED_AGREEMENT_MODE == 0 || OLED_AGREEMENT_MODE==1	
	OLED_RST_Clr();			//RES����
	OLED_RST_Set();			//RES����
	OLED_DC_Clr(); 			// DC����
#endif


	OLED_WR_Byte(0xAE, OLED_CMD);//--������ʾ��

	OLED_WR_Byte(0x00, OLED_CMD); //---�����е�λ��ַ
	OLED_WR_Byte(0x10, OLED_CMD); //---�����и�λ��ַ

	OLED_WR_Byte(0x40, OLED_CMD); //--����RAM��ʾ��ʼ��

	OLED_WR_Byte(0x81, OLED_CMD); //--���öԱȶ�
	OLED_WR_Byte(0xFF, OLED_CMD); //--�Աȶ���ֵ

	OLED_WR_Byte(0xA1, OLED_CMD); //--������Ļˮƽ����     0xa0���ҷ��� 0xa1����
	OLED_WR_Byte(0xC8, OLED_CMD); //--������Ļ��ֱ����   0xc0���·��� 0xc8����

	OLED_WR_Byte(0xA6, OLED_CMD); //--����������ʾ		0xA7Ϊ��ת��ʾ

	OLED_WR_Byte(0xA8, OLED_CMD); //--���ö�·������(16 to 64)
	OLED_WR_Byte(0x3F, OLED_CMD); //--1/64 duty

	OLED_WR_Byte(0xD3, OLED_CMD); //-������ʾƫ�� (0x00~0x3F)
	OLED_WR_Byte(0x00, OLED_CMD); //-ƫ����ֵ

	OLED_WR_Byte(0xD5, OLED_CMD); //--������ʾʱ�ӷ�Ƶ��/����Ƶ��
	OLED_WR_Byte(0x80, OLED_CMD); //--���÷�Ƶ�ȣ���ʱ������Ϊ100֡/��

	OLED_WR_Byte(0xD9, OLED_CMD); //--����Ԥ�������
	OLED_WR_Byte(0xF1, OLED_CMD); // ��Ԥ�������Ϊ15��ʱ�ӣ��ŵ�����Ϊ1��ʱ��

	OLED_WR_Byte(0xDA, OLED_CMD); //--����com����Ӳ������
	OLED_WR_Byte(0x12, OLED_CMD); //--

	OLED_WR_Byte(0xDB, OLED_CMD); //--����VCOMHȡ��ѡ��ȼ�
	OLED_WR_Byte(0x40, OLED_CMD); //--Set VCOM Deselect Level

	OLED_WR_Byte(0x8D, OLED_CMD); //--set Charge Pump enable/disable
	OLED_WR_Byte(0x14, OLED_CMD); //--set(0x10) disable

	OLED_WR_Byte(0x20, OLED_CMD); //--�����ڴ�ѡַģʽ (0x00/0x01/0x02)
	OLED_WR_Byte(0x02, OLED_CMD); //--ҳ��ѡַģʽ

	OLED_WR_Byte(0xA4, OLED_CMD); // ������ʾRAM���ݣ�����λ (0xa4/0xa5)
	OLED_WR_Byte(0xAF, OLED_CMD); //--������ʾ����

	OLED_Clear();
	OLED_Set_Pos(0, 0);
}

