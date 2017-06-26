#include "UltrasonicDriver.h"
#include "gpio.h"
#include "adc.h"
#include "TaskTimeManager.h"  
static Ultrasonic_T* _Ultrasonic;
uint8_t Ult_Cnt;
uint16_t jishi1 = 0, jishi2 = 0;
uint16_t DistanceAnalysis(uint8_t *Data, uint16_t Size);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim == &ULT_WAVE_TIM){
		Ult_Cnt--;
		HAL_GPIO_WritePin(C_A_GPIO_Port, C_A_Pin,(GPIO_PinState)(Ult_Cnt%2));
		HAL_GPIO_WritePin(C_Y_GPIO_Port, C_Y_Pin,(GPIO_PinState)((Ult_Cnt+1)%2));
		if(Ult_Cnt == 0){
//			DelayUS(15);
			GPIO_Float_Init();//转换为IO浮空输入
			HAL_TIM_Base_Stop_IT(&ULT_WAVE_TIM);
			_Ultrasonic->Ult_State = ULT_SEND_FINISH;
		}
	}
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{
	jishi1 = ULT_CAL_TIM_CNT();	
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	_Ultrasonic->Ult_State = ULT_DMA_FINISH;
	jishi2 = ULT_CAL_TIM_CNT();
	ULT_DMA_STOP(ULT_ADC);		
//	printf("dma时间：%d,%d\r\n", jishi1,jishi2);
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

//uint8_t DataTmplate[] = {123,123,128,128,123,123,128,86,124,128,121,124,33,128,123,123,128,128,124,124,128,128,123,123,128,96,124,128,96,124,64,128,57,124,84,128,55,124,56,128,33,124,28,36,9,117,8,124,4,128,0,124,9,128,2,103,10,77,0,33,8,15,2,6,6,15,4,18,5,22,5,15,4,18,5,8,4,10,5,3,4,2,4,3,5,2,5,1,4,1,5,2,5,3,4,0,5,2,4,2,4,1,4,2,4,1,5,0,5,0,5,0,5,2,4,0,4,3,4,1,4,1,4,3,5,2,4,2,5,3,4,1,4,0,5,0,4,1,4,1,5,2,5,4,5,2,5,1,4,1,4,3,5,1,4,1,5,0,5,4,5,2,5,1,5,1,4,1,4,1,4,1,4,3,4,1,4,3,4,0,5,2,4,2,4,3,4,3,4,4,4,3,3,4,5,2,5,2,4,4,5,2,4,0,4,1,4,2,4,2,4,1,5,0,4,3,4,0,4,1,5,5,4,2,5,4,5,2,4,1,5,1,4,3,4,0,5,2,5,3,4,0,4,1,4,0,5,3,4,0,4,3,5,1,4,3,5,4,5,1,4,0,4,2,4,2,5,3,4,0,4,0,5,6,5,0,5,3,5,2,4,3,4,1,5,1,4,4,4,3,4,0,4,1,4,4,4,2,4,0,4,4,4,2,5,2,4,2,4,0,5,1,5,2,4,3,4,1,4,1,4,3,4,1,4,3,4,2,4,3,4,1,4,0,4,2,4,2,4,1,4,3,4,2,4,0,4,3,4,3,4,0,4,2,4,3,4,1,4,3,4,2,4,4,4,1,4,1,4,3,4,0,5,2,4,0,4,5,4,2,5,2,4,5,4,5,4,0,4,5,4,2,4,1,5,3,5,4,4,0,4,3,5,0,5,1,4,3,4,4,5,1,5,4,4,2,4,1,4,4,4,1,4,2,4,2,4,3,4,1,4,2,4,4,4,3,4,1,4,2,4,5,4,1,5,1,4,2,4,2,4,0,4,4,4,2,4,3,5,2,4,3,4,2,4,6,4,3,4,3,4,3,4,4,5,2,4,1,4,1,5,6,5,1,5,1,4,4,4,1,4,1,4,0,4,2,4,4,4,1,4,1,4,3,4,1,4,4,4,1,5,2,4,3,4,1,4,0,4,2,4,3,5,3,5,0,5,1,4,10,5,4,4,3,4,3,4,4,4,0,5,2,5,0,4,7,4,0,5,2,4,5,4,2,4,1,4,0,4,4,4,1,5,5,4,1,4,3,5,2,5,0,4,3,5,3,4,2,4,1,4,2,4,4,4,0,4,3,5,3,4,3,4,2,4,4,4,2,4,6};
uint8_t DataTmplate[] = {0,0,251,252,0,0,251,252,0,0,179,0,0,252,0,0,251,252,0,0,252,251,0,0,252,251,0,138,251,0,0,252,132,0,251,251,0,0,251,252,0,0,251,137,53,251,147,0,120,251,101,0,148,252,94,0,148,252,101,0,140,252,113,119,131,86,121,222,125,28,125,215,122,50,126,178,122,88,126,145,122,114,125,124,123,131,124,120,124,134,124,119,123,134,124,119,125,129,124,127,124,129,123,124,124,131,123,124,124,130,124,123,124,124,124,128,124,127,124,127,124,122,124,128,124,126,124,128,124,123,123,127,124,126,124,128,124,126,124,127,124,127,123,126,124,124,124,126,123,125,124,124,124,124,124,126,124,124,124,128,124,124,124,125,123,126,124,124,124,129,124,123,123,132,124,123,124,122,123,129,124,122,124,133,124,117,124,135,124,115,124,132,124,116,124,130,123,126,124,126,123,129,124,120,124,132,124,117,124,128,124,122,124,127,124,125,123,129,124,123,124,128,124,122,123,132,124,119,123,131,124,122,123,127,124,124,124,126,123,126,124,123,124,128,124,124,123,124,125,126,124,127,124,126,124,127,124,122,123,127,123,123,124,128,123,125,124,124,124,128,124,125,123,124,124,126,123,125,124,124,124,125,124,126,124,126,123,125,124,124,124,130,124,124,123,124,124,126,124,127,124,122,124,129,124,121,124,128,124,124,124,124,124,127,124,125,123,129,124,121,124,124,124,127,123,126,123,123,124,130,123,125,124,123,123,128,124,125,124,127,124,124,124,126,124,125,124,129,124,125,124,128,123,124,124,125,123,128,123,120,124,129,124,124,124,128,124,124,124,125,124,128,124,125,124,124,123,129,124,125,124,126,123,127,124,123,124,125,124,126,124,126,124,125,124,126,124,124,124,128,123,125,124,126,123,124,124,128,124,121,124,132,123,123,124,124,124,125,123,127,124,125,123,124,124,125,123,128,124,126,124,125,124,122,123,127,123,125,124,122,124,126,124,125,124,129,123,122,124,124,123,127,123,126,124,130,124,121,124,126,124,122,124,127,124,123,124,127,124,127,124,126,124,124,124,124,124,125,124,128,123,123,124,126,124,128,123,124,124,126,124,125,124,131,124,120,124,127,124,126,124,121,124,128,124,125,124,125,123,128,123,124,124,125,124,125,123,124,124,127,123,123,124,127,124,128,124,124,124,124,124,129,124,123,123,125,124,127,123,126,124,127,124,125,124,121,124,124,124,126,124,127,123,126,124,122,123,130,124,123,124,125,123,127,123,127,124,122,124,125,124,128,124,126,124,122,124,131,123,124,124,128,124,125,124,124,124,124,124,128,124,121,124,124,124,128,124,126,124,125,124,125,124,127,124,126,124,123,123,126,124,126,124,125,124,126,124,125,123,127,124,128,124,126,124,124,124,126,124,128,123,124,124,125,124,125,124,129,124,123,124,122,124,129,124,124,124,125,124,128,124,125,124,125,124,123,124,126,124,127,124,125,123,126,124,126,124,124,123,127,123,124,124,127,124,125,124,129,124,123,123,128,124,124,124,129,124,123,124,126,123,126,124,128,124,124,124,129,124,127,124,124,124,123,124,127,123,126,124,126,124,131,124,123,124,125,124,128,124,124,123,121,124,128,124,128,124,126,124,125,123,127,124,124,124,124,124,129,124,125,124,125,124,128,124,127,124,126,124,129,124,119,123,127,124,126,124,128,124,122,124,128,123,120,124,132,124,124,124,125,124,126,124,126,124,125,124,127,124,125,123,131,124,122,123,125,123,125,124,129,124,125,124,123,124,129,124,123,124,125,124,124,124,132,124,121,124,125,124,125,124,123,124,130,124,122,124,126,124,126,124,131,124,125,124,126,124,121,124,130,124,121,124,124,124,128,124,127,124,125,124,126,124,126,124,128,123,121,124,130,124,120,124,129,123,122,124,131,124,121,124,128,124,126,124,121,123,127,124,125,124,125,123,125,124,123,124,127,124,127,124,124,124,123,123,126,123,125,124,130,123,122,124,130,124,125,124,121,124,129,123,127,124,123,124,125,124,128,124,124,123,130,124,120,124,125,124,127,124,123,124,126,124,125,124,129,123,123,124,125,123,127,124,126,125,124,123,127,124,126,123,126,124,126,124,122,124,126,124,128,124,124,124,125,124,126,123,123,124,129,124,124,124,123,124,130,124,122,124,130,123,124,124,125,124,124,123,128,124,123,124,127,124,126,124,121,124,129,124,124,124,125,124,121,124,129,124,121,124,127};
void printData(Ultrasonic_T* Ult){
	for(uint16_t i = 0; i < WAVE_ADC_BUFF; i+=2){
		//printf("%d\t%d\r\n", *(Ult->Wave_Adc_Buff+i), *(Ult->Wave_Adc_Buff+i+1));
		printf("%d\t%d\r\n", *(Ult->Wave_Adc_Buff+i) - DataTmplate[i], *(Ult->Wave_Adc_Buff+i+1) - DataTmplate[i+1]);
		//if(i%2==0) continue;
//		printf("%d",Ult->Wave_Adc_Buff[i]);
//		if( i > 0 && i % 2 == 1)
//			printf("\r\n");
	}
}

ULT_RESULT Ultrasonic_Send(Ultrasonic_T* Ult){	
	if(Ult->Ult_State != ULT_NONE)
		return ULT_BUSY;
	uint16_t Distance = 0;
	Ult->Ult_State = ULT_SEND;
	Ult_Cnt = Ult->Wave;
	MX_GPIO_Init();//转换为IO输出
	__HAL_TIM_SET_COUNTER(&ULT_WAVE_TIM, 0);//清0 timer
	ULT_DMA_START(&ULT_ADC);								//ADC开始采集
	HAL_TIM_Base_Start_IT(&ULT_WAVE_TIM);		//开启定时器
	ULT_CAL_TIM_START();									  //计时器开始计时
	//printf("%X,%X\r\n",Ult->Ult_State, ULT_SEND_FINISH);
	//while(Ult->Ult_State != ULT_SEND_FINISH);//等待超声波发送完成
	//while(ULT_CAL_TIM_CNT() < 1200);//躲开盲区
	Ult->Ult_State = ULT_RECEIVE;		//超声波状态转换
	while(Ult->Ult_State != ULT_DMA_FINISH); 
	for(uint8_t i = 1; i > 0 ;i--){
		
		printf("\v");
		Distance = DistanceAnalysis(Ult->Wave_Adc_Buff, WAVE_ADC_BUFF);
		printData(Ult); 
		if(Distance > 0)
			break;
	}
	//printf("距离:%d\r\n", Distance);
	Ult->Ult_State = ULT_NONE;
	if(Distance == 0)
		return ULT_TIMEOUT;
	return ULT_OK;
}

int8_t getTempSensor(void){
	
	HAL_ADC_DeInit(&hadc);
	HAL_ADC_Temp_Init();
	HAL_ADC_Start(&hadc); 
	HAL_ADC_PollForConversion(&hadc,0xffff);
	int8_t temp = (1430 - (HAL_ADC_GetValue(&hadc)*3300/255 ))/4 + 25;
	printf("溫度：%d\r\n", temp);
	HAL_ADC_Stop(&hadc); 
	HAL_ADC_DeInit(&hadc);
	MX_ADC_Init();
	return 0;
}


//0 蓝线  1橙线
uint16_t DistanceAnalysis(uint8_t *Data, uint16_t Size){
	int16_t DataMax = 0, DataMaxIndex = 0;
	for(uint16_t i = 0; i < Size; i += 2){
		if(DATA_CONVERT(Data, i)  > DataMax){
			DataMax = DATA_CONVERT(Data, i);
			DataMaxIndex = i/2;
		}
	}
	
//	for(uint16_t i = 61; i < Size; i += 2){
//		if(DATA_CONVERT(Data, i) > 120){
//			DataMax = DATA_CONVERT(Data, i);
//			DataMaxIndex = i/2;
//		}
//	}
	printf("最大数:%d,最高点:%d\r\n", DataMax, DataMaxIndex);
//	if(DataMaxIndex == 0)
//		return 0;
	return (DataMaxIndex - 4) * 38 * 34 / 2 / 10;
}
/*uint16_t DistanceAnalysis(uint8_t *Data, uint16_t Size){
	uint8_t HighCnt = 0, LowCnt = 0, HighFlag = 1, LowFlag = 0, LowIndex = 255, DeadZone = 0;
	uint8_t *Cursor = NULL;
//	printf("------------------------\r\n");
	//
	for(int i = 1; i < Size; i+=2){
		uint8_t tmpData = abs(*(Data+i) - 128);
		Cursor = (Data + i);
		if(DATA_CONVERT(Cursor,0) > HIGH_THRESHOLD && DATA_CONVERT( Cursor, 2) > HIGH_THRESHOLD && DATA_CONVERT(Cursor, 4) > HIGH_THRESHOLD){
			HighCnt++;
			HighFlag = 1;
			LowFlag = 0;
		}else if(DATA_CONVERT(Cursor, 0) < HIGH_THRESHOLD && DATA_CONVERT(Cursor, 2) < HIGH_THRESHOLD && DATA_CONVERT(Cursor, 4) < HIGH_THRESHOLD){
			LowCnt++;
			HighFlag = 0;
			LowFlag = 1;
		}else{
			if(HighFlag)
				HighCnt++;
			else if(LowFlag)
				LowCnt++;
		}
		
		if(i == 81){
			//printf("high:%d,low:%d\r\n",HighCnt,LowCnt);
			if(HighCnt > 35){
				DeadZone = 1;
				printf("hight > 35\r\n");
			}
		}
	}
	
	HighCnt = 0, LowCnt = 0, HighFlag = 1, LowFlag = 0, LowIndex = 255;
	for(int i = 0; i < Size; i+=2){
		uint8_t tmpData = DATA_CONVERT(Data, i);//abs(*(Data+i) - 128);
		Cursor = (Data + i);
		if(DATA_CONVERT(Cursor,0) > HIGH_THRESHOLD && DATA_CONVERT( Cursor, 2) > HIGH_THRESHOLD && DATA_CONVERT(Cursor, 4) > HIGH_THRESHOLD){
			HighCnt++;
			HighFlag = 1;
			LowFlag = 0;
		}else if(DATA_CONVERT(Cursor, 0) < HIGH_THRESHOLD && DATA_CONVERT(Cursor, 2) < HIGH_THRESHOLD && DATA_CONVERT(Cursor, 4) < HIGH_THRESHOLD){
			LowCnt++;
			HighFlag = 0;
			LowFlag = 1;
			if(LowIndex == 255)
				LowIndex = i;
		}else{
			if(HighFlag)
				HighCnt++;
			else if(LowFlag)
				LowCnt++;
		}
		
	}
	if(DeadZone)
		printf("盲区内测距：%d, %d\r\n", LowIndex, (int)(LowIndex / 2.0 * 38.0 * 0.034/2));
	else
		printf("障碍物在盲区外\r\n");
	return 0;
}*/

