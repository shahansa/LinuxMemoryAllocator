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
	printf("In Function: %s ,line %d\n",__func__, __LINE__ );
	void *ptr[10];
	int i;
	for(i = 1; i <= 10; i++)
	{
		ptr[i] = u_malloc(i*1024 * 128);
		if(NULL == ptr)
			 printf("malloc failed");
	}
	travers_and_print();
	u_free(ptr[5]);
	u_free(ptr[2]);	
	printf("Outside Function: %s ,line %d\n",__func__, __LINE__ );
	u_malloc(128 *1024);
	u_malloc(4*128*1024);
	return 0;	
}
