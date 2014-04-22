#include "stm32f4xx.h"
#include "rtthread.h"
#include "finsh.h"
//#include "typedef.h"
//#include "port.h"
#include "arm_math.h"
#include "my_math.h"
//#include "math.h"

#define debug_math 0

/*******************************************求和*******************************************************/
int32_t sum_s16(int16_t *p, uint16_t size)
{
	int32_t sum = 0;
	uint16_t i = 0;
	for(i = 0; i < size; i++)
	{
		sum = sum + *(p+i);
	}
	return sum;
}
/*******************************************求和*******************************************************/

/**************************************找出最大值******************************************************/
uint16_t find_u16_max(uint16_t *p, uint16_t size)
{
	uint16_t i = 0;
	uint16_t buffer = 0;
	
	for(i = 0; i < size; i++ )
	{
		if(buffer < *(p+i))
		{
			buffer = *(p+i);
		}
	}
	return buffer;
}

int16_t find_s16_max(int16_t *p, uint16_t size)
{
	uint16_t i = 0;
	int16_t buffer = 0;
	
	for(i = 0; i < size; i++ )
	{
		if(buffer < *(p+i))
		{
			buffer = *(p+i);
		}
	}
	return buffer;
}
/**************************************找出最大值******************************************************/

/**************************************线性插值函数****************************************************/
uint16_t linear_interp_y(linear_interp_table * psource, uint16_t x, uint16_t size)//get Y from x
{
	uint16_t i = 0;
	float y = 0;
	for(i = 0; i < size; i++)
	{
		if(x <= psource[i].x)
		{
			break;
		}
	}

	if(i == 0)
	{
		y = (float)x *psource[i].y /psource[i].x;
	}
	else if(i == size)
				{
					y = psource[i-2].y + (float)(x - psource[i-2].x)*(psource[i-1].y - psource[i-2].y)/(psource[i-1].x - psource[i-2].x);
				}
				else
				{
					y = psource[i-1].y + (float)(x - psource[i-1].x)*(psource[i].y - psource[i-1].y)/(psource[i].x - psource[i-1].x);
				}
#if debug_math == 1
	rt_kprintf("linear_interp_y result y = %d, para x = %d\n ",(uint16_t)y,x);
#endif
	return (uint16_t)y;
}

uint16_t linear_interp_x(linear_interp_table * psource, uint16_t y, uint16_t size)//get x from y
{
	uint16_t i = 0;
	float x = 0;
	
	for(i = 0; i < size; i++)
	{
		if(y <= psource[i].y )
		{
			break;
		}
	}
	
	if(i == 0)
	{
		x = (float)y *psource[i].x /psource[i].y;
	}
	else if(i == size)
				{
					x = psource[i-2].x + (float)(y - psource[i-2].y)*(psource[i-1].x - psource[i-2].x)/(psource[i-1].y - psource[i-2].y);
				}
				else
				{
					x = psource[i-1].x + (float)(y - psource[i-1].y)*(psource[i].x - psource[i-1].x)/(psource[i].y - psource[i-1].y);
				}
#if debug_math == 1
	rt_kprintf("linear_interp_y result x = %d,para y = %d\n ",(uint16_t)x,y);
#endif
	return (uint16_t)x;
}
/**************************************线性插值函数****************************************************/

/******************************************MY PID***************************************************/
float  Ti = 0.0/*积分周期*/, Td = 0.0/*微分周期*/, T = 1.0/*采样周期*/;
//float Kp = -30.0,Ki = -2.0,Kd = -120.0;
//float Kp = -17.75,Ki = -0.56,Kd = 0;
float Kp = -21,Ki = -1.3,Kd = -83.2;
//#define max_out 4095
void PID_init(float p, float i, float d)//
{
	
	Kp = p;
	Ki = i; //Kp*T/Ti;
	Kd = d; //Kp*Td/T;
#if debug_math == 1
	rt_kprintf("Ti = %d, Td = %d, T = %d\n",Kp*T/Ki,Kd*T/Kp,T);
#endif
}
#ifdef RT_USING_FINSH
#include <finsh.h>
FINSH_FUNCTION_EXPORT(PID_init, PID_init(uint16_t p, uint16_t i, uint16_t d));
#endif

/*u(k) = u(k-1) + du(k)*/
float Incremental_PID(float In, float Ref)//
{
	static float LastError = 0/*e(k-1)*/,PrevError = 0/*e(k-2)*/;
	float Output_E = 0, Error = 0/*e(k)*/, de = 0, de1 = 0;

	Error = Ref - In ;

	de = Error - LastError;
	de1 = LastError - PrevError;
	
	Output_E = Kp*de + Ki*Error + Kd*(de-de1);
	
	PrevError = LastError;
	LastError = Error;

  return (Output_E);/*du(k)*/
}
/*u(k) = Kp*e(k) + Ki*( e(0)+e(1)+...+e(k) + Kd*(e(k)-e(k-1)) ) + u(0))*/
#define max 1000.0
#define min	-500.0
float test_1 = 0;
float Position_PID(float In, float Ref)//u(0) = 0
{
	static float PrevError_C = 0, IntTerm_C = 0/*sum of integral*/;
	float Error = 0;
	float Output = 0;
	
	Error = Ref - In;
  IntTerm_C += Ki*Error;//IntTerm_C = Ki*(e(0) + e(1) + ... + e(k))
  Output = Kp * Error;
	test_1 = IntTerm_C;
	if(IntTerm_C > (double)max)
	{
		IntTerm_C = (double)max;
	}
	if(IntTerm_C < (double)min)
	{
		IntTerm_C = (double)min;
	}
  Output += IntTerm_C;
  Output += Kd * (Error - PrevError_C);

  PrevError_C = Error;

  return (Output);//u(k)
}
/******************************************MY PID***************************************************/

/*********************************************NTC R CONVERT TO TEMPERATURE***********************************************/
#define T0C 273.0 //define 0'C = 273.0K
//#define B 3477
//#define T 25.0 //10000 in 25'C
//#define R 10000
float R2T(uint16_t Rt, uint16_t B, double T, uint16_t R)
{
	double temp = 0;
	temp = (double)(log((double)Rt/R)/B) + (double)(1/(T+T0C));
	temp = (1/temp) - T0C;
#if debug_math == 1
	rt_kprintf("temperature = %d *0.1'C\n",(uint16_t)(temp*10));
#endif
	return temp;
}
#ifdef RT_USING_FINSH
#include <finsh.h>
FINSH_FUNCTION_EXPORT(R2T, R2T(uint16_t rr)..);
#endif
/*********************************************NTC R CONVERT TO TEMPERATUER***********************************************/





