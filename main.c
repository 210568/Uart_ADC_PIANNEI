
/*
*项目实现目标：利用ADC模块测量片内温度
*项目编写时间：2022年3月23日
*项目编写人：李俊辉
*/

#include "ioCC2530.h"
#include "string.h"
#include "stdlib.h"

char data[] = "ceshiCC2530片内温度传感器！\n";          //功能提示字符串
unsigned int ADCvalue;                          //原始温度数据
float temper;                                   //转换后的温度数据

/*
*函数名称：delay(unsigned int time)
*函数功能：延迟函数
*函数入口：void delay(unsigned int time)
*函数形参：无
*函数返回值：无
*/
void delay(unsigned int time)
{
  for (unsigned int i=0; i<time; i++)
    for (unsigned char j=0; j<240; j++)
    {
      asm("NOP");
      asm("NOP");
      asm("NOP");
    }
}

/*
*函数名称：Initial_usart()
*函数功能：初始化串口
*函数入口：void Initial_usart()
*函数形参：无
*函数返回值：无
*/
void Initial_usart()
{
  CLKCONCMD &= ~0x7F;                   //晶振设置为32Mhz
  while(CLKCONSTA & 0x40);               //等待晶振稳定
  CLKCONCMD &= ~0x47;                   //设置系统主时钟频率为32Mhz
  PERCFG = 0x00;                        //usert0 使用备用位置 1 TX-P0.3 RX-P0.2
  P0SEL |= 0x3C;                ////P0.2 P0.3 P0.4 P0.5 用户外设功能
  P2DIR &= ~0xC0;                //P0优先作为UART方式
  U0CSR |= 0xC0;                        //uart模式 允许接收
  U0GCR = 9;
  U0BAUD = 59;                  //波特率设为 19200
  URX0IF = 0;                    //uart0 RX 中断标志位清零
}

/*
*串口发送数据函数
*/
void Uart_Tx_String(char * data_tx,int len)
{
  for (unsigned int j=0; j<len; j++)
  {
    U0DBUF = *data_tx++;
    while(UTX0IF == 0);
    UTX0IF = 0;
  }
}

/*
*函数名称：
*/
void main(void)
{
  Initial_usart();                      //调用UART初始化函数
  uart_tx_string(data,sizeof(data));                    //发送数据
  
  
  while(1)
  {
   //启动AD转换
    ADCCON3 |= 0x3E;            //内部1.25v为参考电压
    
    //等待AD转换完成
    while((ADCCON1 & 0x80) == 0);                //ADCCON1.EOC 为0b1
   
    //获取AD转换结果
    ADCvalue = ADCvalue >> 2;                   //ADCL低两位是无效的
    temper = ADCvalue * 0.06229 -303.0 - 35;            //温度值转换公式 ，35位偏差调整
    
    //浮点数不能直接通过串口发送
    //温度数据转换为字符串发送到 PC 
    data[0] = (unsigned char)(temper)/10 +48;          //十位
    data[1] = (unsigned char)(temper)%10 + 48;         //个位
    data[2] = '.';                                     //小数点
    data[3] = (unsigned char)(temper * 10)%10 + 48;      //十分位
    data[4] = (unsigned char)(temper * 100)%10 + 48;     //百分位
    
    //温度值通过串口通信发送
    Uart_Tx_String(data, 5);
    Uart_Tx_String("℃\n",3);
    
    //延时
    delay(10000);
    
    
  }
}