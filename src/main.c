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
	ptr = u_malloc(1024);
	if(NULL == ptr)
		 printf("malloc failed");
	return 0;	
}
