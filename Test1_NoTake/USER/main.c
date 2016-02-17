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
double ADC_voltage[4];

void deal_data(double *voltage);

extern u16 ADC_Origin[4];	
extern double Origin_voltage[4];
u8 send_data[20];

 int main(void)
 { 
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(9600);	 	//���ڳ�ʼ��Ϊ9600
 	LED_Init();			     //LED�˿ڳ�ʼ��
 	Adc_Init();		  		//ADC��ʼ��
	KEY_Init(); 
	 
	NRF24L01_Init();    	//��ʼ��NRF24L01 
	
	while(NRF24L01_Check()== 1)	//���NRF24L01�Ƿ���λ.	
	{
		LED0=!LED0;
 		delay_ms(200);
		printf("NRF eror\r\n");
	}
	printf("NRF OK\r\n");
	NRF24L01_TX_Mode();
	
	Origin_Find();
       
	while(1)
	{

		send_data[0]=0xff;
		send_data[1]=0xff;		
	
		for(i=0;i<4;i++)			//PA0  PA1  PC2 PC3
			{
				ADC_value[i] = Filter(i);
				ADC_voltage[i] = ADC_value[i]*900/4096 - Origin_voltage[i];
				
				ADC_OK=1;
				printf("  %d - %.3f",i,ADC_voltage[i]);
			}
		deal_data(ADC_voltage);
		printf("\r\n");
	
			
			
	if(ADC_OK )
	 {	
 			 NRF24L01_TX_Mode();
					if(NRF24L01_TxPacket(send_data)==TX_OK)
				{	
				NRF24L01_Write_Reg(FLUSH_TX,0xff);//���TX FIFO�Ĵ��� 
				}
			ADC_OK = 0;

	 }
	}
 }
 
 
 void deal_data(double *voltage)
 {
		send_data[2] = (u16)voltage[0]/256;
		send_data[3] = (u16)voltage[0]%256;
	 
	  send_data[4] = (u16)voltage[1]/256;
		send_data[5] = (u16)voltage[1]%256;
	 
	  send_data[6] = (u16)voltage[2]/256;
		send_data[7] = (u16)voltage[2]%256;
	 
	  send_data[8] = (u16)voltage[3]/256;
		send_data[9] = (u16)voltage[3]%256;
 }

