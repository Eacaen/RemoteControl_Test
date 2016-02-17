 #include "adc.h"
 #include "delay.h"
 #include "usart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//ADC ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/7
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
	   
		   
//��ʼ��ADC
//�������ǽ��Թ���ͨ��Ϊ��
//����Ĭ�Ͻ�����ͨ��0~3																	   
void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1|RCC_APB2Periph_GPIOC	, ENABLE );	  //ʹ��ADC1ͨ��ʱ��
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M

	//PA1 ��Ϊģ��ͨ����������                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
	GPIO_Init(GPIOC, &GPIO_InitStructure);		

	ADC_DeInit(ADC1);  //��λADC1,������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ת��������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 4;	//˳����й���ת����ADCͨ������Ŀ
	ADC_Init(ADC1, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���   
	
//  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5 );	//���ò���ʱ��Ϊ239.5����
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_239Cycles5 );	//���ò���ʱ��Ϊ239.5����
  ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 2, ADC_SampleTime_239Cycles5 );	//���ò���ʱ��Ϊ239.5����
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_239Cycles5 );	//���ò���ʱ��Ϊ239.5����
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_239Cycles5 );	//���ò���ʱ��Ϊ239.5����
  ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 4, ADC_SampleTime_239Cycles5 );	//���ò���ʱ��Ϊ239.5����

	ADC_Cmd(ADC1, ENABLE);	//ʹ��ָ����ADC1
	ADC_ResetCalibration(ADC1);	//ʹ�ܸ�λУ׼   
	while(ADC_GetResetCalibrationStatus(ADC1));	//�ȴ���λУ׼����
	ADC_StartCalibration(ADC1);	 //����ADУ׼
	while(ADC_GetCalibrationStatus(ADC1));	 //�ȴ�У׼����
 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������	

}

//���ADCֵ
//ch:ͨ��ֵ 0~3
u16 Get_Adc(u8 ch)   
{
	//����ת������	  		 
	ADC1->SQR3&=0XFFFFFFE0;//��������1 ͨ��ch
	ADC1->SQR3|=ch;		  			    
	ADC1->CR2|=1<<22;       //��������ת��ͨ�� 
	while(!(ADC1->SR&1<<1));//�ȴ�ת������	 	   
	return ADC1->DR;		//����adcֵ	
}	 
/*------------------------------------------------- 
���ܣ���λ���˲�������Nֵ�ɸ���ʵ��������� 
      �������ð�ݷ�
�������˲�ǰADֵ
���أ��˲����ADֵ
--------------------------------------------------*/ 
#define N 20	
u16 Filter(u8 ch)  
{  
	   u16 value;
	   u16 value_buf[N];  
	   u16 count,i,j,temp;  
	   for ( count=0;count<N;count++)  
	      {  
	   		  value=Get_Adc(ch);
					delay_ms(1);
					value_buf[count] = value; 
	      } 
	   for (j=0;j<N-1;j++)  
	       {  
	          for (i=0;i<N-j;i++)  
	              {  
	                 if ( value_buf[i]>value_buf[i+1] )  
	                    {  
			              temp = value_buf[i];  
			              value_buf[i] = value_buf[i+1];   
			              value_buf[i+1] = temp;  
			            }  
			      }  
			} 
	   temp=0;
	   for(i=0;i<5;i++)
	   {
	   		temp=temp+value_buf[N/2-i]+value_buf[N/2+i];	
	   } 
	   return temp/10;
}



u16 ADC_Origin[4];	
double Origin_voltage[4];
void Origin_Find()
{
  static u8 x,m;
	static double sum_avg[4]={0};
  printf("FIND ORIGIN\r\n");
  for(x=0;x<4;x++)
	{
			for(m=0;m<10;m++)
			{
				ADC_Origin[x] = Filter(x+1);
				sum_avg[x] += ADC_Origin[x]*3.3/4096;
		  }
		Origin_voltage[x] = (double)sum_avg[x]/10.0;
		sum_avg[x] = 0;
	}
	  for(x=0;x<4;x++)
	{
			for(m=0;m<10;m++)
			{
				ADC_Origin[x] = Filter(x+10);
				sum_avg[x] += ADC_Origin[x]*3.3/4096;
		  }
		Origin_voltage[x] = (double)sum_avg[x]/10.0;
		sum_avg[x] = 0;
	}
}


























