#include "oled.h"
#include "oledfont.h"



#if OLED_SHOW_MODE == 1
uint8_t xx[144][8];
#endif


#if OLED_AGREEMENT_MODE == 0		//0为软件SPI,1为硬件SPI
/**
 * @brief	对OLED写命令或数据
 * @param	dat:要写入的数据/命令
 * @param	cmd:0(OLED_CMD)为写命令,1为写数据
 * @retval	无
 */
void OLED_WR_Byte(uint8_t dat, uint8_t cmd)
{
	uint8_t i;
	if (cmd)
		OLED_DC_Set(); // DC拉高,写数据
	else
		OLED_DC_Clr(); // DC拉低,写命令
	//OLED_CS_Clr();	   //使能线拉低
	for (i = 0; i < 8; i++)
	{
		OLED_SCLK_Clr();	 //时钟线拉低
		if (dat & 0x80)		 //读取最高位数据
			OLED_SDIN_Set(); //置1
		else
			OLED_SDIN_Clr(); //置0
		OLED_SCLK_Set();	 //时钟线上升沿
		dat <<= 1;			 //移位
	}
	//OLED_CS_Set(); //使能线拉高
	// OLED_DC_Set();
}

#elif OLED_AGREEMENT_MODE == 1

/**
 * @brief	对OLED写命令或数据
 * @param	dat:要写入的数据/命令
 * @param	cmd:0(OLED_CMD)为写命令,1为写数据
 * @retval	无
 */
void OLED_WR_Byte(uint8_t dat, uint8_t cmd)
{
	if (cmd)
		OLED_DC_Set(); // DC拉高,写数据
	else
		OLED_DC_Clr(); // DC拉低,写命令
	//OLED_CS_Clr();	   //使能线拉低
	HAL_SPI_Transmit(&OLEDspi, &dat, sizeof(dat), 1000);//调用硬件SPI写数据
	//OLED_CS_Set(); //使能线拉高
}

#elif OLED_AGREEMENT_MODE == 2
/**
  * @brief  IIC 开始
  * @param  无
  * @retval 无
  */
void IIC_Start()
{
	OLED_SDIN_Set();
	OLED_SCLK_Set();//时钟线置高
	OLED_SDIN_Clr();//数据线下降沿
}

/**
  * @brief  IIC 停止
  * @param  无
  * @retval 无
  */
void IIC_Stop()
{
	//OLED_SCLK_Clr();
	OLED_SDIN_Clr();
	OLED_SCLK_Set();//时钟线置高
	OLED_SDIN_Set();//数据线上升沿

}

/**
  * @brief  跳过IIC的应答过程
  * @param  无
  * @retval 无
  */
void IIC_Wait_Ack()
{
	OLED_SCLK_Clr();
	OLED_SDIN_Set();
	OLED_SCLK_Set();
	OLED_SCLK_Clr();
}

/**
  * @brief  IIC 写字节
  * @param  无
  * @retval 无
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
 * @brief	对OLED写命令或数据
 * @param	dat:要写入的数据/命令
 * @param	cmd:0(OLED_CMD)为写命令,1为写数据
 * @retval	无
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
 * @brief	指向OLED的位置坐标
 * @param	x(列地址0-127)
 * @param	y(页地址0-7)
 * @retval	无
 */
void OLED_Set_Pos(uint8_t x, uint8_t y)
{
	OLED_WR_Byte(0xb0 + y, OLED_CMD);					//设置目标显示位置的页起始地址
	OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);	//设置显示位置—列高地址
	OLED_WR_Byte((x & 0x0f) | 0x00, OLED_CMD);			//设置显示位置—列低地址
}

#if OLED_SHOW_MODE == 1
/**
 * @brief	显示显存的内容
 * @param	无
 * @retval	无
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
 * @brief	在指定坐标添加一个点
 * @param	x(0~127)
 * @param	y(0~63)
 * @retval	无
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
 * @brief	在指定坐标删除一个点
 * @param	x(0~127)
 * @param	y(0~63)
 * @retval	无
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
 * @brief	清屏
 * @param	无
 * @retval	无
 */
void OLED_Clear(void)
{
	uint8_t i, n;
#if OLED_SHOW_MODE ==0
	for (i = 0; i < 8; i++)					//对0-7页操作
	{
		OLED_WR_Byte(0xb0 + i, OLED_CMD); 	//设置页地址（0~7）
		OLED_WR_Byte(0x00, OLED_CMD);	  	//设置显示位置—列低地址
		OLED_WR_Byte(0x10, OLED_CMD);	  	//设置显示位置—列高地址

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
			xx[n][i] = 0;//将显存清零
		}
	}
	OLED_Refresh();//显示
#endif
}




/**
 * @brief	在指定位置显示一个字符
 * @param	x(列)
 * @param	y(页)
 * @param	chr(字符对应的ASCII码)
 * @retval	无
 */
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr)
{
	uint8_t c = 0, i = 0;
	c = chr - 32; 										//得到偏移后的值
	// if (x > Max_Column - 1)								//判断x是否超过OLED宽度128
	// {
	// 	x = 0;
	// 	y = y + 2;
	// }
#if OLED_SHOW_MODE == 0
	OLED_Set_Pos(x, y);									//指向OLED的第一页位置坐标
	for (i = 0; i < 8; i++)
		OLED_WR_Byte(F8X16[c * 16 + i], OLED_DATA);
	OLED_Set_Pos(x, y + 1);								//指向OLED的第二页位置坐标
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
 * @brief	计算m的n次方
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
 * @brief	显示len个数字
 * @param	x(列)
 * @param	y(页)
 * @param	num(数字)
 * @retval	无
 */
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len)
{
	uint8_t t, temp;
	uint8_t enshow = 0;
	for (t = 0; t < len; t++)
	{
		temp = (num / oled_pow(10, len - t - 1)) % 10;		//提取各位数字
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
 * @brief	显示一个字符串
 * @param	x(列)
 * @param	y(页)
 * @param	chr(字符串)
 * @retval	无
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
 * @brief	显示一个字符串
 * @param	x(列)
 * @param	y(页)
 * @param	no(文字序号)
 * @retval	无
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
 * @brief	OLED显示BMP图片
 * @param	x0
 * @param	y0
 * @param	x1
 * @param	y1
 * @param	BMP[] (图片)
 * @retval	无
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
 * @brief	OLED水平滚动
 * @param	无
 * @retval	无
 */
void OLED_horizontal_scroll()
{
	OLED_WR_Byte(0x2E, OLED_CMD); //关闭滚动
	OLED_WR_Byte(0x26, OLED_CMD); //水平向左或者右滚动 26/27
	OLED_WR_Byte(0x00, OLED_CMD); //虚拟字节
	OLED_WR_Byte(0x00, OLED_CMD); //起始页 0
	OLED_WR_Byte(0x07, OLED_CMD); /*滚动时间间隔
									0x00: 5 frams
									0x01: 64 frams
									0x02: 128 frams
									0x03: 256 frams
									0x04: 3 frams
									0x05: 4 frams
									0x06: 25 fram
									0x07: 2 fram */
	OLED_WR_Byte(0x07, OLED_CMD); //终止页 7
	OLED_WR_Byte(0x00, OLED_CMD); //虚拟字节
	OLED_WR_Byte(0xFF, OLED_CMD); //虚拟字节
	OLED_WR_Byte(0x2F, OLED_CMD); //开启滚动
}

/**
 * @brief	OLED水平垂直滚动
 * @param	无
 * @retval	无
 */
void OLED_vertical_scroll()
{
	OLED_WR_Byte(0x2e, OLED_CMD); //关闭滚动
	OLED_WR_Byte(0x29, OLED_CMD); //水平垂直和水平滚动左右 29/2a
	OLED_WR_Byte(0x00, OLED_CMD); //虚拟字节
	OLED_WR_Byte(0x00, OLED_CMD); //起始页 0
	OLED_WR_Byte(0x07, OLED_CMD); /*滚动时间间隔
									0x00: 5 frams
									0x01: 64 frams
									0x02: 128 frams
									0x03: 256 frams
									0x04: 3 frams
									0x05: 4 frams
									0x06: 25 fram
									0x07: 2 fram */
	OLED_WR_Byte(0x07, OLED_CMD); //终止页 1
	OLED_WR_Byte(0x01, OLED_CMD); /*垂直滚动偏移量
									0x01到0*3F 对应 1到63*/
	OLED_WR_Byte(0x2F, OLED_CMD); //开启滚动
}



/**
 * @brief	初始化OLED
 * @param	无
 * @retval	无
 */
void OLED_Init(void)
{
#if OLED_AGREEMENT_MODE == 0 || OLED_AGREEMENT_MODE==1	
	OLED_RST_Clr();			//RES拉低
	OLED_RST_Set();			//RES拉高
	OLED_DC_Clr(); 			// DC拉低
#endif


	OLED_WR_Byte(0xAE, OLED_CMD);//--设置显示关

	OLED_WR_Byte(0x00, OLED_CMD); //---设置列低位地址
	OLED_WR_Byte(0x10, OLED_CMD); //---设置列高位地址

	OLED_WR_Byte(0x40, OLED_CMD); //--设置RAM显示起始线

	OLED_WR_Byte(0x81, OLED_CMD); //--设置对比度
	OLED_WR_Byte(0xFF, OLED_CMD); //--对比度数值

	OLED_WR_Byte(0xA1, OLED_CMD); //--设置屏幕水平方向     0xa0左右反置 0xa1正常
	OLED_WR_Byte(0xC8, OLED_CMD); //--设置屏幕竖直方向   0xc0上下反置 0xc8正常

	OLED_WR_Byte(0xA6, OLED_CMD); //--设置正常显示		0xA7为翻转显示

	OLED_WR_Byte(0xA8, OLED_CMD); //--设置多路复用率(16 to 64)
	OLED_WR_Byte(0x3F, OLED_CMD); //--1/64 duty

	OLED_WR_Byte(0xD3, OLED_CMD); //-设置显示偏移 (0x00~0x3F)
	OLED_WR_Byte(0x00, OLED_CMD); //-偏移数值

	OLED_WR_Byte(0xD5, OLED_CMD); //--设置显示时钟分频比/振荡器频率
	OLED_WR_Byte(0x80, OLED_CMD); //--设置分频比，将时钟设置为100帧/秒

	OLED_WR_Byte(0xD9, OLED_CMD); //--设置预充电周期
	OLED_WR_Byte(0xF1, OLED_CMD); // 将预充电设置为15个时钟，放电设置为1个时钟

	OLED_WR_Byte(0xDA, OLED_CMD); //--设置com引脚硬件配置
	OLED_WR_Byte(0x12, OLED_CMD); //--

	OLED_WR_Byte(0xDB, OLED_CMD); //--设置VCOMH取消选择等级
	OLED_WR_Byte(0x40, OLED_CMD); //--Set VCOM Deselect Level

	OLED_WR_Byte(0x8D, OLED_CMD); //--set Charge Pump enable/disable
	OLED_WR_Byte(0x14, OLED_CMD); //--set(0x10) disable

	OLED_WR_Byte(0x20, OLED_CMD); //--设置内存选址模式 (0x00/0x01/0x02)
	OLED_WR_Byte(0x02, OLED_CMD); //--页面选址模式

	OLED_WR_Byte(0xA4, OLED_CMD); // 重新显示RAM内容，即复位 (0xa4/0xa5)
	OLED_WR_Byte(0xAF, OLED_CMD); //--设置显示开启

	OLED_Clear();
	OLED_Set_Pos(0, 0);
}

