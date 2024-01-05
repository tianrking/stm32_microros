// //              GND   电源地
// //              VCC   3.3v电源
// //              SCL   PA5（SCLK）
// //              SDA   PA7（MOSI）
// //              RES   PB0
// //              DC    PB2
// //              CS    PB1

// #include "include.h" // 确保包含了正确的头文件

// // 假设hspi1是你的SPI句柄
// extern SPI_HandleTypeDef hspi1;

// void LCD_Writ_Bus(uint8_t data) 
// {
//     LCD_CS_Clr();  // 选择LCD
//     HAL_SPI_Transmit(&hspi1, &data, 1, HAL_MAX_DELAY);  // 发送一个字节的数据
//     LCD_CS_Set();  // 取消选择LCD
// }

// void LCD_WR_REG(uint8_t reg)
// {
//     LCD_DC_Clr(); // 清除DC表示接下来是命令
//     LCD_Writ_Bus(reg); // 写入命令
//     LCD_DC_Set(); // 设置DC表示接下来是数据
// }

// void LCD_WR_DATA8(uint8_t data)
// {
//     LCD_Writ_Bus(data); // 直接写入数据，假设DC已经正确设置
// }

// void LCD_WR_DATA(uint16_t data)
// {
//     LCD_WR_DATA8(data >> 8); // 先发送数据的高字节
//     LCD_WR_DATA8(data & 0xFF); // 再发送数据的低字节
// }

// void LCD_Address_Set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
// {
// 	LCD_WR_REG(0x2a);//列地址设置
// 	LCD_WR_DATA(x1);
// 	LCD_WR_DATA(x2);
// 	LCD_WR_REG(0x2b);//行地址设置
// 	LCD_WR_DATA(y1);
// 	LCD_WR_DATA(y2);
// 	LCD_WR_REG(0x2c);//储存器写
// }

// void LCD_Init(void)
// {
// 	//SPI1_Init();
// 	//LCD_GPIO_Init();//初始化GPIO
	
// 	LCD_RES_Clr();//复位
// 	osDelay(100);
// 	LCD_RES_Set();
// 	osDelay(100);
	
// 	//LCD_BLK_Set();//打开背光
//   osDelay(100);
	
// 	LCD_WR_REG(0xEF);
// 	LCD_WR_REG(0xEB);
// 	LCD_WR_DATA8(0x14); 
	
//   LCD_WR_REG(0xFE);			 
// 	LCD_WR_REG(0xEF); 

// 	LCD_WR_REG(0xEB);	
// 	LCD_WR_DATA8(0x14); 

// 	LCD_WR_REG(0x84);			
// 	LCD_WR_DATA8(0x40); 

// 	LCD_WR_REG(0x85);			
// 	LCD_WR_DATA8(0xFF); 

// 	LCD_WR_REG(0x86);			
// 	LCD_WR_DATA8(0xFF); 

// 	LCD_WR_REG(0x87);			
// 	LCD_WR_DATA8(0xFF);

// 	LCD_WR_REG(0x88);			
// 	LCD_WR_DATA8(0x0A);

// 	LCD_WR_REG(0x89);			
// 	LCD_WR_DATA8(0x21); 

// 	LCD_WR_REG(0x8A);			
// 	LCD_WR_DATA8(0x00); 

// 	LCD_WR_REG(0x8B);			
// 	LCD_WR_DATA8(0x80); 

// 	LCD_WR_REG(0x8C);			
// 	LCD_WR_DATA8(0x01); 

// 	LCD_WR_REG(0x8D);			
// 	LCD_WR_DATA8(0x01); 

// 	LCD_WR_REG(0x8E);			
// 	LCD_WR_DATA8(0xFF); 

// 	LCD_WR_REG(0x8F);			
// 	LCD_WR_DATA8(0xFF); 


// 	LCD_WR_REG(0xB6);
// 	LCD_WR_DATA8(0x00);
// 	LCD_WR_DATA8(0x20);

// 	LCD_WR_REG(0x36);
// 	if(USE_HORIZONTAL==0)LCD_WR_DATA8(0x08);
// 	else if(USE_HORIZONTAL==1)LCD_WR_DATA8(0xC8);
// 	else if(USE_HORIZONTAL==2)LCD_WR_DATA8(0x68);
// 	else LCD_WR_DATA8(0xA8);

// 	LCD_WR_REG(0x3A);			
// 	LCD_WR_DATA8(0x05); 


// 	LCD_WR_REG(0x90);			
// 	LCD_WR_DATA8(0x08);
// 	LCD_WR_DATA8(0x08);
// 	LCD_WR_DATA8(0x08);
// 	LCD_WR_DATA8(0x08); 

// 	LCD_WR_REG(0xBD);			
// 	LCD_WR_DATA8(0x06);
	
// 	LCD_WR_REG(0xBC);			
// 	LCD_WR_DATA8(0x00);	

// 	LCD_WR_REG(0xFF);			
// 	LCD_WR_DATA8(0x60);
// 	LCD_WR_DATA8(0x01);
// 	LCD_WR_DATA8(0x04);

// 	LCD_WR_REG(0xC3);			
// 	LCD_WR_DATA8(0x13);
// 	LCD_WR_REG(0xC4);			
// 	LCD_WR_DATA8(0x13);

// 	LCD_WR_REG(0xC9);			
// 	LCD_WR_DATA8(0x22);

// 	LCD_WR_REG(0xBE);			
// 	LCD_WR_DATA8(0x11); 

// 	LCD_WR_REG(0xE1);			
// 	LCD_WR_DATA8(0x10);
// 	LCD_WR_DATA8(0x0E);

// 	LCD_WR_REG(0xDF);			
// 	LCD_WR_DATA8(0x21);
// 	LCD_WR_DATA8(0x0c);
// 	LCD_WR_DATA8(0x02);

// 	LCD_WR_REG(0xF0);   
// 	LCD_WR_DATA8(0x45);
// 	LCD_WR_DATA8(0x09);
// 	LCD_WR_DATA8(0x08);
// 	LCD_WR_DATA8(0x08);
// 	LCD_WR_DATA8(0x26);
//  	LCD_WR_DATA8(0x2A);

//  	LCD_WR_REG(0xF1);    
//  	LCD_WR_DATA8(0x43);
//  	LCD_WR_DATA8(0x70);
//  	LCD_WR_DATA8(0x72);
//  	LCD_WR_DATA8(0x36);
//  	LCD_WR_DATA8(0x37);  
//  	LCD_WR_DATA8(0x6F);


//  	LCD_WR_REG(0xF2);   
//  	LCD_WR_DATA8(0x45);
//  	LCD_WR_DATA8(0x09);
//  	LCD_WR_DATA8(0x08);
//  	LCD_WR_DATA8(0x08);
//  	LCD_WR_DATA8(0x26);
//  	LCD_WR_DATA8(0x2A);

//  	LCD_WR_REG(0xF3);   
//  	LCD_WR_DATA8(0x43);
//  	LCD_WR_DATA8(0x70);
//  	LCD_WR_DATA8(0x72);
//  	LCD_WR_DATA8(0x36);
//  	LCD_WR_DATA8(0x37); 
//  	LCD_WR_DATA8(0x6F);

// 	LCD_WR_REG(0xED);	
// 	LCD_WR_DATA8(0x1B); 
// 	LCD_WR_DATA8(0x0B); 

// 	LCD_WR_REG(0xAE);			
// 	LCD_WR_DATA8(0x77);
	
// 	LCD_WR_REG(0xCD);			
// 	LCD_WR_DATA8(0x63);		


// 	LCD_WR_REG(0x70);			
// 	LCD_WR_DATA8(0x07);
// 	LCD_WR_DATA8(0x07);
// 	LCD_WR_DATA8(0x04);
// 	LCD_WR_DATA8(0x0E); 
// 	LCD_WR_DATA8(0x0F); 
// 	LCD_WR_DATA8(0x09);
// 	LCD_WR_DATA8(0x07);
// 	LCD_WR_DATA8(0x08);
// 	LCD_WR_DATA8(0x03);

// 	LCD_WR_REG(0xE8);			
// 	LCD_WR_DATA8(0x34);

// 	LCD_WR_REG(0x62);			
// 	LCD_WR_DATA8(0x18);
// 	LCD_WR_DATA8(0x0D);
// 	LCD_WR_DATA8(0x71);
// 	LCD_WR_DATA8(0xED);
// 	LCD_WR_DATA8(0x70); 
// 	LCD_WR_DATA8(0x70);
// 	LCD_WR_DATA8(0x18);
// 	LCD_WR_DATA8(0x0F);
// 	LCD_WR_DATA8(0x71);
// 	LCD_WR_DATA8(0xEF);
// 	LCD_WR_DATA8(0x70); 
// 	LCD_WR_DATA8(0x70);

// 	LCD_WR_REG(0x63);			
// 	LCD_WR_DATA8(0x18);
// 	LCD_WR_DATA8(0x11);
// 	LCD_WR_DATA8(0x71);
// 	LCD_WR_DATA8(0xF1);
// 	LCD_WR_DATA8(0x70); 
// 	LCD_WR_DATA8(0x70);
// 	LCD_WR_DATA8(0x18);
// 	LCD_WR_DATA8(0x13);
// 	LCD_WR_DATA8(0x71);
// 	LCD_WR_DATA8(0xF3);
// 	LCD_WR_DATA8(0x70); 
// 	LCD_WR_DATA8(0x70);

// 	LCD_WR_REG(0x64);			
// 	LCD_WR_DATA8(0x28);
// 	LCD_WR_DATA8(0x29);
// 	LCD_WR_DATA8(0xF1);
// 	LCD_WR_DATA8(0x01);
// 	LCD_WR_DATA8(0xF1);
// 	LCD_WR_DATA8(0x00);
// 	LCD_WR_DATA8(0x07);

// 	LCD_WR_REG(0x66);			
// 	LCD_WR_DATA8(0x3C);
// 	LCD_WR_DATA8(0x00);
// 	LCD_WR_DATA8(0xCD);
// 	LCD_WR_DATA8(0x67);
// 	LCD_WR_DATA8(0x45);
// 	LCD_WR_DATA8(0x45);
// 	LCD_WR_DATA8(0x10);
// 	LCD_WR_DATA8(0x00);
// 	LCD_WR_DATA8(0x00);
// 	LCD_WR_DATA8(0x00);

// 	LCD_WR_REG(0x67);			
// 	LCD_WR_DATA8(0x00);
// 	LCD_WR_DATA8(0x3C);
// 	LCD_WR_DATA8(0x00);
// 	LCD_WR_DATA8(0x00);
// 	LCD_WR_DATA8(0x00);
// 	LCD_WR_DATA8(0x01);
// 	LCD_WR_DATA8(0x54);
// 	LCD_WR_DATA8(0x10);
// 	LCD_WR_DATA8(0x32);
// 	LCD_WR_DATA8(0x98);

// 	LCD_WR_REG(0x74);			
// 	LCD_WR_DATA8(0x10);	
// 	LCD_WR_DATA8(0x85);	
// 	LCD_WR_DATA8(0x80);
// 	LCD_WR_DATA8(0x00); 
// 	LCD_WR_DATA8(0x00); 
// 	LCD_WR_DATA8(0x4E);
// 	LCD_WR_DATA8(0x00);					
	
//   LCD_WR_REG(0x98);			
// 	LCD_WR_DATA8(0x3e);
// 	LCD_WR_DATA8(0x07);

// 	LCD_WR_REG(0x35);	
// 	LCD_WR_REG(0x21);

// 	LCD_WR_REG(0x11);
// 	osDelay(120);
// 	LCD_WR_REG(0x29);
// 	osDelay(20);
// } 



