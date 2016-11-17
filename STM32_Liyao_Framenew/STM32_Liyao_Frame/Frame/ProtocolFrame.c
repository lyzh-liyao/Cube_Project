#include "ProtocolFrame.h" 
#include "Protocol.h"  
#include "LOG.h" 
#include "List.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define PRINT_ERR
//###################################���������###################################
//Protocol_Info_T protocol_send_infos[SEND_PROTOCOL_NUM] = {0};//����Э��ջ
//Protocol_Info_T recv_protocol_infos[RECV_PROTOCOL_NUM] = {0};//����Э��ջ 
//static uint8_t _Recv_Protocol_Arr[RECV_PROTOCOL_NUM] = {0};//�Ѿ����յ�Э�������  
//Protocol_Desc_T Send_Desc_P[SEND_PROTOCOL_NUM];//����Э��ջ
//Protocol_Desc_T Recv_Desc_P[RECV_PROTOCOL_NUM];//����Э��ջ 
//Protocol_Desc_T Transpond_Desc_P[TRANSPOND_PROTOCOL_NUM];//����Э��ջ

List_Head_T* Send_Desc_P = NULL;
List_Head_T* Recv_Desc_P = NULL;
List_Head_T* Transpond_Desc_P = NULL;
//###################################���ڱ�����################################### 
 


#if PROTOCOL_RESOLVER_1
	#define RESOLVER_1_RPQUEUE_SIZE		30 //����Э�黺�������洢����Э�飩 
	Protocol_Resolver_T _ProtocolResolver_1; 
	Protocol_Resolver_T* ProtocolResolver_1 = &_ProtocolResolver_1;
#endif
#if PROTOCOL_RESOLVER_2
	#define RESOLVER_2_RPQUEUE_SIZE		30 //����Э�黺�������洢����Э�飩 
	Protocol_Resolver_T _ProtocolResolver_2;
	Protocol_Resolver_T* ProtocolResolver_2 = &_ProtocolResolver_2;
#endif
#if PROTOCOL_RESOLVER_3
	#define RESOLVER_3_RPQUEUE_SIZE		30 //����Э�黺�������洢����Э�飩 
	Protocol_Resolver_T _ProtocolResolver_3; 
	Protocol_Resolver_T* ProtocolResolver_3 = &_ProtocolResolver_3;
#endif
#if PROTOCOL_RESOLVER_4
	#define RESOLVER_4_RPQUEUE_SIZE		30 //����Э�黺�������洢����Э�飩 
	Protocol_Resolver_T _ProtocolResolver_4;
	Protocol_Resolver_T* ProtocolResolver_4 = &_ProtocolResolver_4;
#endif
//-----------------------------------------------------  
//###################################���ں�����###################################
/****************************************************
	������:	clean_recv_buf
	����:		���Э��ջ����д���Э��
	����:		liyao 2015��9��8��14:10:51
****************************************************/
void _clean_recv_buf(Protocol_Resolver_T* pr){
	memset(&pr->pi, 0, sizeof(Protocol_Info_T)); 
	pr->Recv_State = 0;
	pr->Cnt = 0;
	pr->Index = 0;
	pr->Is_FE = 0;
	pr->CheckSum = 0;
}

/****************************************************
	������:	_Fetch_Protocol
	����:		��ȡ��ִ���Ѿ������Э��
	����:		liyao 2016��9��8��10:54:34
****************************************************/
void _Fetch_Protocol(Protocol_Resolver_T* pr){
	Protocol_Info_T pi;
	while(Queue_Link_Get(pr->Protocol_Queue,&pi) == 0){ 
		if(pi.Check != NULL){
			if(pi.Check(&pi) < 0){
				Log.error("Э��У�鲻ͨ��\r\n");
				break;
			}
		}
		if(pi.Handle != NULL){
			pi.Handle(&pi);
		}else{
			Log.error("�յ�Э�鵫���޴�������\r\n");
		} 
		FREE(pi.ParameterList);
	}
}

#if PROTOCOL_VERSION == 2
/****************************************************
	������:	Protocol_Put
	����:		����Э�����ݲ�������װ
	����:		Э������
	ע��: 	ͨ��protocol_flag��־λ��ʾ�Ƿ�������µ�Э��
	����:		liyao 2015��9��8��14:10:51
****************************************************/
int8_t _Protocol_Put(Protocol_Resolver_T* pr,uint8_t* datas,uint8_t len){
	uint8_t i, data; 
	uint16_t src_board_action;
	List_Node_T* Cur_Node = Recv_Desc_P->Head;
	
	for(i = 0; i < len; i++){ 
		data = datas[i];
    if(pr->pi.Head != 0xFD && data != 0xFD)
      return -1;
		if(pr->pi.Head == 0xFD && data == 0xFD){ //Э�鱻�ж�����
			_clean_recv_buf(pr);
			Log.error("Э����;����0xFD\r\n");
			return -1;
		}
		if(data == 0xFE){//����ת��
			pr->Is_FE = 1;
			continue;
		}else if(pr->Is_FE){
			switch(data){
				case 0x7D: data = 0xFD;break;
				case 0x78: data = 0xF8;break;
				case 0x7E: data = 0xFE;break;
			} 
			pr->Is_FE = 0;
		}
			
		if(pr->Recv_State > 0 && pr->Recv_State < 7)//�ų�֡ͷ֡β����У���
			pr->CheckSum += data;
	//Э�����״̬��
		switch(pr->Recv_State){
			case 0:	//����֡ͷ
						pr->pi.Head = data;
						pr->Recv_State++; 
						break;
			case 1:	//����Ԥ��λ
						pr->pi.Standby1 = data;
						pr->Recv_State++; 
						break;
			case 2: //����֡��(��ID������λ���һ��)
						pr->Recv_State++; 
						pr->pi.Plen = data;
						if(data < 4){
							_clean_recv_buf(pr);
							Log.error("����֡������\r\n");
							return -2;
						}
						else
							pr->Cnt = pr->pi.ParaLen = data - 3;//������Ϊ��������
							pr->pi.AllLen = data + 5;//������ΪЭ���ܳ��Ȱ���FD��F8
						break;
			case 3: //����Ŀ���
						pr->pi.Module = data;
						pr->Recv_State++; 
						break;
			case 4: //�������
						pr->pi.Serial = data;
						pr->Recv_State++; 
						break;
			case 5: //����ָ����(ACTION)
						pr->pi.Action = data;
						pr->Recv_State++; 
						break;
			case 6: //�������� 
						pr->ParaData[pr->Index++] = data;
						//((uint8_t *)(&pr->pi.ParameterList))[pr->index++] = data;  
						if(--pr->Cnt == 0)
							pr->Recv_State++;
						break;
			case 7: //����У���У��  
						pr->pi.CheckSum = data;
						/*У�����ʱ�ر�*/
						if(((uint8_t)pr->CheckSum & 0xff) != data){
              uint8_t mess[50] = {0};
              sprintf((char *)mess, "Э��У��ʹ���:%X\r\n", pr->CheckSum);
							Log.error((char const*)mess);
							_clean_recv_buf(pr);
							return -3;
						}else{ 
							pr->Recv_State++; 
						} 
						break;
			case 8: //����֡β ֡���ͺͳ��Ƚ���ƥ�� 
						if(data != 0xF8){
							_clean_recv_buf(pr);
							Log.error("֡βλ�÷�0xF8����\r\n");
							return -4;
						}
						pr->pi.Tail = data;
						src_board_action = pr->pi.Module << 8 | pr->pi.Action;

						while(Cur_Node != NULL){
							Protocol_Desc_T* pdt = Cur_Node->Data;
							if(	src_board_action ==  pdt->ModuleAction &&//Ŀ���ƥ��,����ƥ�� 
									pr->pi.ParaLen == pdt->ProtocolSize)//֡����ƥ��
							{
								pr->pi.ParameterList = MALLOC(pr->Index);
								MALLOC_CHECK(pr->pi.ParameterList, "_Protocol_Put");
								memcpy(pr->pi.ParameterList, pr->ParaData, pr->Index);
								pr->pi.Handle = pdt->Handle;
								pr->pi.Check = pdt->Check;
								pr->pi.ProtocolDesc = pdt;
								break;
							}
							Cur_Node = Cur_Node->Next;
						}
						 
						if(Cur_Node == NULL){//У�鲻ͨ�� 
							FREE(pr->pi.ParameterList);
							_clean_recv_buf(pr);
							Log.error("����Э�����ƥ�䵱ǰЭ��\r\n");
							return -5;
						}else{
							Queue_Link_Put(pr->Protocol_Queue, &pr->pi, sizeof(Protocol_Info_T));//��Э����Ϣ����Э�黺�����  
              //ProtocolResolver_1->Fetch_Protocol(ProtocolResolver_1);
							_clean_recv_buf(pr); 
						}
						break;
		}
	}; 
	return 0;
}
#elif PROTOCOL_VERSION == 1
/****************************************************
	������:	Protocol_Put
	����:		����Э�����ݲ�������װ
	����:		Э������
	ע��: 	ͨ��protocol_flag��־λ��ʾ�Ƿ�������µ�Э��
	����:		liyao 2015��9��8��14:10:51
****************************************************/
int8_t _Protocol_Put(Protocol_Resolver_T* pr,uint8_t* datas,uint8_t len){
	uint8_t i, data;
	List_Node_T* Cur_Node = Recv_Desc_P->Head;
	
	for(i = 0; i < len; i++){
		data = datas[i];
    if(pr->pi.Head != 0xFD && data != 0xFD)
      return -1;
		if(pr->pi.Head == 0xFD && data == 0xFD){ //Э�鱻�ж�����
			_clean_recv_buf(pr);
			Log.error("Э����;����0xFD\r\n");
			return -1;
		}
		if(data == 0xFE){//����ת��
			pr->Is_FE = 1;
			continue;
		}else if(pr->Is_FE){
			switch(data){
				case 0x7D: data = 0xFD;break;
				case 0x78: data = 0xF8;break;
				case 0x7E: data = 0xFE;break;
			} 
			pr->Is_FE = 0;
		} 
	//Э�����״̬��
		switch(pr->Recv_State){
			case 0:	//����֡ͷ
						pr->pi.Head = data;
						pr->Recv_State++; 
						break;
			case 1:	//����֡����
						pr->pi.Action = data;
						pr->Recv_State++; 
						break;
			case 2: //�������� �� ֡β
						pr->ParaData[pr->Index++] = data; 
            if(data == 0xF8){
              //У��ʹ��� 
              pr->pi.ParaLen = pr->Index - 2;
              pr->pi.CheckSum = pr->ParaData[pr->Index-2];//�����ڶ���ֵΪУ���  
              for(uint8_t j = 0; j < pr->pi.ParaLen;j++)
                pr->CheckSum += pr->ParaData[j];
              pr->CheckSum += pr->pi.Action;
              if(((uint8_t)pr->CheckSum & 0xff) != pr->pi.CheckSum){
                uint8_t mess[50] = {0};
                sprintf((char *)mess, "Э��У��ʹ���:%X\r\n", pr->CheckSum);
                Log.error((char const*)mess);
                _clean_recv_buf(pr);
                return -3;
              }
              //֡β����
              pr->pi.Tail = data;
              //Э��ƥ�䴦��
              while(Cur_Node != NULL){
                Protocol_Desc_T* pdt = Cur_Node->Data;
                if(	pr->pi.Action ==  (pdt->ModuleAction & 0xff) &&//Ŀ���ƥ��,����ƥ�� 
                    pr->pi.ParaLen == pdt->ProtocolSize)//֡����ƥ��
                {
                  pr->pi.ParameterList = MALLOC(pr->Index);
                  MALLOC_CHECK(pr->pi.ParameterList, "_Protocol_Put");
                  memcpy(pr->pi.ParameterList, pr->ParaData, pr->Index);
                  pr->pi.Handle = pdt->Handle;
                  pr->pi.Check = pdt->Check;
                  pr->pi.ProtocolDesc = pdt;
                  break;
                }
                Cur_Node = Cur_Node->Next;
              }
              //ƥ�������� 
              if(Cur_Node == NULL){//У�鲻ͨ�� 
                FREE(pr->pi.ParameterList);
                _clean_recv_buf(pr);
                Log.error("����Э�����ƥ�䵱ǰЭ��\r\n");
                return -5;
              }else{
                Queue_Link_Put(pr->Protocol_Queue, &pr->pi, sizeof(Protocol_Info_T));//��Э����Ϣ����Э�黺�����  
                //ProtocolResolver_1->Fetch_Protocol(ProtocolResolver_1);
                _clean_recv_buf(pr); 
              } 
            }
						break; 					
		}
	}; 
	return 0;
}
#endif
/****************************************************
	������:	IsShift
	����:		ԭ�ַ�
	����: 	�ַ�ת��
****************************************************/
 uint16_t IsShift(uint8_t* Data){
  if(*Data == 0xFD || *Data == 0xF8 || *Data == 0xFE){//??
     switch(*Data){
        case 0xFD:return 0xFE<<8 | 0x7D;
        case 0xF8:return 0xFE<<8 | 0x78;
        case 0xFE:return 0xFE<<8 | 0x7E;
    }
  }else{
      return 0;
  }
  return 0;
}

//###################################���⺯����###################################

/****************************************************
	������:	getCheckSum_ByProtocolInfo
	����:		����Э����Ϣ���У���
	����:		Protocol_Info_TЭ��������Ϣ
	����ֵ:	У��ͽ��
	ע�⣺	ֻ���ص�8λ
	����:		liyao 2015��9��8��14:10:51
****************************************************/
uint8_t getCheckSum_ByProtocolInfo(Protocol_Info_T* pi){
	uint8_t i, sum = 0; 
  sum += pi->Standby1;
  sum += pi->Plen;
  sum += pi->Module;
  sum += pi->Serial;
  sum += pi->Action;
  for(i = 0; i < pi->ParaLen; i++)
    sum+=((uint8_t*)pi->ParameterList)[i];  
  return sum;
}

/****************************************************
	������:	Protocol_Register
	����:		Э��ע��
	����:		liyao 2016��9��18��16:12:16  
****************************************************/
int8_t Protocol_Register(Protocol_Desc_T* Desc_T,PROTOCOL_TYPE Protocol_Type){
	List_Head_T* tmp_Desc_Head;
	switch((uint8_t)Protocol_Type){
		case (uint8_t)SEND: 
			tmp_Desc_Head = Send_Desc_P;
			break;
		case (uint8_t)RECEIVE: 
			tmp_Desc_Head = Recv_Desc_P;
			break;
		case (uint8_t)TRANSPOND: 
			tmp_Desc_Head = Transpond_Desc_P;
			break;  
	}
	
	Desc_T->SrcModule = (MODULE)(Desc_T->ModuleAction >> 12);
	Desc_T->TargetModule = (MODULE)(Desc_T->ModuleAction & 0x0f);
	List_Add(tmp_Desc_Head, Desc_T, sizeof(Protocol_Desc_T)); 
	return -1;
	
}

/****************************************************
	������:	Get_Protocol_Description
	����:		��ȡЭ��������Ϣ
	����:		liyao 2016��9��18��16:12:16  
****************************************************/
Protocol_Desc_T* Get_Protocol_Description(MODULE_ACTION ModuleAction,PROTOCOL_TYPE Protocol_Type){
	List_Head_T* tmp_Desc_Head;
	List_Node_T* Cur_Node = NULL;
	switch((uint8_t)Protocol_Type){
		case (uint8_t)SEND: 
			tmp_Desc_Head = Send_Desc_P;
			break;
		case (uint8_t)RECEIVE: 
			tmp_Desc_Head = Recv_Desc_P;
			break;
		case (uint8_t)TRANSPOND: 
			tmp_Desc_Head = Transpond_Desc_P;
			break;  
	}
	
	Cur_Node = tmp_Desc_Head->Head;
	while(Cur_Node != NULL){
		Protocol_Desc_T* pdt = Cur_Node->Data;
		if(ModuleAction == pdt->ModuleAction)
			return pdt;
		Cur_Node = Cur_Node->Next;
	} 
	Log.error("Get_Protocol_Descriptionδ�ҵ������ṹ��");
	return NULL;
	
}

/****************************************************
	������:	ProtocolFrame_Init
	����:		��ʼ��ȫ��Э������У�顢ִ�к���
	����:		liyao 2015��9��8��14:10:51      
****************************************************/
void ProtocolFrame_Init(){ 
	Send_Desc_P = List_Init(0);
	Recv_Desc_P = List_Init(0);
	Transpond_Desc_P = List_Init(0);
	//Э���б���ʼ��
	Protocol_Init();
	//�������ݶ��г�ʼ�� 
#if PROTOCOL_RESOLVER_1
//	ProtocolResolver_1->Protocol_Queue = Queue_Init( _UART1_Protocol_QueueBuf,sizeof(Protocol_Info_T), RESOLVER_1_RPQUEUE_SIZE);
	ProtocolResolver_1->Protocol_Queue = Queue_Link_Init(RESOLVER_1_RPQUEUE_SIZE); 
	ProtocolResolver_1->Protocol_Put = _Protocol_Put;
	ProtocolResolver_1->Fetch_Protocol = _Fetch_Protocol;
#endif  

#if PROTOCOL_RESOLVER_2
//	ProtocolResolver_2->Protocol_Queue = Queue_Init( _UART2_Protocol_QueueBuf,sizeof(Protocol_Info_T), RESOLVER_2_RPQUEUE_SIZE);
	ProtocolResolver_2->Protocol_Queue = Queue_Link_Init(RESOLVER_2_RPQUEUE_SIZE);
	ProtocolResolver_2->RPQueue_Size = RESOLVER_2_RPQUEUE_SIZE; 
	ProtocolResolver_2->Protocol_Put = _Protocol_Put;
	ProtocolResolver_2->Fetch_Protocol = _Fetch_Protocol;
#endif  
	
#if PROTOCOL_RESOLVER_3
//	ProtocolResolver_3->Protocol_Queue = Queue_Init( _UART3_Protocol_QueueBuf,sizeof(Protocol_Info_T), RESOLVER_3_RPQUEUE_SIZE);
	ProtocolResolver_3->Protocol_Queue = Queue_Link_Init(RESOLVER_3_RPQUEUE_SIZE);
	ProtocolResolver_3->RPQueue_Size = RESOLVER_3_RPQUEUE_SIZE; 
	ProtocolResolver_3->Protocol_Put = _Protocol_Put;
	ProtocolResolver_3->Fetch_Protocol = _Fetch_Protocol;
#endif 
	
#if PROTOCOL_RESOLVER_4
//	ProtocolResolver_4->Protocol_Queue = Queue_Init( _UART4_Protocol_QueueBuf,sizeof(Protocol_Info_T), RESOLVER_4_RPQUEUE_SIZE);
	ProtocolResolver_4->Protocol_Queue = Queue_Link_Init(RESOLVER_4_RPQUEUE_SIZE);
	ProtocolResolver_4->RPQueue_Size = RESOLVER_4_RPQUEUE_SIZE; 
	ProtocolResolver_4->Protocol_Put = _Protocol_Put;
	ProtocolResolver_4->Fetch_Protocol = _Fetch_Protocol;
#endif
}

/****************************************************
	������:	Protocol_To_Uart
	����:		�򻺳���д�������������
	����:		Protocol_Info_TЭ��������Ϣ
	����:		liyao 2015��9��8��14:10:51
****************************************************/
int8_t Protocol_To_Uart(Protocol_Info_T* pi){ 
	uint8_t data[PROTOCOL_SINGLE_BUFF] = {0},i = 0, index = 0; 
  uint16_t tmpData = 0; 
  
  tmpData = pi->Head;
  data[index++] = tmpData;
#if PROTOCOL_VERSION == 2
  if((tmpData = IsShift(&pi->Standby1)) > 0){
    data[index++] = tmpData>>8; 
    data[index++] = tmpData&0xff; 
  }else{
    data[index++] = pi->Standby1;
  }
  if((tmpData = IsShift(&pi->Plen)) > 0){
    data[index++] = tmpData>>8; 
    data[index++] = tmpData&0xff; 
  }else{
    data[index++] = pi->Plen;
  }
  if((tmpData = IsShift(&pi->Module)) > 0){
    data[index++] = tmpData>>8; 
    data[index++] = tmpData&0xff; 
  }else{
    data[index++] = pi->Module;
  }
  if((tmpData = IsShift(&pi->Serial)) > 0){
    data[index++] = tmpData>>8; 
    data[index++] = tmpData&0xff; 
  }else{
    data[index++] = pi->Serial;
  }  
#endif
  if((tmpData = IsShift(&pi->Action)) > 0){
    data[index++] = tmpData>>8; 
    data[index++] = tmpData&0xff; 
  }else{
    data[index++] = pi->Action;
  }    
  
  for(i = 0; i < pi->ParaLen; i++){
    if((tmpData = IsShift((uint8_t*)pi->ParameterList + i)) > 0){
      data[index++] = tmpData>>8;
      data[index++] = tmpData&0xff;
    }else{
      data[index++] = ((uint8_t*)pi->ParameterList)[i];
    }
  }
  if((tmpData = IsShift(&pi->CheckSum)) > 0){
    data[index++] = tmpData>>8; 
    data[index++] = tmpData&0xff; 
  }else{
    data[index++] = pi->CheckSum;
  }  
  data[index++] = pi->Tail; 
  
  pi->ProtocolDesc->Send(data, index);
	return 0;
}

//###################################�Զ�����չ������###################################
/****************************************************
	������:	Protocol_Send
	����:		����Э��
	����:		Ŀ���,�����ṹ��,�����ṹ�峤��
	ע��Э����ʷ���ͳһ����
	����:		liyao 2016��9��18��11:50:55
****************************************************/
void Protocol_Send(MODULE_ACTION ModuleAction, void* Data,uint8_t Len){ 
	Protocol_Info_T pi = {0};
	pi.ParameterList = MALLOC(Len);
	MALLOC_CHECK(pi.ParameterList, "Protocol_Send"); 
	pi.ProtocolDesc = Get_Protocol_Description(ModuleAction, SEND);
	pi.Head = 0xFD;
	pi.Plen = Len + 3;//��������+3   ֡����
	pi.Module = ModuleAction >> 8; 
	pi.Serial = pi.ProtocolDesc->Serial++;
	pi.Action = ModuleAction & 0x00ff;
//	pi.ParameterList = *Protocol_t; 
	memcpy(pi.ParameterList, Data, Len);
	pi.CheckSum = getCheckSum_ByProtocolInfo(&pi); 
	pi.Tail = 0xF8;
	pi.ParaLen = Len;
	pi.AllLen = pi.Plen + 5;
	Protocol_To_Uart(&pi);	
	FREE(pi.ParameterList);
}

/****************************************************
	������:	Protocol_Send_Transpond
	����:		ת��Э��
	����:		Protocol_Info_T�ṹ��
	����:		liyao 2016��9��18��11:51:35
****************************************************/
void Protocol_Send_Transpond(Protocol_Info_T* pi){  
	Protocol_To_Uart(pi);	
}

/*****************************************************************
������:FetchProtocols
��ע: ������λ�����������ܺ���
******************************************************************/
void FetchProtocols(void)
{
	#if PROTOCOL_RESOLVER_1
		ProtocolResolver_1->Fetch_Protocol(ProtocolResolver_1);
	#endif
	#if PROTOCOL_RESOLVER_2
		ProtocolResolver_2->Fetch_Protocol(ProtocolResolver_2);
	#endif
	#if PROTOCOL_RESOLVER_3
		ProtocolResolver_3->Fetch_Protocol(ProtocolResolver_3);
	#endif
	#if PROTOCOL_RESOLVER_4
		ProtocolResolver_4->Fetch_Protocol(ProtocolResolver_4);
	#endif
}