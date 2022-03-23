
/*
*��Ŀʵ��Ŀ�꣺����ADCģ�����Ƭ���¶�
*��Ŀ��дʱ�䣺2022��3��23��
*��Ŀ��д�ˣ����
*/

#include "ioCC2530.h"
#include "string.h"
#include "stdlib.h"

char data[] = "ceshiCC2530Ƭ���¶ȴ�������\n";          //������ʾ�ַ���
unsigned int ADCvalue;                          //ԭʼ�¶�����
float temper;                                   //ת������¶�����

/*
*�������ƣ�delay(unsigned int time)
*�������ܣ��ӳٺ���
*������ڣ�void delay(unsigned int time)
*�����βΣ���
*��������ֵ����
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
*�������ƣ�Initial_usart()
*�������ܣ���ʼ������
*������ڣ�void Initial_usart()
*�����βΣ���
*��������ֵ����
*/
void Initial_usart()
{
  CLKCONCMD &= ~0x7F;                   //��������Ϊ32Mhz
  while(CLKCONSTA & 0x40);               //�ȴ������ȶ�
  CLKCONCMD &= ~0x47;                   //����ϵͳ��ʱ��Ƶ��Ϊ32Mhz
  PERCFG = 0x00;                        //usert0 ʹ�ñ���λ�� 1 TX-P0.3 RX-P0.2
  P0SEL |= 0x3C;                ////P0.2 P0.3 P0.4 P0.5 �û����蹦��
  P2DIR &= ~0xC0;                //P0������ΪUART��ʽ
  U0CSR |= 0xC0;                        //uartģʽ �������
  U0GCR = 9;
  U0BAUD = 59;                  //��������Ϊ 19200
  URX0IF = 0;                    //uart0 RX �жϱ�־λ����
}

/*
*���ڷ������ݺ���
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
*�������ƣ�
*/
void main(void)
{
  Initial_usart();                      //����UART��ʼ������
  uart_tx_string(data,sizeof(data));                    //��������
  
  
  while(1)
  {
   //����ADת��
    ADCCON3 |= 0x3E;            //�ڲ�1.25vΪ�ο���ѹ
    
    //�ȴ�ADת�����
    while((ADCCON1 & 0x80) == 0);                //ADCCON1.EOC Ϊ0b1
   
    //��ȡADת�����
    ADCvalue = ADCvalue >> 2;                   //ADCL����λ����Ч��
    temper = ADCvalue * 0.06229 -303.0 - 35;            //�¶�ֵת����ʽ ��35λƫ�����
    
    //����������ֱ��ͨ�����ڷ���
    //�¶�����ת��Ϊ�ַ������͵� PC 
    data[0] = (unsigned char)(temper)/10 +48;          //ʮλ
    data[1] = (unsigned char)(temper)%10 + 48;         //��λ
    data[2] = '.';                                     //С����
    data[3] = (unsigned char)(temper * 10)%10 + 48;      //ʮ��λ
    data[4] = (unsigned char)(temper * 100)%10 + 48;     //�ٷ�λ
    
    //�¶�ֵͨ������ͨ�ŷ���
    Uart_Tx_String(data, 5);
    Uart_Tx_String("��\n",3);
    
    //��ʱ
    delay(10000);
    
    
  }
}