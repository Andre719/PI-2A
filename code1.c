//本程序时钟采用内部RC振荡器。     DCO：8MHz,供CPU时钟;  SMCLK：1MHz,供定时器时钟
#include <msp430g2553.h>
#include <tm1638.h>  //与TM1638有关的变量及函数定义均在该H文件中

//[3.1]定义用于输出控制电平的4路GPIO引脚操作命令，推荐使用引脚P1.0至P1.3
#define CTL0_L  P1OUT&=~BIT0
#define CTL0_H  P1OUT|=BIT0
#define CTL1_L  P1OUT&=~BIT5
#define CTL1_H  P1OUT|=BIT5
#define CTL2_L  P1OUT&=~BIT2
#define CTL2_H  P1OUT|=BIT2
#define CTL3_L  P1OUT&=~BIT3
#define CTL3_H  P1OUT|=BIT3
//定义红外检测引脚P1.7
#define INFRARED  (P1IN&BIT7)

//////////////////////////////
//         常量定义         //
//////////////////////////////

// 0.1s软件定时器溢出值，5个20ms
#define V_T100ms	5
// 0.5s软件定时器溢出值，25个20ms
#define V_T500ms	25

//[1.1]增益等级总数
#define GAIN_STATENUM    15

//乐谱
const unsigned int music_data0[][2]=        //《届かない恋》
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
const unsigned int music_data1[][2]=        //《荷塘月色》
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
const unsigned int music_data2[][2]=		//《星之所在》
{
	{1047,400},{988,400},{1047,400},{1319,400},{988,800},{0,800},{880,400},{784,400},
	{1760,400},{523,400},{784,800},{0,800},{698,400},{1319,400},{698,400},{523,400},{988,800},
	{784,800},{880,400},{988,400},{1047,400},{1319,400},{1175,800},{0,800},{1047,400},{988,400},
	{1047,400},{1319,400},{988,800},{784,800},{880,400},{988,400},{1047,400},{1175,400},{1319,800},
	{1319,800},{1397,400},{1319,400},{1175,400},{1047,400},{988,400},{1319,400},{0,0}
};
const unsigned int music_data3[][2]=		//《喀秋莎》
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
const unsigned int music_data4[][2] =       //《Canon》
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
//       变量定义           //
//////////////////////////////

// 软件定时器计数
unsigned char clock100ms=0;
unsigned char clock500ms=0;
// 软件定时器溢出标志
unsigned char clock100ms_flag=0;
unsigned char clock500ms_flag=0;
// 测试用计数器
unsigned int test_counter=0;
// 8位数码管显示的数字或字母符号
// 注：板上数码位从左到右序号排列为4、5、6、7、0、1、2、3
//[1.2]左4位固定显示“增益”的英文GAIN,右数第3位固定显示“负号”
unsigned char digit[8]={' ','-',' ',' ','G','A','I','N'};
// 8位小数点 1亮  0灭
// 注：板上数码位小数点从左到右序号排列为4、5、6、7、0、1、2、3
unsigned char pnt=0x04;
// 8个LED指示灯状态，每个灯4种颜色状态，0灭，1绿，2红，3橙（红+绿）
// 注：板上指示灯从左到右序号排列为7、6、5、4、3、2、1、0
//     对应元件LED8、LED7、LED6、LED5、LED4、LED3、LED2、LED1
//[1.3]LED灯设为全灭
unsigned char led[]={0,0,0,0,0,0,0,0};
//[1.4]增益等级取值，初值为1，对应增益0.1
unsigned char gain_state=1;
//[2.1]增加两个与按键操作有关的全局变量
unsigned char key_state=0,key_flag=1,key_code=0;
//增加音乐播放变量
unsigned int const (*music_data)[2];
unsigned int audio_frequency=0,audio_dura=0,audio_ptr=0;
unsigned int music_num = 0;
unsigned int speed = 20;
//增加红外遥控变量
unsigned int infrared_state=0,infrared_pulsewidth=0,infrared_flag=0;
//增加自动增益变量
unsigned char auto_control = 0;
int sample;
double volt;   //AD转换后数值
double Vmax = 1.5;
double Vmin = 0.4;

//////////////////////////////
//       系统初始化         //
//////////////////////////////

//ADC初始化子程序，用于自动增益
void ADC10_Init(void)
{
    ADC10CTL0 = SREF_1 + ADC10SHT_2 + REFON + REF2_5V+ ADC10ON;  //Vref+ =2.5V
    P1DIR &= ~BIT4;
    ADC10CTL1 = INCH_4 ;
    ADC10AE0 |= BIT4;
}

//  I/O端口和引脚初始化
void Init_Ports(void)
{
	P2SEL &= ~(BIT7+BIT6);       //P2.6、P2.7 设置为通用I/O端口
	  //因两者默认连接外晶振，故需此修改

	P2DIR |= BIT7 + BIT6 + BIT5; //P2.5、P2.6、P2.7 设置为输出
	  //本电路板中三者用于连接显示和键盘管理器TM1638，工作原理详见其DATASHEET
	//[3.2]增加端口初始化语句，将4路GPIO引脚P1.0、P1.5、P1.2、P1.3设置为输出
	P1DIR |= BIT0 + BIT5 + BIT2 +BIT3;
	//初始化P2.1设置为定时器A1的PWM输出引脚
	P2SEL|= BIT1;
	P2DIR|= BIT1;
	//初始化P1.7作为输入
	P1DIR &= ~BIT7;
 }

//  定时器TIMER0初始化，循环定时20ms
void Init_Timer0(void)
{
	TA0CTL = TASSEL_2 + MC_1 ;      // Source: SMCLK=1MHz, UP mode,
	TA0CCR0 = 20000;                // 1MHz时钟,计满20000次为 20ms
	TA0CCTL0 = CCIE;                // TA0CCR0 interrupt enabled
}

//初始化Timer1，产生440Hz的方波信号
void Init_Timer1(void)
{
	TA1CTL = TASSEL_2 + MC_1;
	TA1CCTL1 = OUTMOD_7;
	TA1CCR0 = 1000000/440;          //设定周期，1000000为定时器1时钟频率，440为音频频率
	TA1CCR1 = TA1CCR0/2;            //设置占空比等于50%
}
//  MCU器件初始化，注：会调用上述函数
void Init_Devices(void)
{
	WDTCTL = WDTPW + WDTHOLD;     // Stop watchdog timer，停用看门狗
	if (CALBC1_8MHZ ==0xFF || CALDCO_8MHZ == 0xFF)
	{
		while(1);            // If calibration constants erased, trap CPU!!
	}

    //设置时钟，内部RC振荡器。     DCO：8MHz,供CPU时钟;  SMCLK：1MHz,供定时器时钟
	BCSCTL1 = CALBC1_8MHZ; 	 // Set range
	DCOCTL = CALDCO_8MHZ;    // Set DCO step + modulation
	BCSCTL3 |= LFXT1S_2;     // LFXT1 = VLO
	IFG1 &= ~OFIFG;          // Clear OSCFault flag
	BCSCTL2 |= DIVS_3;       //  SMCLK = DCO/8

    Init_Ports();           //调用函数，初始化I/O口
    Init_Timer0();          //调用函数，初始化定时器0
    Init_Timer1();          //调用函数，初始化定时器1
    ADC10_Init();           //调用函数，初始化ADC
    _BIS_SR(GIE);           //开全局中断
   //all peripherals are now initialized
}
//[3.3]增加一段功能函数子程序，专门负责输出与当前增益等级对应的4路控制电平
//////////////////////////////
//      功能子程序                                    //
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
//      中断服务程序        //
//////////////////////////////

// Timer0_A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_A0 (void)
{
	// 0.1秒钟软定时器计数
	if (++clock100ms>=V_T100ms)
	{
		clock100ms_flag = 1; //当0.1秒到时，溢出标志置1
		clock100ms = 0;
	}
 	// 0.5秒钟软定时器计数
	if (++clock500ms>=V_T500ms)
	{
		clock500ms_flag = 1; //当0.5秒到时，溢出标志置1
		clock500ms = 0;
	}

	// 刷新全部数码管和LED指示灯
	TM1638_RefreshDIGIandLED(digit,pnt,led);

	// 检查当前键盘输入，0代表无键操作，1-16表示有对应按键
	//   键号显示在两位数码管上
	key_code=TM1638_Readkeyboard();
	//[1.5]删除原程序对数码管的两处操作
	//digit[6]=key_code%10;
	//digit[5]=key_code/10;
    //[2.2]按键操作在时钟中断服务程序中的状态转移处理程序
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
    //音乐播放的调节
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
				TA1CCR0 = 1000000/audio_frequency; //设定周期
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
	//红外控制中断程序
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
	//自动增益中断程序
	if(auto_control)
	{
		ADC10CTL0 |= ENC + ADC10SC;
		while (ADC10CTL1 & ADC10BUSY);//等待AD转换器
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
//         主程序           //
//////////////////////////////

int main(void)
{
	//unsigned char i=0,temp;
	Init_Devices( );
	while (clock100ms<3);   // 延时60ms等待TM1638上电完成
	init_TM1638();	    //初始化TM1638
    //[3.4]系统初始化结尾，调用一次gain_control()
	gain_control();
	music_data=music_data0;

	while(1)
	{
		//[1.6]删除原程序中不再有用的操作
		/*
		if (clock100ms_flag==1)   // 检查0.1秒定时是否到
		{
			clock100ms_flag=0;
		   	// 每0.1秒累加计时值在数码管上以十进制显示，有键按下时暂停计时
			if (key_code==0)
			{
				if (++test_counter>=10000) test_counter=0;
				digit[0] = test_counter/1000; 	    //计算百位数
				digit[1] = test_counter/100%10; 	//计算十位数
				digit[2] = test_counter/10%10; 	    //计算个位数
				digit[3] = test_counter%10;         //计算百分位数
			}
		}

		if (clock500ms_flag==1)   // 检查0.5秒定时是否到
		{
			clock500ms_flag=0;
			// 8个指示灯以走马灯方式，每0.5秒向右（循环）移动一格
			temp=led[0];
			for (i=0;i<7;i++) led[i]=led[i+1];
			led[7]=temp;
		}
		*/
		//[2.3]按键操作在main主程序中的处理程序
		if (key_flag==1)
		{
			key_flag=0;
			switch (key_code)
			{
			case 1:
				if (++gain_state>GAIN_STATENUM) gain_state=1;
				//[3.5]sw1有操作后，调用一次gain_control()
				gain_control();
				break;
			case 2:
				if (--gain_state==0) gain_state=GAIN_STATENUM;
				//[3.6]sw2有操作后，调用一次gain_control()
				gain_control();
				break;
			case 3:    //自动增益开关
				if (auto_control == 0)  auto_control = 1;
				else  auto_control = 0;
				break;
			case 13:   //切换歌曲
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
			case 14:   //切换播放速度
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
		//[1.7]根据gain_state计算用于显示当前增益的两位数字
		digit[2] = gain_state/10;  //计算十位数
		digit[3] = gain_state%10;  //计算个位数
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
				//遥控器有操作后，调用一次gain_control()
				gain_control();
			    break;
			case 2:
				if (--gain_state==0) gain_state=GAIN_STATENUM;
				//遥控器有操作后，调用一次gain_control()
				gain_control();
				break;
		    default: break;
			}
			infrared_flag=0;  //清标志
			digit[2] = gain_state/10;  //计算十位数
			digit[3] = gain_state%10;  //计算个位数
		}

	}
}
