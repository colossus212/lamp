#include "stm32f4xx.h"
#include "arm_math.h"
#include "logic_f4.h"
#include "rtthread.h"
#include "finsh.h"

#define adc_size 8
typedef struct adc_io
{
	uint8_t ADC_CH;
	GPIO_TypeDef* GPIOx ;
	uint16_t GPIO_Pin;
}adc_io;

#define DMA_ADC_MODE	1
#define REGULAR_MODE	1

#define ADC_BUFFER_SIZE		16//定义ADC连续采样个数 <16
#define ADC1_DMA_CHANNEL	DMA_Channel_0
#define ADC1_DMA_STREAM		DMA2_Stream0
#define ADC1_DR_ADDRESS		(ADC1_BASE + 0x4C)
#define ADC2_DMA_CHANNEL	DMA_Channel_1
#define ADC2_DMA_STREAM		DMA2_Stream2
#define ADC2_DR_ADDRESS		(ADC2_BASE + 0x4C)
#define ADC3_DMA_CHANNEL	DMA_Channel_2
#define ADC3_DMA_STREAM		DMA2_Stream1
#define ADC3_DR_ADDRESS		(ADC3_BASE + 0x4C)

q15_t ADC1ConvertedValue[ADC_BUFFER_SIZE];//ADC1
q15_t ADC2ConvertedValue[ADC_BUFFER_SIZE];//ADC2 __IO q15_t ADC2ConvertedValue[ADC_BUFFER_SIZE];
q15_t ADC3ConvertedValue[ADC_BUFFER_SIZE];//ADC3

adc_io adc[adc_size] = {
{ADC_Channel_0,GPIOA, GPIO_Pin_0}/*AIN4*/, {ADC_Channel_1,GPIOA, GPIO_Pin_1}/*AIN5*/, 
{ADC_Channel_2,GPIOA, GPIO_Pin_2}/*AIN6*/, {ADC_Channel_3,GPIOA, GPIO_Pin_3}/*AIN7*/,
{ADC_Channel_0,GPIOC, GPIO_Pin_0}/*AIN0*/, {ADC_Channel_1,GPIOC, GPIO_Pin_1}/*AIN1*/, 
{ADC_Channel_2,GPIOC, GPIO_Pin_2}/*AIN2*/, {ADC_Channel_3,GPIOC, GPIO_Pin_3}/*AIN3*/
};

static void init_adc1regular(void)
{//开始ADC1 DMA 
	DMA_Cmd(ADC1_DMA_STREAM,ENABLE);//使用ADC2 DMA方式采集电池电压
	ADC_DMACmd(ADC1,ENABLE);//使能ADC2的DMA传输
	ADC_RegularChannelConfig(ADC1, adc[0].ADC_CH, 1, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC1, adc[0].ADC_CH, 2, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC1, adc[0].ADC_CH, 3, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC1, adc[0].ADC_CH, 4, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC1, adc[0].ADC_CH, 5, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC1, adc[0].ADC_CH, 6, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC1, adc[0].ADC_CH, 7, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC1, adc[0].ADC_CH, 8, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC1, adc[0].ADC_CH, 9, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC1, adc[0].ADC_CH,10, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC1, adc[0].ADC_CH,11, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC1, adc[0].ADC_CH,12, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC1, adc[0].ADC_CH,13, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC1, adc[0].ADC_CH,14, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC1, adc[0].ADC_CH,15, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC1, adc[0].ADC_CH,16, ADC_SampleTime_28Cycles);	
	ADC_DMARequestAfterLastTransferCmd(ADC1,ENABLE); //源数据变化时开启DMA传输
	ADC_Cmd(ADC1, ENABLE);
}							
	
static void init_adc2regular(void)
{//开始ADC2 DMA 顺序采样AIN0,AIN1,AIN2
	DMA_Cmd(ADC2_DMA_STREAM,ENABLE);//使用ADC2 DMA方式采集电池电压
	ADC_DMACmd(ADC2,ENABLE);//使能ADC2的DMA传输
	ADC_RegularChannelConfig(ADC2, adc[1].ADC_CH, 1, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC2, adc[1].ADC_CH, 2, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC2, adc[1].ADC_CH, 3, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC2, adc[1].ADC_CH, 4, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC2, adc[1].ADC_CH, 5, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC2, adc[1].ADC_CH, 6, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC2, adc[1].ADC_CH, 7, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC2, adc[1].ADC_CH, 8, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC2, adc[1].ADC_CH, 9, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC2, adc[1].ADC_CH,10, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC2, adc[1].ADC_CH,11, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC2, adc[1].ADC_CH,12, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC2, adc[1].ADC_CH,13, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC2, adc[1].ADC_CH,14, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC2, adc[1].ADC_CH,15, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC2, adc[1].ADC_CH,16, ADC_SampleTime_28Cycles);	
	ADC_DMARequestAfterLastTransferCmd(ADC2,ENABLE); //源数据变化时开启DMA传输
	ADC_Cmd(ADC2, ENABLE);
}						

static void init_adc3regular(void)
{//开始ADC3 DMA循序采样AIN2,AIN0,AIN1	
	DMA_Cmd(ADC3_DMA_STREAM,ENABLE);
	ADC_DMACmd(ADC3,ENABLE);//使能ADC3的DMA传输
	ADC_RegularChannelConfig(ADC3, adc[2].ADC_CH, 1, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC3, adc[2].ADC_CH, 2, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC3, adc[2].ADC_CH, 3, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC3, adc[2].ADC_CH, 4, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC3, adc[2].ADC_CH, 5, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC3, adc[2].ADC_CH, 6, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC3, adc[2].ADC_CH, 7, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC3, adc[2].ADC_CH, 8, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC3, adc[2].ADC_CH, 9, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC3, adc[2].ADC_CH,10, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC3, adc[2].ADC_CH,11, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC3, adc[2].ADC_CH,12, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC3, adc[2].ADC_CH,13, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC3, adc[2].ADC_CH,14, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC3, adc[2].ADC_CH,15, ADC_SampleTime_28Cycles);
	ADC_RegularChannelConfig(ADC3, adc[2].ADC_CH,16, ADC_SampleTime_28Cycles);
	ADC_DMARequestAfterLastTransferCmd(ADC3,ENABLE); //源数据变化时开启DMA传输
	ADC_Cmd(ADC3, ENABLE);
}

void dma_initialize(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	//配置ADC1 DMA
    DMA_DeInit(ADC1_DMA_STREAM);
    DMA_InitStructure.DMA_Channel 				= ADC1_DMA_CHANNEL;
    DMA_InitStructure.DMA_PeripheralBaseAddr 	= (uint32_t)ADC1_DR_ADDRESS;
    DMA_InitStructure.DMA_Memory0BaseAddr 		= (uint32_t)ADC1ConvertedValue;
    DMA_InitStructure.DMA_DIR             		= DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize      		= ADC_BUFFER_SIZE;//采样个数
    DMA_InitStructure.DMA_PeripheralInc   		= DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc       		= DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize 	= DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize     	= DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode     				= DMA_Mode_Circular ;//缓冲区满后自动覆盖
    DMA_InitStructure.DMA_Priority 				= DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode 				= DMA_FIFOMode_Enable;//使能FIFO
    DMA_InitStructure.DMA_FIFOThreshold   		= DMA_FIFOThreshold_HalfFull;
    DMA_InitStructure.DMA_MemoryBurst     		= DMA_MemoryBurst_INC4;
    DMA_InitStructure.DMA_PeripheralBurst 		= DMA_PeripheralBurst_Single;
    DMA_Init(ADC1_DMA_STREAM, & DMA_InitStructure);
	DMA_Cmd(ADC1_DMA_STREAM,ENABLE);
	//配置ADC2 DMA
	DMA_DeInit(ADC2_DMA_STREAM);
    DMA_InitStructure.DMA_Channel 				= ADC2_DMA_CHANNEL;
    DMA_InitStructure.DMA_PeripheralBaseAddr 	= (uint32_t)ADC2_DR_ADDRESS;
    DMA_InitStructure.DMA_Memory0BaseAddr 		= (uint32_t)ADC2ConvertedValue;
    DMA_InitStructure.DMA_DIR             		= DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize      		= ADC_BUFFER_SIZE;//采样个数
    DMA_InitStructure.DMA_PeripheralInc   		= DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc       		= DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize 	= DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize     	= DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode     				= DMA_Mode_Circular ;//缓冲区满后自动覆盖
    DMA_InitStructure.DMA_Priority 				= DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode 				= DMA_FIFOMode_Enable;//使能FIFO
    DMA_InitStructure.DMA_FIFOThreshold   		= DMA_FIFOThreshold_HalfFull;
    DMA_InitStructure.DMA_MemoryBurst     		= DMA_MemoryBurst_INC4;
    DMA_InitStructure.DMA_PeripheralBurst 		= DMA_PeripheralBurst_Single;
    DMA_Init(ADC2_DMA_STREAM, & DMA_InitStructure);
	DMA_Cmd(ADC2_DMA_STREAM,ENABLE);
	//配置ADC3 DMA
	DMA_DeInit(ADC3_DMA_STREAM);
    DMA_InitStructure.DMA_Channel 				= ADC3_DMA_CHANNEL;
    DMA_InitStructure.DMA_PeripheralBaseAddr 	= (uint32_t)ADC3_DR_ADDRESS;
    DMA_InitStructure.DMA_Memory0BaseAddr 		= (uint32_t)ADC3ConvertedValue;
    DMA_InitStructure.DMA_DIR             		= DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize      		= ADC_BUFFER_SIZE;//采样个数
    DMA_InitStructure.DMA_PeripheralInc   		= DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc       		= DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize 	= DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize     	= DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode     				= DMA_Mode_Circular ;//缓冲区满后自动覆盖
    DMA_InitStructure.DMA_Priority 				= DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode 				= DMA_FIFOMode_Enable;//使能FIFO
    DMA_InitStructure.DMA_FIFOThreshold   		= DMA_FIFOThreshold_HalfFull;
    DMA_InitStructure.DMA_MemoryBurst     		= DMA_MemoryBurst_INC4;
    DMA_InitStructure.DMA_PeripheralBurst 		= DMA_PeripheralBurst_Single;
	DMA_Init(ADC3_DMA_STREAM, & DMA_InitStructure);
	DMA_Cmd(ADC3_DMA_STREAM,ENABLE);
}

void adc_initialize(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	uint8_t i = 0;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//配置GPIO口为模拟输入
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//配置为弱上拉或弱下拉
	for(i = 0; i < adc_size; i++)
	{
		GPIO_InitStructure.GPIO_Pin = adc[i].GPIO_Pin ;
		GPIO_Init(adc[i].GPIOx , &GPIO_InitStructure);
	}
#if DMA_ADC_MODE == 1	
	dma_initialize();
#endif	
	/* ADC Common Init **********************************************************/
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//ADC独立模式
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//ADC采样周期6分频  APB2 / 4 = 84M / 4 = 21M < 36M
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//单次采样无效
	ADC_CommonInit(&ADC_CommonInitStructure);
	
	/* ADC1 Init ****************************************************************/
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//ADC_ExternalTrigConvEdge_Falling;//open external trigger
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T8_TRGO;//使用T8做为TRIGGER
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 16;//连续转换个数
	ADC_Init(ADC1, &ADC_InitStructure);
	ADC_Init(ADC2, &ADC_InitStructure);
	ADC_Init(ADC3, &ADC_InitStructure);
	ADC_TempSensorVrefintCmd(ENABLE);
//	
//	TIM_SelectOutputTrigger(TIM8, TIM_TRGOSource_OC1);
#if REGULAR_MODE == 1
	init_adc1regular();
	init_adc2regular();
	init_adc3regular();
#endif
}

void trig_adc(void)//must close external trigger in ADC_Init()
{
	ADC_SoftwareStartConv(ADC1);
	ADC_SoftwareStartConv(ADC2);
	ADC_SoftwareStartConv(ADC3);
}
#ifdef FINSH_USING_SYMTAB
FINSH_FUNCTION_EXPORT(trig_adc, trig_adc);
#endif

uint16_t adc_get(uint8_t ch)
{
	uint8_t i = 0;
	q15_t mean = 0;
	q15_t rms = 0;
//	trig_adc();
	switch (ch)
	{
		case 0: arm_mean_q15(ADC1ConvertedValue, 15, &mean);
				arm_rms_q15(ADC1ConvertedValue, 15, &rms);
				mean = mean*12500/4095/33;//I = code*2.5V/4095/33R*5000
//				rt_kprintf("mean = %d, rms = %d", mean,rms);
			break;
		case 1: arm_mean_q15(ADC2ConvertedValue , 15, &mean);
			break;
		case 2: arm_mean_q15(ADC3ConvertedValue , 15, &mean);
			break;
		case 3: mean = 0;
			break;
		case 4: mean = 0;
			break;
		case 5:	mean = 0;
			break;
		case 6:	mean = 0;
			break;
		case 7:	mean = 0;
			break;
		default:
			break;
	}
	return mean;
}
#ifdef FINSH_USING_SYMTAB
FINSH_FUNCTION_EXPORT(adc_get, adc_get(uint8_t ch));
#endif


















