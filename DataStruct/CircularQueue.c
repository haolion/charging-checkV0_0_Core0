#include "CircularQueue.h"

/*以下用于一般形式的FIFO*/
void CircularQ_init(CircularQ *CircularQ_Buff, int Q_Num)
{
	CircularQ_Buff->head = 0;
	CircularQ_Buff->tail = 0;
	CircularQ_Buff->tag = 0;
	CircularQ_Buff->num = Q_Num;
	CircularQ_Buff->length = 0;

}
void CircularQ_Free(CircularQ *CircularQ_Buff)
{

}
void CircularQ_Push(CircularQ *CircularQ_Buff)
{

//	//判断队列是否填满
//	if(CircularQ_is_Full(CircularQ_Buff) == 1)
//	{
//
//		return -1;
//	}

	CircularQ_Buff->tail = (CircularQ_Buff->tail + 1) % CircularQ_Buff->num;
	CircularQ_Buff->length++;

	if(CircularQ_Buff->head == CircularQ_Buff->tail)
	{
		CircularQ_Buff->tail = 1;
	}


}
void CircularQ_Poll(CircularQ *CircularQ_Buff)
{

//	//判断队列是否为空
//	if(CircularQ_is_Empty(CircularQ_Buff) == 1)
//	{
//
//		return -1;
//	}


	CircularQ_Buff->head = (CircularQ_Buff->head + 1) % CircularQ_Buff->num;
	CircularQ_Buff->length--;

	if(CircularQ_Buff->head == CircularQ_Buff->tail)
	{
		CircularQ_Buff->tag = 0;
	}

}


/*以下用于内带浮点型数组的FIFO形式*/
int CircularQF_init(CircularQ_Float *CircularQF_Buff)
{
	CircularQF_Buff->head = 0;
	CircularQF_Buff->tail = 0;
	CircularQF_Buff->tag = 0;
	CircularQF_Buff->num = CircularQF_Max;

	return 0;

}
int CircularQF_Free(CircularQ_Float *CircularQF_Buff)
{
	return 0;

}
int	CircularQF_Push(CircularQ_Float *CircularQF_Buff, float Data)
{

	//判断队列是否填满
	if(CircularQF_is_Full(CircularQF_Buff) == 1)
	{

		return -1;
	}

	CircularQF_Buff->Data[CircularQF_Buff->tail] = Data;
	CircularQF_Buff->tail = (CircularQF_Buff->tail + 1) % CircularQF_Buff->num;

	if(CircularQF_Buff->head == CircularQF_Buff->tail)
	{
		CircularQF_Buff->tail = 1;
	}

	return 0;

}
int	CircularQF_Poll(CircularQ_Float *CircularQF_Buff, float *Data)
{

	//判断队列是否为空
	if(CircularQF_is_Empty(CircularQF_Buff) == 1)
	{

		return -1;
	}

	*Data = CircularQF_Buff->Data[CircularQF_Buff->head];
	CircularQF_Buff->head = (CircularQF_Buff->head + 1) % CircularQF_Buff->num;

	if(CircularQF_Buff->head == CircularQF_Buff->tail)
	{
		CircularQF_Buff->tag = 0;
	}

	return 0;

}
