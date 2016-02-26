#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "adc.h"
#include "24l01.h" 	 


u8 i,t,ADC_OK=0;
u16 ADC_value[4];
int ADC_voltage[4];

void deal_data(int *voltage);
extern u16 ADC_Origin[4];	
extern int Origin_voltage[4];
u8 send_data[10];

 int main(void)
 {
	delay_init();	    	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(9600);	 	//串口初始化为9600
 	LED_Init();			     //LED端口初始化
 	Adc_Init();		  		//ADC初始化
	KEY_Init(); 
	 
	NRF24L01_Init();    	//初始化NRF24L01 
	
	while(NRF24L01_Check()== 1)	//检查NRF24L01是否在位.	
	{
		LED0=!LED0;
 		delay_ms(200);
		printf("NRF eror\r\n");
	}
	printf("NRF OK\r\n");
	NRF24L01_TX_Mode();
	
// 	Origin_Find();
       
	while(1)
	{

		send_data[0]=0xff;
		send_data[1]=0xff;		
		for(i=0;i<2;i++)			//PA0  PA1
			{
				ADC_value[i] = Filter(i+1);
				ADC_voltage[i] = ADC_value[i] - Origin_voltage[i];
				
				ADC_OK=1;
// 				printf("  %d - %d",i,ADC_voltage[i]);
			}
					for(i=2;i<4;i++)			//PA0  PA1
			{
				ADC_value[i] = Filter(i+10);
				ADC_voltage[i] = ADC_value[i]- Origin_voltage[i];
				
				ADC_OK=1;
				printf("  %d - %d",i,ADC_voltage[i]);
			}
				printf("\r\n");
		deal_data(ADC_voltage);
	
	
			
			
	if(ADC_OK )
	 {	
 			 NRF24L01_TX_Mode();
					if(NRF24L01_TxPacket(send_data)==TX_OK)
				{	
				NRF24L01_Write_Reg(FLUSH_TX,0xff);//清除TX FIFO寄存器 
				}
			ADC_OK = 0;

	 }
	}
 }
 
/*************************************************
 0.										3.
            
4095<---1990--->0			1<---2073--->4093


1.  4095              4.	 1
	    |										 |
      |                    |
		2083									2116
 			|										 |
      |										 |
		  1										4092						
*************************************************/
 void deal_data(int *voltage)
 {
// 	 u16 comb;
		send_data[2] = (u16)voltage[0]/256;
		send_data[3] = (u16)voltage[0]%256;
	 
	  send_data[4] = (u16)voltage[1]/256;
		send_data[5] = (u16)voltage[1]%256;
	 
	  send_data[6] = (u16)voltage[2]/256;
		send_data[7] = (u16)voltage[2]%256;
	 
	  send_data[8] = (u16)voltage[3]/256;
		send_data[9] = (u16)voltage[3]%256;
	 
	 printf("%x  %x \r\n",send_data[8],send_data[9]);
	 
// 	 comb = send_data[8]<<8 | send_data[9];
// 	 printf("%d\r\n",comb);
 }

