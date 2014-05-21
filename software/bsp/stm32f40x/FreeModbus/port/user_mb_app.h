#ifndef	USER_APP
#define USER_APP
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mb_m.h"
#include "mbconfig.h"
#include "mbframe.h"
#include "mbutils.h"

/* -----------------------Slave Defines -------------------------------------*/
#define S_DISCRETE_INPUT_START        0
#define S_DISCRETE_INPUT_NDISCRETES   16
#define S_COIL_START                  0
#define S_COIL_NCOILS                 512
#define S_REG_INPUT_START             0
#define S_REG_INPUT_NREGS             10
#define S_REG_HOLDING_START           0
#define S_REG_HOLDING_NREGS           512
//�ӻ�ģʽ���ڱ��ּĴ����У�������ַ��Ӧ�Ĺ��ܶ���
#define          S_HD_RESERVE                     0		  //����
#define          S_HD_CPU_USAGE_MAJOR             1         //��ǰCPU�����ʵ�����λ
#define          S_HD_CPU_USAGE_MINOR             2         //��ǰCPU�����ʵ�С��λ

//�ӻ�ģʽ��������Ĵ����У�������ַ��Ӧ�Ĺ��ܶ���
#define          S_IN_RESERVE                     0		  //����

//�ӻ�ģʽ������Ȧ�У�������ַ��Ӧ�Ĺ��ܶ���
#define          S_CO_RESERVE                     2		  //����

//�ӻ�ģʽ������ɢ�����У�������ַ��Ӧ�Ĺ��ܶ���
#define          S_DI_RESERVE                     1		  //����

/* -----------------------Master Defines -------------------------------------*/
#define M_DISCRETE_INPUT_START        0
#define M_DISCRETE_INPUT_NDISCRETES   16
#define M_COIL_START                  0
#define M_COIL_NCOILS                 4096
#define M_REG_INPUT_START             0
#define M_REG_INPUT_NREGS             8
#define M_REG_HOLDING_START           6001
#define M_REG_HOLDING_NREGS           3756
//����ģʽ���ڱ��ּĴ����У�������ַ��Ӧ�Ĺ��ܶ���
#define          M_HD_RESERVE                     0		  //����

//����ģʽ��������Ĵ����У�������ַ��Ӧ�Ĺ��ܶ���
#define          M_IN_RESERVE                     0		  //����

//����ģʽ������Ȧ�У�������ַ��Ӧ�Ĺ��ܶ���
#define          M_CO_RESERVE                     2		  //����

//����ģʽ������ɢ�����У�������ַ��Ӧ�Ĺ��ܶ���
#define          M_DI_RESERVE                     1		  //����

#endif