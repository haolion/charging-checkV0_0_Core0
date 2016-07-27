/*
 * 程序名称：SAE-J1939链路层
 * 描述：	1、基于CAN2.0B协议；
 * 		2、 根据协议对数据接收进行控制，包括握手，释放等；
 * 		3、打包以及解包；
 * 		4、本程序为侦听模式；
 * 		5、本程序适用于XL902的DC-MeterV0.1(直流表主板V0.1)主板。
 * 版本：V1.0
 * 编写：张颖豪
 * 版本修改：张颖豪
 * 时间：2015/12/15
 *
 */
#include "SAE_J1939.h"

static SAEJ1939_mailParam *MailParamBuff;
static char MailNum;
static CAN_RX_BUF J1939RX_Buff;
static J1939Unit J1939Frame;
static J1939INFO *info;
static J1939Frame_Buff *J1939FrameBuff;


//双向链表结构操作，用于处理帧的接收缓存
static J1939_Result J1939BuffNode_ini ( J1939INFO *J1939_info);
static J1939BuffNode *J1939BuffNode_add(J1939INFO *J1939_info, J1939Unit J1939data);
static J1939BuffNode *J1939BuffNode_find(J1939INFO *J1939_info, unsigned int J1939PGN);
static J1939_Result J1939BuffNode_delete(J1939INFO *J1939_info, J1939BuffNode *pNode);
static J1939_Result J1939RListNode_ini ( J1939INFO *J1939_info);
static J1939RListNode *J1939RListNode_add(J1939INFO *J1939_info,
										unsigned int fPGN, char fFlag_Connected,
										unsigned int ftimetick, unsigned char fNUM
										);
static J1939RListNode *J1939RListNode_findPGN(J1939INFO *J1939_info, unsigned int J1939PGN);
static J1939RListNode *J1939RListNode_findFrame(J1939INFO *J1939_info);
static J1939RListNode *J1939RListNode_findConnected(J1939INFO *J1939_info);
static J1939RListNode *J1939RListNode_FindPrev(J1939INFO *J1939_info, J1939RListNode *cur_node, char Flag_FirstTime);
//static J1939_Result J1939RListNode_delete(J1939INFO *J1939_info, J1939RListNode *pNode);
static J1939_Result J1939RecieveList_ini ( J1939RListNode *J1939_info);
static J1939RecieveList *J1939RecieveList_add(J1939RListNode *J1939_info,
										unsigned short FrameByteNum, unsigned char FrameNum);
static J1939RecieveList *J1939RecieveListFIFO_Get(J1939RListNode *J1939_info);
static J1939_Result J1939RecieveList_delete(J1939RListNode *J1939_info, J1939RecieveList *pNode);

static void SAEJ1939FrameMail_Set(SAEJ1939_mailParam Frame);
void J1939TransmitProtocol(J1939Unit J1939data);
static J1939_Result IndividualFrame_add(J1939RListNode *pRListNode);
static void ChangeToJ1939Frame(CAN_RX_BUF buf);

/*
 * 函数：邮箱配置参数设置
 * 描述：	1.将应用层需要进行配置的邮箱的配置参数进行保存，之后调用J1939_Start函数进行配置
 * 接口参数：	*mailParam		SAEJ1939_mailParam格式的指针位置
 * 			mail_num		邮箱的个数，不超过32个
 * 返回参数：	设置成功	J1939_TRUE
 * 			设置失败	J1939_FALSE
 * 备注：无
 *
 */
J1939_Result J1939MailParam_Set(SAEJ1939_mailParam *mailParam, char mail_num)
{

	MailParamBuff = heap_calloc(2, 1, sizeof(SAEJ1939_mailParam)*mail_num);
	MailNum = mail_num;
	if(MailParamBuff == NULL)
	{
#ifdef	Debug_SAEJ1939
		printf("J1939MailParam_Set false.\n");
#endif
		return J1939_FALSE;
	}
	memcpy(MailParamBuff, mailParam, sizeof(SAEJ1939_mailParam)*mail_num);

	return J1939_TRUE;

}
/*
 * 函数：J1939链路层启动
 * 描述：	1.其中包括对物理层的配置以及链路层的初始环境建立
 * 接口参数：	无
 * 返回参数：	设置成功	J1939_TRUE
 * 			设置失败	J1939_FALSE
 * 备注：无
 *
 */
J1939_Result J1939_Start(void)
{
	bool state;
	char i;
//管口使能
	CAN2_Port_Ini();
//安装CAN2.0中断服务函数
	state = CAN2RxHandle_Set();
	if(state == 0)
	{
#ifdef	Debug_SAEJ1939
		printf("CAN2RxHandle_Set false.\n");
#endif
		return J1939_FALSE;
	}
	state = CAN2TxHandle_Set();
	if(state == 0)
	{
#ifdef	Debug_SAEJ1939
		printf("CAN2TxHandle_Set false.\n");
#endif
		return J1939_FALSE;
	}
	state = CAN2ErrHandle_Set();
	if(state == 0)
	{
#ifdef	Debug_SAEJ1939
		printf("CAN2ErrHandle_Set false.\n");
#endif
		return J1939_FALSE;
	}

//250KHz
	CAN2Fre_Set(250000, 2, 4, 3);

//配置Can邮箱
	for(i = 0; i < MailNum; i++)
	{
		SAEJ1939FrameMail_Set(MailParamBuff[i]);

	}

//释放邮箱配置信息占用的内存空间
	heap_free(2, MailParamBuff);

//使能CAN
	Can2_Enable();

//邮箱中断使能
	Can2MailInt_Enable();

//以下对需要的数据缓存进行初始化
	info = heap_calloc(2, 1, sizeof(J1939INFO));
	if(info == NULL)
	{
#ifdef	Debug_SAEJ1939
		printf("heap_calloc(2, 1, sizeof(J1939INFO)) false.\n");
#endif
		return J1939_FALSE;
	}

	state = J1939BuffNode_ini(info);
	if(state == 0)
	{
#ifdef	Debug_SAEJ1939
		printf("J1939BuffNode_ini false.\n");
#endif
		return J1939_FALSE;
	}
	state = J1939RListNode_ini(info);
	if(state == 0)
	{
#ifdef	Debug_SAEJ1939
		printf("J1939BuffNode_ini false.\n");
#endif
		return J1939_FALSE;
	}

	J1939FrameBuff = heap_calloc(2, 1, sizeof(J1939Frame_Buff));
	if(J1939FrameBuff == NULL)
	{
#ifdef	Debug_SAEJ1939
		printf("J1939BuffNode_ini false.\n");
#endif
		return J1939_FALSE;
	}

	return J1939_TRUE;

}

/*
 * 函数：SAEJ1939底层邮箱配置
 * 描述：	1.对CAN的邮箱地址等信息进行配置，此处将其配置为符合J1939的报文
 * 		2.内部函数
 * 接口参数：SAEJ1939_mail结构体形式的参数
 * 返回参数：无
 * 备注：无
 *
 */
static void SAEJ1939FrameMail_Set(SAEJ1939_mailParam Frame)
{
	unsigned short ID0 = 0;
	unsigned short ID1 = 0;
	unsigned char Length = 0;

	ID1 = Frame.priority << 10;//优先级
	ID1 |= (Frame.PGN >> 8);
	ID0 = Frame.PS<<8;
	ID0 |= Frame.SA;
	Length = Frame.length;

	if(Frame.TxRx == Rx)
	{
		MailRxID_Set(CAN20b, ID0, ID1, Length);
	}else
	{
		MailTxID_Set(CAN20b, ID0, ID1, Length);
	}

}

/*
 * 函数：SAEJ1939传输协议帧底层邮箱配置
 * 描述：	1.配置用于传输协议帧传输的邮箱的底层配置，该传输协议帧用于连接握手，释放等动作。
 * 		2.配置用于长帧传输的数据帧底层邮箱
 * 接口参数：	Address_souce		源地址
 * 			Address_dest		目标地址
 * 			TxRx				发送还是接收
 * 返回参数：	无
 * 备注：无
 *
 */
void J1939TPMail_Set(unsigned char Address_souce,unsigned char Address_dest, Can_Mode TxRx)
{
	unsigned short ID0 = 0;
	unsigned short ID1 = 0, ID1_1 = 0;
	unsigned char Length = 0;

	ID1 = Priority_TPCM << 10;//优先级
	ID1 |= (PGN_TPCM >> 8);
	ID1_1 = Priority_TPDT << 10;
	ID1_1 |= (PGN_TPDT >> 8);
	ID0 = Address_dest << 8;
	ID0 |= Address_souce;
	Length = 8;

	if(TxRx == Rx)
	{
		MailRxID_Set(CAN20b, ID0, ID1, Length);
		MailRxID_Set(CAN20b, ID0, ID1_1, Length);
	}else
	{
		MailTxID_Set(CAN20b, ID0, ID1, Length);
		MailTxID_Set(CAN20b, ID0, ID1_1, Length);
	}

}

/*
 * 函数：接收J1939的报文
 * 描述：	1.将CAN接收到的数据移入对应的结构体FIFO中；
 * 		2.对CAN通信帧进行处理，得到对应的数据帧；
 * 		3.对接收到整帧的数据帧在相应的接收数量查询列表中数量加（CBMSBuff->Recieve_Table[PGNx]）
 * 接口参数：	无
 * 返回参数：	无
 * 备注：区别J1939数据帧和CAN通信帧，一个数据帧可能包含多个通信帧，对多于8位的数据帧，其首个CAN通信帧的第一个数据为其数据帧的长度
 *
 */
void J1939_Recieve(void)
{
	char DataLength;
	short i, j, k = 0;
	J1939_Result J1939Result;
	J1939RListNode *pNode;
	J1939BuffNode *pBuffNode;
	//一个触发，此处为信号量触发,或者外部触发(此处使用了外部触发)


	//判断CAN 接收缓存是否有数据，有数据则全部读出
	while(CAN2Buff_Is_Empty() == 0)
	{
		//从CANBuff中读出数据
		CAN2Buff_Read(&J1939RX_Buff);
		//将J1939RX_Buff中的数据搬到J1939Frame中
		ChangeToJ1939Frame(J1939RX_Buff);
		//判断是否为传输协议帧，假如是，则进行处理，否则加入链表
		if(J1939Frame.PGN == PGN_TPCM)
		{
			//根据传输协议进行处理
			J1939TransmitProtocol(J1939Frame);

		}else if(J1939Frame.PGN == PGN_TPDT)//长帧传输帧数据
		{
			//查找处于连接状态的PGN
			pNode = J1939RListNode_findConnected(info);
			if(pNode == NULL)
			{
#ifdef	Debug_SAEJ1939
				printf("J1939RListNode_findConnected false.\n");
#endif
			}
			//将数据进行重新封装并加入到缓存链表
			J1939Frame.PGN = pNode->PGN;
			//接收数据加入缓存链表
			pBuffNode = J1939BuffNode_add(info, J1939Frame);
			if(pBuffNode == NULL)
			{
#ifdef	Debug_SAEJ1939
				printf("J1939BuffNode_add false.\n");
#endif
			}

		}else//以下为单帧情况
		{
			//接收数据加入缓存链表
			pBuffNode = J1939BuffNode_add(info, J1939Frame);

			if(pBuffNode == NULL)
			{
#ifdef	Debug_SAEJ1939
				printf("J1939BuffNode_add false.\n");
#endif
			}
			/*
			 * 以下对控制信息进行处理
			 */
			pNode = J1939RListNode_findPGN(info, J1939Frame.PGN);
			IndividualFrame_add(pNode);//单独帧添加进缓存并修改状态链表
		}
	}
}

/*
 * 函数：格式转换
 * 描述：	1.将CAN接收到的数据保存为J1939Unit格式的数据
 * 接口参数：	buf			CAN接收到的数据
 * 返回参数：	无
 * 备注：无
 *
 */
static void ChangeToJ1939Frame(CAN_RX_BUF buf)
{

	J1939Frame.PGN = (unsigned int)(buf.Mailbox_ID1 & 0x00FF) << 8;//提取PGN
	J1939Frame.Length = buf.Length;
	J1939Frame.Data[0] = (unsigned char)(buf.Data0 >> 8);
	J1939Frame.Data[1] = (unsigned char)(buf.Data0);
	J1939Frame.Data[2] = (unsigned char)(buf.Data1 >> 8);
	J1939Frame.Data[3] = (unsigned char)(buf.Data1);
	J1939Frame.Data[4] = (unsigned char)(buf.Data2 >> 8);
	J1939Frame.Data[5] = (unsigned char)(buf.Data2);
	J1939Frame.Data[6] = (unsigned char)(buf.Data3 >> 8);
	J1939Frame.Data[7] = (unsigned char)(buf.Data3);

}

/*
 * 函数：J1939的传输协议控制函数
 * 描述：	1.进行握手释放等操作；
 * 		2.此处由于采用侦听模式，因此部分过程不实现；
 * 		3.内部主要对接收状态查询链表进行数据操作，外部通过对链表的查询可以知道链路是否正在连接，接收是否释放以及数据帧数量及长度等。
 * 接口参数：	J1939data			接收到的J1939Unit格式的数据
 * 返回参数：	无
 * 备注：无
 *
 */
void J1939TransmitProtocol(J1939Unit J1939data)
{
	Transmit_Mode Tmode;
	unsigned int Frame_PGN = 0;
	J1939RListNode *pNode;
	J1939RecieveList *pList;
	J1939_Result J1939Result;
	unsigned short fFrameByte_Num;

	Tmode = (Transmit_Mode)J1939data.Data[0];
	Frame_PGN = ((unsigned int)J1939data.Data[5]) << 0;//LSB
	Frame_PGN |= ((unsigned int)J1939data.Data[6]) << 8;
	Frame_PGN |= ((unsigned int)J1939data.Data[7]) << 16;//MSB
	switch(Tmode)
	{
		case TPCM_RTS://请求发送
			/*将收到的PGN对接收链表进行登记，用于后续判断接收到的帧是否为短帧*/
			pNode = J1939RListNode_findPGN(info, Frame_PGN);
			if(pNode == NULL)//判断是否在接收列表中存在，不存在时则新加链表节点
			{
				pNode = J1939RListNode_add(info, Frame_PGN, J1939_Connected, 0, 0);
				if(pNode == NULL)
				{
#ifdef	Debug_SAEJ1939
				printf("J1939RListNode_add false.\n");
#endif
				}
			}else
			{
				pNode->Flag_Connected = J1939_Connected;

			}
			break;
		case TPCM_CTS://准备发送
			//不处理

			break;
		case TPCM_EndofMsgAsk://结束应答
			/*查找对应PGN所在的链表位置*/
			pNode = J1939RListNode_findPGN(info, Frame_PGN);
			if(pNode == NULL)//判断是否在接收列表中存在，不存在则表示错误
			{
#ifdef	Debug_SAEJ1939
				printf("TPCMEndofMsgAsk false.\n");
#endif
			}else
			{
				pNode->Flag_Connected = 0;		//连接结束
				//此处资料没有说明字节是大端还是小端，因此未验证,大概率为大端，此次采用大端
				fFrameByte_Num = ((unsigned int)J1939data.Data[1]);
				fFrameByte_Num |= ((unsigned int)J1939data.Data[2]) << 8;
				pNode->NUM ++;//接收到的帧数量加一
				//加参数加入相应链表
				pList = J1939RecieveList_add(pNode, fFrameByte_Num, J1939data.Data[3]);
				if(pList == NULL)
				{
#ifdef	Debug_SAEJ1939
				printf("TPCMEndofMsgAsk false.\n");
#endif
				}
			}
			break;
		case TPCM_Abort://放弃连接
			/*查找对应PGN所在的链表位置*/
			pNode = J1939RListNode_findPGN(info, Frame_PGN);
			if(pNode == NULL)//判断是否在接收列表中存在，不存在则表示错误
			{
#ifdef	Debug_SAEJ1939
				printf("TPCMAbort false.\n");
#endif
			}else
			{
				pNode->Flag_Connected = 0;		//连接结束
			}
			break;
		case TPCM_BAM://广播公告消息

			break;
		default:
			break;
	}


}

/*
 * 函数：单独帧添加
 * 描述：	1.对于数据帧只由单帧完成的单独帧进行数据状态查询链表的修改；
 * 接口参数：	*pRListNode			数据状态查询链表的位置
 * 返回参数：	设置成功	J1939_TRUE
 * 			设置失败	J1939_FALSE
 * 备注：无
 *
 */
static J1939_Result IndividualFrame_add(J1939RListNode *pRListNode)
{
	J1939_Result J1939Result;
	J1939RListNode *pNode = pRListNode;
	J1939RecieveList *pList;

	if(pNode == NULL)	//如果接收标记链表为空，则创建新的节点
	{
		//加入接收链表节点
		pNode = J1939RListNode_add(info, J1939Frame.PGN, J1939_Unconnected, 0, 0);
		if(pNode == NULL)
		{
#ifdef	Debug_SAEJ1939
			printf("IndividualFrame_add false.\n");
#endif
			return J1939_FALSE;
		}
	}
	//创建新的接收链表
	pNode->NUM++;
	pList = J1939RecieveList_add(pNode, J1939Frame.Length, 1);
	if(pList == NULL)
	{
#ifdef	Debug_SAEJ1939
		printf("IndividualFrame_add false.\n");
#endif
		return J1939_FALSE;
	}


	return J1939_TRUE;
}

/*
 * 函数：帧接收数量查询
 * 描述：	1.对数据状态查询链表进行遍历，统计接收到的总的数据帧数量；
 * 接口参数：	无
 * 返回参数：	整型数据		帧数量
 * 备注：无
 *
 */
int J1939FrameNum_Recieve(void)
{
	int Num = 0;
	J1939RListNode *pNode = NULL;

	//遍历链表中的接收帧数，并全部加起来
	for(pNode = J1939RListNode_FindPrev(info, pNode, J7939_FirstTime);
		pNode != NULL;
		pNode = J1939RListNode_FindPrev(info, pNode, NotFirstTime)
		)
	{
		Num += pNode->NUM;
	}

	return Num;
}

/*
 * 函数：J1939数据帧提取
 * 描述：	1.从链表总提取出第一个完整的数据帧；
 * 接口参数：	无
 * 返回参数：	J1939Frame_Buff指针参数		存放帧数据的地址
 * 备注：无
 *
 */
J1939Frame_Buff *J1939Frame_get(void)
{
	unsigned char FrameNO_ = 0;
	J1939RecieveList *pList;
	J1939BuffNode *pBuffNode;
	J1939RListNode *pNode;
	J1939_Result J1939Result;
	unsigned short FrameNO_Now;

	//获取第一个存有帧的地址，提取其中的控制信息
	pNode = J1939RListNode_findFrame(info);
	pList = J1939RecieveListFIFO_Get(pNode);


	//判断是长帧或者短帧
	//短帧情况，直接赋值保存，同时处理相应的控制信息
	if(pList->Frame_Num == 1)
	{
		//获取对应PGN的缓存帧
		pBuffNode = J1939BuffNode_find(info, pNode->PGN);
		//保存信息
		J1939FrameBuff->PGN = pBuffNode->Data.PGN;
		J1939FrameBuff->Length = pBuffNode->Data.Length;
		for(int i = 0; i < J1939FrameBuff->Length; i++)
		{
			J1939FrameBuff->Buff[i] = pBuffNode->Data.Data[i];
		}

		//进行控制信息的处理
		pNode->NUM--;
		J1939Result = J1939RecieveList_delete(pNode, pList);
		if(J1939Result == J1939_FALSE)
		{
#ifdef	Debug_SAEJ1939
		printf("J1939Frame_get false.\n");
#endif
		return NULL;
		}

		//删除缓存节点
		J1939Result = J1939BuffNode_delete(info, pBuffNode);
		if(J1939Result == J1939_FALSE)
		{
#ifdef	Debug_SAEJ1939
		printf("J1939BuffNode_delete false.\n");
#endif
		}
		//返回地址指针
		return J1939FrameBuff;
	}
	//长帧情况
	while(FrameNO_ != pList->Frame_Num)		//接收到的帧序号等于全部帧数量时退出循环
	{
		//获取对应PGN的缓存帧
		pBuffNode = J1939BuffNode_find(info, pNode->PGN);
		if(pBuffNode == NULL)
		{
#ifdef	Debug_SAEJ1939
			printf("J1939Frame_Get J1939BuffNode_find false.\n");
#endif
		}

		//提取帧序号
		FrameNO_ = pBuffNode->Data.Data[0];
		FrameNO_Now = (FrameNO_ - 1) * 7;//此处为接收数组的基本偏移量
		//保存信息
		if(FrameNO_ == 1)//第一帧的时候保存对应的PGN和长度
		{
			J1939FrameBuff->PGN = pBuffNode->Data.PGN;
			J1939FrameBuff->Length = pList->FrameByte_Num;//从接收列表中获取帧的总长度
		}
		//保存信息,当存在放弃连接时，buff中会有上次的数据，此时因为未接收完成，其数据会被后面同编号的数据给覆盖
		for(int i = 1; i < J1939FrameBuff->Length; i++)
		{
			J1939FrameBuff->Buff[FrameNO_Now + i - 1] = pBuffNode->Data.Data[i];
		}
		//删除缓存节点
		J1939Result = J1939BuffNode_delete(info, pBuffNode);
		if(J1939Result == J1939_FALSE)
		{
#ifdef	Debug_SAEJ1939
		printf("J1939BuffNode_delete false.\n");
#endif
		return NULL;
		}
		//当接收到最后一帧的情况
		if(FrameNO_ == pList->Frame_Num)
		{
			//进行控制信息的处理
			pNode->NUM--;
			J1939Result = J1939RecieveList_delete(pNode, pList);
			if(J1939Result == J1939_FALSE)
			{
	#ifdef	Debug_SAEJ1939
			printf("J1939Frame_get false.\n");
	#endif
			return NULL;
			}

		}

	}

	return J1939FrameBuff;

}

/*
 * 函数：J1939触发事件
 * 描述：	1.数据接收的触发事件；
 * 接口参数：	无
 * 返回参数：	无
 * 备注：采样UCOS时采用信号量进行触发
 *
 */
void J1939SemTrigger(void)
{


#ifdef J1939_UCOS
	OS_ERR err;
	//等待信号量，该信号量由物理层CAN发送触发，触发条件为CAN接收到J1939的相关数据
	OSSemPend(&g_Sem_CBMS,0u, OS_OPT_PEND_BLOCKING, NULL, &err);

#endif

}


/*
 *以下都为链表操作
 */
/*
 * to ini the J1939_info->BuffHead,all the link list node to be found are based on it.
 */
static J1939_Result J1939BuffNode_ini ( J1939INFO *J1939_info)
{
	J1939BuffNode *Node = NULL;	/* assume failure	*/

	/* All higher level linked lists must be initialized.	*/
	Node = (J1939BuffNode *) heap_malloc (2, sizeof (J1939BuffNode));
	if(Node == NULL)
	{
		return J1939_FALSE;
	}
	/*set the value of memory to 0*/
	memset(Node, 0, sizeof(J1939BuffNode));
	/* Add DO to front of DO List in first entry of LNType list	*/
	list_add_first ((DBL_LNK**)&(J1939_info->BuffHead), (DBL_LNK*)Node);

	return J1939_TRUE;

}

static J1939BuffNode *J1939BuffNode_add(J1939INFO *J1939_info, J1939Unit J1939data)
{
	J1939BuffNode *Node = NULL;	/* assume failure	*/

	/* All higher level linked lists must be initialized.	*/
	Node = (J1939BuffNode *) heap_malloc (2, sizeof (J1939BuffNode));
	if(Node == NULL)
	{
		return NULL;
	}
//	J1939Debug_Count ++;
	/*memory copy*/
	memcpy(&(Node->Data), &J1939data, sizeof(J1939Unit));
	/* Add DO to front of DO List in first entry of LNType list	*/
	list_add_first ((DBL_LNK**)&J1939_info->BuffHead, (DBL_LNK*)Node);

	return Node;

}

static J1939BuffNode *J1939BuffNode_find(J1939INFO *J1939_info, unsigned int J1939PGN)
{
	J1939BuffNode *pNode = NULL;

	pNode = (J1939BuffNode *) list_find_last ((DBL_LNK *) J1939_info->BuffHead);

	for (pNode = (J1939BuffNode *) list_find_last ((DBL_LNK *) J1939_info->BuffHead);
			pNode != NULL;
			pNode = (J1939BuffNode *) list_find_prev ((DBL_LNK *) J1939_info->BuffHead, (DBL_LNK *) pNode))
	{
		if (pNode->Data.PGN == J1939PGN)
			return (pNode);
	}
	return (NULL);	/* match not found	*/

}

static J1939_Result J1939BuffNode_delete(J1939INFO *J1939_info, J1939BuffNode *pNode)
{

	list_unlink((DBL_LNK **)&J1939_info->BuffHead, (DBL_LNK *)pNode);
	heap_free(2, pNode);

	return J1939_TRUE;
}

/*
 * to ini the J1939_info->RListHead,all the link list node to be found are based on it.
 */
static J1939_Result J1939RListNode_ini ( J1939INFO *J1939_info)
{
	J1939RListNode *Node = NULL;	/* assume failure	*/

	/* All higher level linked lists must be initialized.	*/
	Node = (J1939RListNode *) heap_malloc (2, sizeof (J1939RListNode));
	if(Node == NULL)
	{
		return J1939_FALSE;
	}
	/*set the value of memory to 0*/
	memset(Node, 0, sizeof(J1939RListNode));
	/* Add DO to front of DO List in first entry of LNType list	*/
	list_add_first ((DBL_LNK**)&J1939_info->RListHead, (DBL_LNK*)Node);

	return J1939_TRUE;

}

static J1939RListNode *J1939RListNode_add(J1939INFO *J1939_info,
										unsigned int fPGN, char fFlag_Connected,
										unsigned int ftimetick, unsigned char fNUM
										)
{
	J1939RListNode *Node = NULL;	/* assume failure	*/
	J1939_Result state;

	/* All higher level linked lists must be initialized.	*/
	Node = (J1939RListNode *) heap_calloc (2, 1, sizeof (J1939RListNode));
	if(Node == NULL)
	{
		return NULL;
	}
	/*memory copy*/
	Node->PGN = fPGN;
	Node->Flag_Connected = fFlag_Connected;
	Node->timetick = ftimetick;
	Node->NUM = fNUM;
	state = J1939RecieveList_ini(Node);//此处初始化下一个树枝的头
	if(state == 0)
	{
#ifdef	Debug_SAEJ1939
		printf("J1939BuffNode_ini false.\n");
#endif
		return NULL;
	}
	/* Add DO to front of DO List in first entry of LNType list	*/
	list_add_first ((DBL_LNK**)&J1939_info->RListHead, (DBL_LNK*)Node);

	return (Node);

}

static J1939RListNode *J1939RListNode_findPGN(J1939INFO *J1939_info, unsigned int J1939PGN)
{
	J1939RListNode *pNode = NULL;

	for (pNode = (J1939RListNode *) list_find_last ((DBL_LNK *) J1939_info->RListHead);
			pNode != NULL;
			pNode = (J1939RListNode *) list_find_prev ((DBL_LNK *) J1939_info->RListHead, (DBL_LNK *) pNode))
	{
		if (pNode->PGN == J1939PGN)
			return (pNode);
	}
	return (NULL);	/* match not found	*/
}

static J1939RListNode *J1939RListNode_findFrame(J1939INFO *J1939_info)
{
	J1939RListNode *pNode = NULL;

	for (pNode = (J1939RListNode *) list_find_last ((DBL_LNK *) J1939_info->RListHead);
			pNode != NULL;
			pNode = (J1939RListNode *) list_find_prev ((DBL_LNK *) J1939_info->RListHead, (DBL_LNK *) pNode))
	{
		if (pNode->NUM > 0)
			return (pNode);
	}
	return (NULL);	/* match not found	*/
}

static J1939RListNode *J1939RListNode_findConnected(J1939INFO *J1939_info)
{
	J1939RListNode *pNode = NULL;

	for (pNode = (J1939RListNode *) list_find_last ((DBL_LNK *) J1939_info->RListHead);
			pNode != NULL;
			pNode = (J1939RListNode *) list_find_prev ((DBL_LNK *) J1939_info->RListHead, (DBL_LNK *) pNode))
	{
		if (pNode->Flag_Connected == J1939_Connected)	//查找处于连接的位置，同一目标地址尤其只有一个。
			return (pNode);
	}
	return (NULL);	/* match not found	*/
}
//static J1939_Result J1939RListNode_delete(J1939INFO *J1939_info, J1939RListNode *pNode)
//{
//
//	list_unlink((DBL_LNK **)&J1939_info->RListHead, (DBL_LNK *)pNode);
//	heap_free(2, pNode);
//
//	return J1939_TRUE;
//}

static J1939RListNode *J1939RListNode_FindPrev(J1939INFO *J1939_info, J1939RListNode *cur_node, char Flag_FirstTime)
{
	J1939RListNode *pNode = cur_node;
	if(Flag_FirstTime == 1)
	{
		pNode = (J1939RListNode *) list_find_last ((DBL_LNK *) J1939_info->RListHead);
	}
	pNode = (J1939RListNode *) list_find_prev ((DBL_LNK *) J1939_info->RListHead, (DBL_LNK *) pNode);

	return (pNode);
}

/*
 * to ini the J1939_info->RListHead,all the link list node to be found are based on it.
 */
static J1939_Result J1939RecieveList_ini ( J1939RListNode *J1939_info)
{
	J1939RecieveList *Node = NULL;	/* assume failure	*/

	/* All higher level linked lists must be initialized.	*/
	Node = (J1939RecieveList *) heap_malloc (2, sizeof (J1939RecieveList));
	if(Node == NULL)
	{
		return J1939_FALSE;
	}
	/*set the value of memory to 0*/
	memset(Node, 0, sizeof(J1939RecieveList));
	/* Add DO to front of DO List in first entry of LNType list	*/
	list_add_first ((DBL_LNK**)&J1939_info->RLInfoHead, (DBL_LNK*)Node);

	return J1939_TRUE;

}

static J1939RecieveList *J1939RecieveList_add(J1939RListNode *J1939_info,
										unsigned short FrameByteNum, unsigned char FrameNum)
{
	J1939RecieveList *Node = NULL;	/* assume failure	*/

	/* All higher level linked lists must be initialized.	*/
	Node = (J1939RecieveList *) heap_calloc (2, 1, sizeof (J1939RecieveList));
	if(Node == NULL)
	{
		return NULL;
	}
	/*memory copy*/
	Node->FrameByte_Num = FrameByteNum;
	Node->Frame_Num = FrameNum;
	/* Add DO to front of DO List in first entry of LNType list	*/
	list_add_first ((DBL_LNK**)&J1939_info->RLInfoHead, (DBL_LNK*)Node);

	return (Node);

}

static J1939RecieveList *J1939RecieveListFIFO_Get(J1939RListNode *J1939_info)//此处需要再向前移一个单位，因为最后一个节点为初始节点。
{
	J1939RecieveList *pNode = NULL;

	pNode = (J1939RecieveList *) list_find_last ((DBL_LNK *) J1939_info->RLInfoHead);
	pNode = (J1939RecieveList*)(pNode->l.prev);

	return (pNode);
}

static J1939_Result J1939RecieveList_delete(J1939RListNode *J1939_info, J1939RecieveList *pNode)
{

	list_unlink((DBL_LNK **)&J1939_info->RLInfoHead, (DBL_LNK *)pNode);
	heap_free(2, pNode);

	return J1939_TRUE;
}







