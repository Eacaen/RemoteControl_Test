 #include "adc.h"
 #include "delay.h"
 #include "usart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//ADC 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/7
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
	   
		   
//初始化ADC
//这里我们仅以规则通道为例
//我们默认将开启通道0~3																	   
void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1|RCC_APB2Periph_GPIOC	, ENABLE );	  //使能ADC1通道时钟
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M

	//PA1 作为模拟通道输入引脚                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
	GPIO_Init(GPIOC, &GPIO_InitStructure);		

	ADC_DeInit(ADC1);  //复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 4;	//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器   
	
//  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5 );	//配置采样时间为239.5周期
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_239Cycles5 );	//配置采样时间为239.5周期
  ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 2, ADC_SampleTime_239Cycles5 );	//配置采样时间为239.5周期
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_239Cycles5 );	//配置采样时间为239.5周期
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_239Cycles5 );	//配置采样时间为239.5周期
  ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 4, ADC_SampleTime_239Cycles5 );	//配置采样时间为239.5周期

	ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1
	ADC_ResetCalibration(ADC1);	//使能复位校准   
	while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束
	ADC_StartCalibration(ADC1);	 //开启AD校准
	while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束
 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能	

}

//获得ADC值
//ch:通道值 0~3
u16 Get_Adc(u8 ch)   
{
	//设置转换序列	  		 
	ADC1->SQR3&=0XFFFFFFE0;//规则序列1 通道ch
	ADC1->SQR3|=ch;		  			    
	ADC1->CR2|=1<<22;       //启动规则转换通道 
	while(!(ADC1->SR&1<<1));//等待转换结束	 	   
	return ADC1->DR;		//返回adc值	
}	 
/*------------------------------------------------- 
功能：中位数滤波函数，N值可根据实际情况调整 
      排序采用冒泡法
参数：滤波前AD值
返回：滤波后的AD值
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
int Origin_voltage[4];
void Origin_Find()
{
  static u8 x,m;
	static double sum_avg[4]={0};
  printf("FIND ORIGIN\r\n");
  for(x=0;x<2;x++)
	{
			for(m=0;m<10;m++)
			{
				ADC_Origin[x] = Filter(x+1);
				sum_avg[x] += ADC_Origin[x];
		  }
		Origin_voltage[x] = (double)sum_avg[x]/10.0;
		sum_avg[x] = 0;
	}
	  for(x=2;x<4;x++)
	{
			for(m=0;m<10;m++)
			{
				ADC_Origin[x] = Filter(x+10);
				sum_avg[x] += ADC_Origin[x];
		  }
		Origin_voltage[x] = sum_avg[x]/10.0;
		sum_avg[x] = 0;
	}
}


























