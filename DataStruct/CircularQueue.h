
#ifndef CIRCULARQUEUE_H
#define CIRCULARQUEUE_H

#define CircularQF_Max	100
typedef struct CircularQ_Float
{
	int head;
	int tail;
	int tag;
	int num;
	float Data[CircularQF_Max];

}CircularQ_Float;

//#define CircularQ_Max	1024
typedef struct CircularQ
{
	volatile int head;			//FIFO的头部，先出
	volatile int tail;			//FIFO的尾部，进
	volatile int tag;			//FIFO溢出标志
	volatile int num;			//FIFO的总容量
	volatile int length;			//FIFO的实际长度

}CircularQ;
/*以下用于一般形式的FIFO*/
void CircularQ_init(CircularQ *CircularQ_Buff, int Q_Num);
void CircularQ_Free(CircularQ *CircularQF_Buff);
void CircularQ_Push(CircularQ *CircularQF_Buff);
void CircularQ_Poll(CircularQ *CircularQF_Buff);

#define CircularQ_is_Empty(q)	((q.head == q.tail) && (q.tag == 0))
#define CircularQ_is_Full(q)	((q.head == q.tail) && (q.tag == 1))


/*以下用于内带浮点型数组的FIFO形式*/
int CircularQF_init(CircularQ_Float *CircularQF_Buff);
int CircularQF_Free(CircularQ_Float *CircularQF_Buff);
int	CircularQF_Push(CircularQ_Float *CircularQF_Buff, float Data);
int	CircularQF_Poll(CircularQ_Float *CircularQF_Buff, float *Data);

#define CircularQF_is_Empty(q)	((q->head == q->tail) && (q->tag == 0))
#define CircularQF_is_Full(q)	((q->head == q->tail) && (q->tag == 1))




#endif
