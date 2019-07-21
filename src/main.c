/* file : main.c
 * description : driver calls for all the functions
 * author:
 * acknowledgements:
 * date modified :
 */



#include "../include/common.h"
#include "../include/commonFunctions.h"

int main()
{
	void *ptr;
	int i;
	for(i = 1; i < 10; i++)
	{
		ptr = u_malloc(i*1024);
		if(NULL == ptr)
			 printf("malloc failed");
	}
	return 0;	
}
