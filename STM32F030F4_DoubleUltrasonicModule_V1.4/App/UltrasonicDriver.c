#include "UltrasonicDriver.h"
#include "gpio.h"
#include "adc.h"
#include "TaskTimeManager.h"  
#include "Protocol.h"
static Ultrasonic_T* _Ultrasonic;
uint8_t Ult_Cnt;
volatile uint8_t cnt;
void ULT_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim == &ULT_WAVE_TIM){
		Ult_Cnt--;
		HAL_GPIO_WritePin(C_A_GPIO_Port, C_A_Pin,(GPIO_PinState)(Ult_Cnt%2));
		HAL_GPIO_WritePin(C_Y_GPIO_Port, C_Y_Pin,(GPIO_PinState)((Ult_Cnt+1)%2));
		if(Ult_Cnt == 0){
			ULT_WAVE_TIM_STOP();
			_Ultrasonic->Ult_State = ULT_SEND_FINISH;
		}
	}

	if(htim == &ULT_CAL_TIM){
		ULT_CAL_TIM_STOP();
		ULT_DMA_STOP(&ULT_ADC);
		_Ultrasonic->Ult_State = ULT_DMA_FINISH;
		//printf("cnt:%d\r\n", cnt);
	}
}


static void _Ultrasonic_DataClear(Ultrasonic_T* ult){
	ult->It_Cnt = 0;
	ult->In1MaxValue = 0;
	ult->In1MaxTiming = 0;
	ult->In2MaxValue = 0;
	ult->In2MaxTiming = 0;
	ult->DistanceSection = 0;
	ult->NearCnt = 0;
	ult->FarCnt = 0;
}


/*
	波形解析策略：1000us以内阈值超过NEAR_DISTANCE 计算距离
								1000us以外阈值超过FAR_DISTANCE  计算距离
*/
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if(_Ultrasonic->DistanceSection == NEAR_DISTANCE){
		return;
	}
	for(uint16_t i = 0; i < WAVE_SINGLE_BUFF; i++){
		if(_Ultrasonic->It_Cnt < (1000 / (WAVE_SINGLE_BUFF * SAMPLING_INTERVAL ))){// 1000us以内划分为近段
			if(_Ultrasonic->Wave_Adc_Buff[i][ADC_IN2_CHANNEL] > NEAR_DISTANCE){//数据中出现高于近段阈值的数
				_Ultrasonic->NearCnt++;
				if((_Ultrasonic->Wave_Adc_Buff[i][ADC_IN2_CHANNEL]) > _Ultrasonic->In2MaxValue){//取高于近段阈值的最大数
					_Ultrasonic->In2MaxValue = _Ultrasonic->Wave_Adc_Buff[i][ADC_IN2_CHANNEL];
					_Ultrasonic->In2MaxTiming = _Ultrasonic->It_Cnt * (WAVE_SINGLE_BUFF * SAMPLING_INTERVAL) + (i * SAMPLING_INTERVAL);
				}
			}
			
		}else{//1000us以外划分为远段
			if(_Ultrasonic->Wave_Adc_Buff[i][ADC_IN2_CHANNEL] > FAR_DISTANCE){
				_Ultrasonic->FarCnt++;
				if((_Ultrasonic->Wave_Adc_Buff[i][ADC_IN2_CHANNEL])  > _Ultrasonic->In2MaxValue){
					_Ultrasonic->In2MaxValue = _Ultrasonic->Wave_Adc_Buff[i][ADC_IN2_CHANNEL];
					_Ultrasonic->In2MaxTiming = _Ultrasonic->It_Cnt * (WAVE_SINGLE_BUFF * SAMPLING_INTERVAL) + (i * SAMPLING_INTERVAL);
				}
			}
			
		}
	}
	if(_Ultrasonic->NearCnt > 3){
		_Ultrasonic->DistanceSection = NEAR_DISTANCE;
	}else if(_Ultrasonic->FarCnt > 3){
		_Ultrasonic->DistanceSection = FAR_DISTANCE;
	}
	_Ultrasonic->It_Cnt++;
	//HAL_ADC_Start_DMA(hadc, (uint32_t*)_Ultrasonic->Wave_Adc_Buff, WAVE_ADC_BUFF);
	
}

int16_t DistanceAnalysis(uint16_t MaxTiming){
	int16_t distance = (MaxTiming-200)*34/100/2;
	if(distance < 29)
		distance = 0;
	else if(_Ultrasonic->DistanceSection == 0)
		distance = 0;
		
	return distance;
}


static ULT_RESULT _Ultrasonic_Send(Ultrasonic_T* Ult){
	if(Ult->Ult_State != ULT_NONE)
		return ULT_BUSY;
	Ult->Ult_State = ULT_SEND;
	Ult_Cnt = Ult->Wave;
	cnt=0;
	
	_Ultrasonic_DataClear(_Ultrasonic);
	
	ULT_DMA_START(&ULT_ADC);															//ADC开始采集
	ULT_WAVE_TIM_START();																	//开启波形发送
	ULT_CAL_TIM_START();					//计时器开始计时	
	while(_Ultrasonic->Ult_State != ULT_DMA_FINISH);
	int16_t distance = DistanceAnalysis(_Ultrasonic->In2MaxTiming);
//	printf("距离:%d\r\n", distance);
//	printf("%d\t%d\t%d\t%d\t%dmm\r\n", _Ultrasonic->In1MaxValue, _Ultrasonic->In1MaxTiming,_Ultrasonic->In2MaxValue, _Ultrasonic->In2MaxTiming, distance);
//	printf("\v");
//	for(uint16_t i = 0; i < WAVE_SINGLE_BUFF; i++){
//		printf("%d,%d\r\n",_Ultrasonic->Wave_Adc_Buff[i][ADC_IN1_CHANNEL],_Ultrasonic->Wave_Adc_Buff[i][ADC_IN2_CHANNEL]);
//	}
//	for(uint8_t i = 1; i > 0 ;i--){
		//printf("%d\t%d\t%d\t%d\t%f\r\n", _Ultrasonic->In1MaxValue, _Ultrasonic->In1MaxTiming,_Ultrasonic->In2MaxValue, _Ultrasonic->In2MaxTiming, (_Ultrasonic->In1MaxTiming-200.0) /56  );
	
//		printf("\v");
//		Distance = DistanceAnalysis(Ult->Wave_Adc_Buff, WAVE_ADC_BUFF);
//		printData(Ult); 
		
//			break;
//	}
	
	Distance_T Distance = {0}; 
	Distance.ID = _Ultrasonic->ID;
	Distance.Distance_H = distance >> 8;
	Distance.Distance_L = distance & 0xFF;
	Protocol_Send( DISTANCE_REPORT, &Distance, sizeof(Distance_T));
	Ult->Ult_State = ULT_NONE;
	return ULT_OK;
}

int8_t getTempSensor(void){
	uint16_t AD_Value = 0;
	HAL_ADC_DeInit(&hadc);
	HAL_ADC_Temp_Init();
	HAL_ADC_Start(&hadc); 
	HAL_ADC_PollForConversion(&hadc,50);
	if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc), HAL_ADC_STATE_REG_EOC)){
		//int8_t temp = (1430 - (HAL_ADC_GetValue(&hadc)*3300.0/255 ))/4.3 + 25;
		AD_Value = ((AD_Value*3300/4096-760)/2.5+25);
		
	}
	printf("溫度：%d\r\n", AD_Value);
	HAL_ADC_Stop(&hadc); 
	HAL_ADC_DeInit(&hadc);
	MX_ADC_Init();
	return 0;
}


////0 蓝线  1橙线
//uint16_t DistanceAnalysis(uint8_t *Data, uint16_t Size){
//	int16_t DataMax = 0, DataMaxIndex = 0;
//	for(uint16_t i = 0; i < Size; i += 2){
//		if(DATA_CONVERT(Data, i)  > DataMax){
//			DataMax = DATA_CONVERT(Data, i);
//			DataMaxIndex = i/2;
//		}
//	}
//	
////	for(uint16_t i = 61; i < Size; i += 2){
////		if(DATA_CONVERT(Data, i) > 120){
////			DataMax = DATA_CONVERT(Data, i);
////			DataMaxIndex = i/2;
////		}
////	}
//	printf("最大数:%d,最高点:%d\r\n", DataMax, DataMaxIndex);
////	if(DataMaxIndex == 0)
////		return 0;
//	return (DataMaxIndex - 4) * 38 * 34 / 2 / 10;
//}

ULT_RESULT Ultrasonic_Init(Ultrasonic_T* Ult){
	Ult->Wave = ULT_WAVE_COUNT;
	Ult->Ult_State = ULT_NONE;
	Ult->TaskID = -1;
	Ult->ID = 5;
//	Ult->Wave_Adc_Buff = MALLOC(WAVE_ADC_BUFF);
//	MALLOC_CHECK(Ult->Wave_Adc_Buff, NULL);
	_Ultrasonic = Ult;
  HAL_GPIO_WritePin(GPIOA, SYNC_Pin|C_Y_Pin|C_A_Pin, GPIO_PIN_SET);
	
	
	Ult->Send = _Ultrasonic_Send;
	return ULT_OK;
}

