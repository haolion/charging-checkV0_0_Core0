
#include "Data_conversion.h"




int Complement18ToInt32(unsigned int num)
{
	unsigned int i = num;
	unsigned int j = 0x20000;//第十八位
	int k;

	if(num & 0x20000)		//最高位假如是1，则为负数，否则为正数
	{
		num = num | 0xFFFE0000;
		return (int)num;

	}else
	{
		return (int)i;
	}
}














