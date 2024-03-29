#include "cfifo.h"

CFIFO_CREATE(test_non_static, uint8_t, 8);
CFIFO_CREATE_STATIC(test_static, uint8_t, 8);

int main(void)
{
	CFIFO_CREATE_STATIC(fifo, uint8_t, 16);
	(void) fifo;
	(void) test_non_static;
	(void) test_static;
	return 0;
}
