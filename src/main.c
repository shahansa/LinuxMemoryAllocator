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
	void *ptr[10];
	int i;
	for(i = 1; i <= 10; i++)
	{
		ptr[i] = u_malloc(i*1024);
		if(NULL == ptr)
			 printf("malloc failed");
	}
	u_free(ptr[5]);
	u_free(ptr[2]);
	void *ptri = u_malloc(1024);
	void *ptr2 = u_malloc(2048);

		
	return 0;	
}
