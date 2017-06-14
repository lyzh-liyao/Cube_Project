#include "UltrasonicDriver.h"
#include "gpio.h"
#include "adc.h"
static Ultrasonic_T* _Ultrasonic;
uint8_t Ult_Cnt;
uint16_t jishi1 = 0, jishi2 = 0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim == &ULT_WAVE_TIM){
		Ult_Cnt--;
		HAL_GPIO_WritePin(C_A_GPIO_Port, C_A_Pin,(GPIO_PinState)(Ult_Cnt%2));
		if(Ult_Cnt == 0){
			GPIO_Float_Init();//转换为IO浮空输入
			HAL_TIM_Base_Stop_IT(&ULT_WAVE_TIM);
			_Ultrasonic->Ult_State = ULT_SEND_FINISH;
		}
	}
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
//void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	jishi1 = ULT_CAL_TIM_CNT();
	//ULT_DMA_STOP(ULT_ADC);	
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	jishi2 = ULT_CAL_TIM_CNT();
	ULT_DMA_STOP(ULT_ADC);		
	printf("dma时间：%d,%d\r\n", jishi1,jishi2);
	jishi1 = jishi2 = 0;
}

ULT_RESULT Ultrasonic_Init(Ultrasonic_T* Ult){
	Ult->Wave = ULT_WAVE_COUNT;
	Ult->Ult_State = ULT_NONE;
	Ult->Wave_Adc_Buff = MALLOC(WAVE_ADC_BUFF);
	MALLOC_CHECK(Ult->Wave_Adc_Buff, NULL);
	_Ultrasonic = Ult;
	return ULT_OK;
}

void printData(Ultrasonic_T* Ult){
	for(uint16_t i = 0; i < WAVE_ADC_BUFF; i++){
		if(i%2==0) continue;
		printf("%3d,%3d", i, Ult->Wave_Adc_Buff[i]);
		if( i > 0 && i % 10 == 0)
			printf("\r\n");
	}
}

ULT_RESULT Ultrasonic_Send(Ultrasonic_T* Ult){	

	uint8_t in = 0,out = 0;
	Ult->Ult_State = ULT_SEND;
	Ult_Cnt = Ult->Wave;
	MX_GPIO_Init();//转换为IO输出
	__HAL_TIM_SET_COUNTER(&ULT_WAVE_TIM, 0);
	HAL_TIM_Base_Start_IT(&ULT_WAVE_TIM);
	ULT_CAL_TIM_START();//计时器开始计时
	//printf("%X,%X\r\n",Ult->Ult_State, ULT_SEND_FINISH);
//	while(Ult->Ult_State != ULT_SEND_FINISH);
//	
//	while(ULT_CAL_TIM_CNT() < 1400);//躲开盲区
	ULT_DMA_START(&ULT_ADC);								//ADC开始采集
	Ult->Ult_State = ULT_RECEIVE;		//超声波状态转换
	while(ULT_CAL_TIM_CNT() < 15*1000){//最大量程内（15ms ≈ 5M）循环查找波形
		in = 0,out = 0;
		for(uint16_t i = 0; i < WAVE_ADC_BUFF; i++){//WAVE_ADC_BUFF
			if(((Ult->Wave_Adc_Buff[i] > 130 && Ult->Wave_Adc_Buff[i] < 140) ||
				  (Ult->Wave_Adc_Buff[i] > 110 && Ult->Wave_Adc_Buff[i] < 120)) && 
				   Ult->Wave_Adc_Buff[i] > 0){
				in = 1;
			}	 
			if(Ult->Wave_Adc_Buff[i] > 140 || Ult->Wave_Adc_Buff[i] < 110){
				out = 1;
			}
		}
		if(in == 1 )//&& out == 0)
			{
				uint16_t tl = ULT_CAL_TIM_CNT();
				
				Ult->Ult_State = ULT_RECEIVE_FINISH;
				ULT_DMA_STOP(&ULT_ADC);	
				ULT_CAL_TIM_STOP();//计时器关闭计时
				
				printf("耗时:%d,距离:%d\r\n", tl, (int)(0.34f*tl)/2);
				//printData(Ult);
				return ULT_OK;
			}
		
	}
	return ULT_TIMEOUT;
}

int8_t getTempSensor(void){
	
	HAL_ADC_DeInit(&hadc);
	HAL_ADC_Temp_Init();
	HAL_ADC_Start(&hadc); 
	HAL_ADC_PollForConversion(&hadc,0xffff);
	int8_t temp = (1430 - (HAL_ADC_GetValue(&hadc)*3300/255 ))/4 + 25;
	//printf("溫度：%d\r\n", temp);
	HAL_ADC_Stop(&hadc); 
	HAL_ADC_DeInit(&hadc);
	MX_ADC_Init();
	return 0;
}


