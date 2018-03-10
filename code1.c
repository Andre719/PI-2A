//������ʱ�Ӳ����ڲ�RC������     DCO��8MHz,��CPUʱ��;  SMCLK��1MHz,����ʱ��ʱ��
#include <msp430g2553.h>
#include <tm1638.h>  //��TM1638�йصı���������������ڸ�H�ļ���

//[3.1]��������������Ƶ�ƽ��4·GPIO���Ų�������Ƽ�ʹ������P1.0��P1.3
#define CTL0_L  P1OUT&=~BIT0
#define CTL0_H  P1OUT|=BIT0
#define CTL1_L  P1OUT&=~BIT5
#define CTL1_H  P1OUT|=BIT5
#define CTL2_L  P1OUT&=~BIT2
#define CTL2_H  P1OUT|=BIT2
#define CTL3_L  P1OUT&=~BIT3
#define CTL3_H  P1OUT|=BIT3
//�������������P1.7
#define INFRARED  (P1IN&BIT7)

//////////////////////////////
//         ��������         //
//////////////////////////////

// 0.1s�����ʱ�����ֵ��5��20ms
#define V_T100ms	5
// 0.5s�����ʱ�����ֵ��25��20ms
#define V_T500ms	25

//[1.1]����ȼ�����
#define GAIN_STATENUM    15

//����
const unsigned int music_data0[][2]=        //���줫�ʤ�����
{
    {659,400},{622,400},{494,600},{554,1000},{0,1200},{554,200},{622,200},{659,600},{0,80},{659,400},{0,80},{659,200},{740,200},{622,600},{0,400},  //15
	{659,400},{622,400},{494,600},{554,1000},{0,1200},{659,200},{740,200},{831,1000},{880,200},{831,200},{740,600},{0,400},   //12
	{659,400},{622,400},{494,600},{554,1000},{0,1200},{554,200},{622,200},{659,600},{0,80},{659,400},{0,80},{659,200},{740,200},{622,600},{0,400},  //15
	{659,400},{622,400},{494,600},{554,1000},{0,1200},{659,200},{740,200},{831,1000},{880,200},{831,200},{740,600},{0,600},   //12
	{659,400},{622,400},{659,400},{622,600},{494,200},{0,40},{494,600},{0,200},{494,400},{740,400},{622,400},{740,400},{0,40},{740,600},{659,200},{0,40},{659,600},{0,400},  //18
    {740,200},{0,40},{740,400},{831,200},{880,400},{0,40},{880,600},{831,600},{740,400},{831,1600},{0,600},   // 11
	{740,600},{831,600},{659,400},{740,600},{831,600},{988,400},{740,1000},{0,80},{622,200},{494,200},{622,200},{659,200},{740,200},{659,800},{0,400},   //15
	{740,600},{831,600},{659,400},{740,600},{831,600},{988,400},{740,1000},{0,80},{659,200},{622,200},{659,800},{0,400},    //12
	{544,400},{622,200},{659,200},{544,200},{0,40},{740,600},{831,600},{659,400},{740,600},{831,600},{988,400},{740,1000},{0,80},{622,200},{494,200},{622,200},{659,200},{740,200},{659,800},{0,400},    //20
	{659,400},{622,200},{659,200},{740,800},{0,80},{831,600},{988,400},{740,600},{831,1000},{0,600},  //10
	{659,400},{622,200},{659,200},{740,800},{0,80},{659,200},{622,200},{554,1600},   //8
	{0,3000},{0,0}
};
const unsigned int music_data1[][2]=        //��������ɫ��
{
	{523,600},{784,200},{523,200},{784,200},{523,200},{587,200},{659,1600},
	{523,600},{784,200},{523,200},{784,200},{523,200},{587,200},{587,1600},
	{523,600},{784,200},{523,200},{784,200}, {587,200} ,{523,200},{440,1000},{392,200}, {523,200},{587,200},
	{523,600},{784,200},{523,200},{784,200},{523,200},{440,200},{523,1600},
	{523,200},{523,400},{440,200},{392,400},{440,400},
	{523,400},{523,200},{587,200},{659,800},
	{587,200},{587,400},{523,200},{587,400},{587,200},{784,200},
	{784,200},{659,200},{659,200},{587,200},{659,800},
	{523,200},{523,400},{440,200},{392,400},{784,400},
	{659,200},{587,200}, {659,200},{587,200},{523,800},
	{587,200}, {587,400}, {523,200}, {587,200}, {587,400},{659,200},
	{587,200},{523,200},{440,200}, {587,200},{523,800},
	{523,200},{523,400},{440,200},{392,400},{440,400},
	{523,200},{523,400},{587,200},{659,800},
	{587,200},{587,400},{523,200},{587,400},{587,200},{784,200},
	{784,200},{659,200},{659,200},{587,200},{659,800},
	{523,200},{523,200},{523,200},{440,200},{392,400},{784,400},
	{659,200},{587,200}, {659,200},{587,200},{523,800},
	{587,200},{587,400}, {523,200}, {587,200}, {587,400},{659,200},
	{587,200},{523,200},{440,200}, {587,200},{523,800},
	{659,200},{784,400}, {784,200}, {784,400}, {784,400},
	{880,200},{784,200},{659,200},{587,200},{523,800},
	{880,200},{1047,200},{880,200},{784,200},{659,200},{587,200},{523,200},{440,200},
	{587,400},{587,200},{659,200},{659,200},{587,600},
	{659,200},{784,400}, {784,200}, {784,400}, {784,400},
	{880,200},{784,200},{659,200},{587,200},{523,800},
	{440,200},{523,200},{440,200},{392,200},{587,400},{659,400},
	{523,1200},{0,400},{0,0}
  };
const unsigned int music_data2[][2]=		//����֮���ڡ�
{
	{1047,400},{988,400},{1047,400},{1319,400},{988,800},{0,800},{880,400},{784,400},
	{1760,400},{523,400},{784,800},{0,800},{698,400},{1319,400},{698,400},{523,400},{988,800},
	{784,800},{880,400},{988,400},{1047,400},{1319,400},{1175,800},{0,800},{1047,400},{988,400},
	{1047,400},{1319,400},{988,800},{784,800},{880,400},{988,400},{1047,400},{1175,400},{1319,800},
	{1319,800},{1397,400},{1319,400},{1175,400},{1047,400},{988,400},{1319,400},{0,0}
};
const unsigned int music_data3[][2]=		//������ɯ��
{
	{440,600},{494,200},{523,600},{440,200},
	{523,400},{494,200},{440,200},{494,400},
	{330,400},{494,600},{523,200},{578,600},
	{494,200},{578,400},{523,200},{494,200},{440,800},{659,400},{880,400},{784,400},
	{880,200},{784,200},{698,400},{659,200},
	{578,200},{659,400},{440,400},{0,200},
	{698,400},{578,200},{659,600},{523,200},
	{494,200},{330,200},{523,200},{494,200},
	{440,800},{659,400},{880,400},{784,400},
	{880,200},{784,200},{698,400},{659,200},
	{578,200},{659,400},{440,400},{0,200},
	{698,400},{578,200},{659,600},{523,200},
	{494,200},{330,200},{523,200},{494,200},
	{440,800},{0,0}
};
const unsigned int music_data4[][2] =       //��Canon��
{
	{784,400},{659,200},{698,200},{784,400},{659,200},{698,200},
	{784,200},{392,200},{440,200},{493,200},{523,200},{587,200},{659,200},{698,200},
	{659,400},{523,200},{587,200},{659,400},{330,200},{349,200},
	{392,200},{440,200},{392,200},{349,200},{392,200},{523,200},{493,200},{523,200},
	{440,400},{523,200},{493,200},{440,400},{392,200},{349,200},
	{392,200},{349,200},{330,200},{349,200},{392,200},{440,200},{493,200},{523,200},
	{440,400},{523,200},{493,200},{523,400},{493,200},{523,200},
	{493,200},{440,200},{493,200},{523,200},{587,200},{659,200},{698,200},{784,200},
	{784,400},{659,200},{698,200},{784,400},{659,200},{698,200},
	{784,200},{392,200},{440,200},{493,200},{523,200},{587,200},{659,200},{698,200},
	{659,400},{523,200},{587,200},{659,400},{330,200},{349,200},
	{392,200},{440,200},{392,200},{349,200},{392,200},{523,200},{493,200},{523,200},
	{440,400},{523,200},{493,200},{440,400},{392,200},{349,200},
	{392,200},{349,200},{330,200},{349,200},{392,200},{440,200},{493,200},{523,200},
	{440,400},{523,200},{493,200},{523,400},{493,200},{440,200},
	{493,200},{523,200},{587,200},{523,200},{493,200},{523,200},{440,200},{493,200},
	{523,1600},{0,0}
};

//////////////////////////////
//       ��������           //
//////////////////////////////

// �����ʱ������
unsigned char clock100ms=0;
unsigned char clock500ms=0;
// �����ʱ�������־
unsigned char clock100ms_flag=0;
unsigned char clock500ms_flag=0;
// �����ü�����
unsigned int test_counter=0;
// 8λ�������ʾ�����ֻ���ĸ����
// ע����������λ�������������Ϊ4��5��6��7��0��1��2��3
//[1.2]��4λ�̶���ʾ�����桱��Ӣ��GAIN,������3λ�̶���ʾ�����š�
unsigned char digit[8]={' ','-',' ',' ','G','A','I','N'};
// 8λС���� 1��  0��
// ע����������λС����������������Ϊ4��5��6��7��0��1��2��3
unsigned char pnt=0x04;
// 8��LEDָʾ��״̬��ÿ����4����ɫ״̬��0��1�̣�2�죬3�ȣ���+�̣�
// ע������ָʾ�ƴ������������Ϊ7��6��5��4��3��2��1��0
//     ��ӦԪ��LED8��LED7��LED6��LED5��LED4��LED3��LED2��LED1
//[1.3]LED����Ϊȫ��
unsigned char led[]={0,0,0,0,0,0,0,0};
//[1.4]����ȼ�ȡֵ����ֵΪ1����Ӧ����0.1
unsigned char gain_state=1;
//[2.1]���������밴�������йص�ȫ�ֱ���
unsigned char key_state=0,key_flag=1,key_code=0;
//�������ֲ��ű���
unsigned int const (*music_data)[2];
unsigned int audio_frequency=0,audio_dura=0,audio_ptr=0;
unsigned int music_num = 0;
unsigned int speed = 20;
//���Ӻ���ң�ر���
unsigned int infrared_state=0,infrared_pulsewidth=0,infrared_flag=0;
//�����Զ��������
unsigned char auto_control = 0;
int sample;
double volt;   //ADת������ֵ
double Vmax = 1.5;
double Vmin = 0.4;

//////////////////////////////
//       ϵͳ��ʼ��         //
//////////////////////////////

//ADC��ʼ���ӳ��������Զ�����
void ADC10_Init(void)
{
    ADC10CTL0 = SREF_1 + ADC10SHT_2 + REFON + REF2_5V+ ADC10ON;  //Vref+ =2.5V
    P1DIR &= ~BIT4;
    ADC10CTL1 = INCH_4 ;
    ADC10AE0 |= BIT4;
}

//  I/O�˿ں����ų�ʼ��
void Init_Ports(void)
{
	P2SEL &= ~(BIT7+BIT6);       //P2.6��P2.7 ����Ϊͨ��I/O�˿�
	  //������Ĭ�������⾧�񣬹�����޸�

	P2DIR |= BIT7 + BIT6 + BIT5; //P2.5��P2.6��P2.7 ����Ϊ���
	  //����·������������������ʾ�ͼ��̹�����TM1638������ԭ�������DATASHEET
	//[3.2]���Ӷ˿ڳ�ʼ����䣬��4·GPIO����P1.0��P1.5��P1.2��P1.3����Ϊ���
	P1DIR |= BIT0 + BIT5 + BIT2 +BIT3;
	//��ʼ��P2.1����Ϊ��ʱ��A1��PWM�������
	P2SEL|= BIT1;
	P2DIR|= BIT1;
	//��ʼ��P1.7��Ϊ����
	P1DIR &= ~BIT7;
 }

//  ��ʱ��TIMER0��ʼ����ѭ����ʱ20ms
void Init_Timer0(void)
{
	TA0CTL = TASSEL_2 + MC_1 ;      // Source: SMCLK=1MHz, UP mode,
	TA0CCR0 = 20000;                // 1MHzʱ��,����20000��Ϊ 20ms
	TA0CCTL0 = CCIE;                // TA0CCR0 interrupt enabled
}

//��ʼ��Timer1������440Hz�ķ����ź�
void Init_Timer1(void)
{
	TA1CTL = TASSEL_2 + MC_1;
	TA1CCTL1 = OUTMOD_7;
	TA1CCR0 = 1000000/440;          //�趨���ڣ�1000000Ϊ��ʱ��1ʱ��Ƶ�ʣ�440Ϊ��ƵƵ��
	TA1CCR1 = TA1CCR0/2;            //����ռ�ձȵ���50%
}
//  MCU������ʼ����ע���������������
void Init_Devices(void)
{
	WDTCTL = WDTPW + WDTHOLD;     // Stop watchdog timer��ͣ�ÿ��Ź�
	if (CALBC1_8MHZ ==0xFF || CALDCO_8MHZ == 0xFF)
	{
		while(1);            // If calibration constants erased, trap CPU!!
	}

    //����ʱ�ӣ��ڲ�RC������     DCO��8MHz,��CPUʱ��;  SMCLK��1MHz,����ʱ��ʱ��
	BCSCTL1 = CALBC1_8MHZ; 	 // Set range
	DCOCTL = CALDCO_8MHZ;    // Set DCO step + modulation
	BCSCTL3 |= LFXT1S_2;     // LFXT1 = VLO
	IFG1 &= ~OFIFG;          // Clear OSCFault flag
	BCSCTL2 |= DIVS_3;       //  SMCLK = DCO/8

    Init_Ports();           //���ú�������ʼ��I/O��
    Init_Timer0();          //���ú�������ʼ����ʱ��0
    Init_Timer1();          //���ú�������ʼ����ʱ��1
    ADC10_Init();           //���ú�������ʼ��ADC
    _BIS_SR(GIE);           //��ȫ���ж�
   //all peripherals are now initialized
}
//[3.3]����һ�ι��ܺ����ӳ���ר�Ÿ�������뵱ǰ����ȼ���Ӧ��4·���Ƶ�ƽ
//////////////////////////////
//      �����ӳ���                                    //
//////////////////////////////

void gain_control(void)
{
	switch (gain_state)
	{
	case  1:CTL3_L;CTL2_L;CTL1_L;CTL0_H;break;
	case  2:CTL3_L;CTL2_L;CTL1_H;CTL0_L;break;
	case  3:CTL3_L;CTL2_L;CTL1_H;CTL0_H;break;
	case  4:CTL3_L;CTL2_H;CTL1_L;CTL0_L;break;
	case  5:CTL3_L;CTL2_H;CTL1_L;CTL0_H;break;
	case  6:CTL3_L;CTL2_H;CTL1_H;CTL0_L;break;
	case  7:CTL3_L;CTL2_H;CTL1_H;CTL0_H;break;
	case  8:CTL3_H;CTL2_L;CTL1_L;CTL0_L;break;
	case  9:CTL3_H;CTL2_L;CTL1_L;CTL0_H;break;
	case 10:CTL3_H;CTL2_L;CTL1_H;CTL0_L;break;
	case 11:CTL3_H;CTL2_L;CTL1_H;CTL0_H;break;
	case 12:CTL3_H;CTL2_H;CTL1_L;CTL0_L;break;
	case 13:CTL3_H;CTL2_H;CTL1_L;CTL0_H;break;
	case 14:CTL3_H;CTL2_H;CTL1_H;CTL0_L;break;
	case 15:CTL3_H;CTL2_H;CTL1_H;CTL0_H;break;
	}
}

//////////////////////////////
//      �жϷ������        //
//////////////////////////////

// Timer0_A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_A0 (void)
{
	// 0.1������ʱ������
	if (++clock100ms>=V_T100ms)
	{
		clock100ms_flag = 1; //��0.1�뵽ʱ�������־��1
		clock100ms = 0;
	}
 	// 0.5������ʱ������
	if (++clock500ms>=V_T500ms)
	{
		clock500ms_flag = 1; //��0.5�뵽ʱ�������־��1
		clock500ms = 0;
	}

	// ˢ��ȫ������ܺ�LEDָʾ��
	TM1638_RefreshDIGIandLED(digit,pnt,led);

	// ��鵱ǰ�������룬0�����޼�������1-16��ʾ�ж�Ӧ����
	//   ������ʾ����λ�������
	key_code=TM1638_Readkeyboard();
	//[1.5]ɾ��ԭ���������ܵ���������
	//digit[6]=key_code%10;
	//digit[5]=key_code/10;
    //[2.2]����������ʱ���жϷ�������е�״̬ת�ƴ������
	switch (key_state)
	{
	case 0:
		if (key_code>0)
		{ key_state=1;key_flag=1; }
		break;
	case 1:
		if (key_code==0)
		{ key_state=0;}
		break;
	default: key_state=0;break;
	}
    //���ֲ��ŵĵ���
	if (audio_dura==0)
	{
		TA1CTL =0;
		if (music_data[audio_ptr][1]==0)
		{
			audio_ptr=0;
			audio_dura=0;
		}
		else
		{
			audio_dura=music_data[audio_ptr][1]/speed;
			audio_frequency=music_data[audio_ptr][0];
			if (audio_frequency!=0)
			{
				TA1CCR0 = 1000000/audio_frequency; //�趨����
				TA1CCR1 = TA1CCR0/2;
				TA1CTL = TASSEL_2 + MC_1;
			}
			audio_ptr++;
		}
	}
	else
	{
		audio_dura--;
	}
	//��������жϳ���
	switch(infrared_state)
	{
	case 0:
		if (INFRARED!=0)
		{  if (++infrared_pulsewidth>=2)  infrared_state=1;  }
		else
		{  infrared_pulsewidth=0;  }
		break;
	case 1:
		if (INFRARED!=0)
		{  if (++infrared_pulsewidth>=9)  infrared_state=2;  }
		else
		{
			infrared_flag=1;
			infrared_pulsewidth=0;
			infrared_state=0;
		}
		break;
	case 2:
		if (INFRARED==0)
		{
			infrared_flag=2;
			infrared_pulsewidth=0;
			infrared_state=0;
		}
		break;
	default:
		infrared_state=0;
		infrared_pulsewidth=0;
		break;
	}
	if (INFRARED==0) led[0]=0;
	else led[0]=1;
	//�Զ������жϳ���
	if(auto_control)
	{
		ADC10CTL0 |= ENC + ADC10SC;
		while (ADC10CTL1 & ADC10BUSY);//�ȴ�ADת����
		sample = ADC10MEM;
		volt = sample*2.5/1024;

		if(volt > Vmax && gain_state >1)
		{
			--gain_state;
			gain_control();
		}
		if(volt < Vmin && gain_state <15)
		{
			++gain_state;
			gain_control();
		}

		ADC10CTL0 |= ENC + ADC10SC;
	}
}

//////////////////////////////
//         ������           //
//////////////////////////////

int main(void)
{
	//unsigned char i=0,temp;
	Init_Devices( );
	while (clock100ms<3);   // ��ʱ60ms�ȴ�TM1638�ϵ����
	init_TM1638();	    //��ʼ��TM1638
    //[3.4]ϵͳ��ʼ����β������һ��gain_control()
	gain_control();
	music_data=music_data0;

	while(1)
	{
		//[1.6]ɾ��ԭ�����в������õĲ���
		/*
		if (clock100ms_flag==1)   // ���0.1�붨ʱ�Ƿ�
		{
			clock100ms_flag=0;
		   	// ÿ0.1���ۼӼ�ʱֵ�����������ʮ������ʾ���м�����ʱ��ͣ��ʱ
			if (key_code==0)
			{
				if (++test_counter>=10000) test_counter=0;
				digit[0] = test_counter/1000; 	    //�����λ��
				digit[1] = test_counter/100%10; 	//����ʮλ��
				digit[2] = test_counter/10%10; 	    //�����λ��
				digit[3] = test_counter%10;         //����ٷ�λ��
			}
		}

		if (clock500ms_flag==1)   // ���0.5�붨ʱ�Ƿ�
		{
			clock500ms_flag=0;
			// 8��ָʾ��������Ʒ�ʽ��ÿ0.5�����ң�ѭ�����ƶ�һ��
			temp=led[0];
			for (i=0;i<7;i++) led[i]=led[i+1];
			led[7]=temp;
		}
		*/
		//[2.3]����������main�������еĴ������
		if (key_flag==1)
		{
			key_flag=0;
			switch (key_code)
			{
			case 1:
				if (++gain_state>GAIN_STATENUM) gain_state=1;
				//[3.5]sw1�в����󣬵���һ��gain_control()
				gain_control();
				break;
			case 2:
				if (--gain_state==0) gain_state=GAIN_STATENUM;
				//[3.6]sw2�в����󣬵���һ��gain_control()
				gain_control();
				break;
			case 3:    //�Զ����濪��
				if (auto_control == 0)  auto_control = 1;
				else  auto_control = 0;
				break;
			case 13:   //�л�����
				if (++music_num >= 5) music_num=0;
				switch(music_num)
				{
				case 0: music_data = music_data0; audio_dura=0; audio_ptr=0; audio_frequency=0; break;
				case 1: music_data = music_data1; audio_dura=0; audio_ptr=0; audio_frequency=0; break;
				case 2: music_data = music_data2; audio_dura=0; audio_ptr=0; audio_frequency=0; break;
				case 3: music_data = music_data3; audio_dura=0; audio_ptr=0; audio_frequency=0; break;
				case 4: music_data = music_data4; audio_dura=0; audio_ptr=0; audio_frequency=0; break;
				default: break;
				}
				break;
			case 14:   //�л������ٶ�
				switch(speed)
				{
				case 10: speed=20; break;
				case 20: speed=30; break;
				case 30: speed=10; break;
				default: break;
				}
				break;
			default: break;
			}
		//[1.7]����gain_state����������ʾ��ǰ�������λ����
		digit[2] = gain_state/10;  //����ʮλ��
		digit[3] = gain_state%10;  //�����λ��
		led[3]=auto_control;
		if (speed==10) led[7]=1;
		else led[7]=0;
		if (speed==20) led[6]=1;
		else led[6]=0;
		if (speed==30) led[5]=1;
		else led[5]=0;
		}
		if (infrared_flag!=0)
		{
			switch(infrared_flag)
			{
			case 1:
				if (++gain_state>GAIN_STATENUM) gain_state=1;
				//ң�����в����󣬵���һ��gain_control()
				gain_control();
			    break;
			case 2:
				if (--gain_state==0) gain_state=GAIN_STATENUM;
				//ң�����в����󣬵���һ��gain_control()
				gain_control();
				break;
		    default: break;
			}
			infrared_flag=0;  //���־
			digit[2] = gain_state/10;  //����ʮλ��
			digit[3] = gain_state%10;  //�����λ��
		}

	}
}
