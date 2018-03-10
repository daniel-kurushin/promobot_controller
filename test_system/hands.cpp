#include <stdio.h>
#include "hands.hpp"

void processHands(char *resp_buf, int cmd)
{
	
	sprintf(resp_buf, "%d %d", cmd, 0);
}