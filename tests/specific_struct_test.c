#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

struct test {
    uint8_t a;
    uint16_t b;
    uint32_t c;
    uint8_t *d;
};

#define CFIFO_SPECIFIC_TYPE struct test
#define CFIFO_SPECIFIC_USE_MEMCPY 1
#include "cfifo_specific.c"

int main(void)
{
    CFIFO_SPECIFIC_CREATE_STATIC(fifo, 16);
    uint8_t b = 4;
    struct test s;
    struct test h;

    s.a = 1;
    s.b = 2;
    s.c = 3;
    s.d = &b;

    assert(cfifo_specific_push(fifo, &s) == CFIFO_SUCCESS);
    assert(cfifo_specific_size(fifo) == 1);
    assert(cfifo_specific_pop(fifo, &h) == CFIFO_SUCCESS);
    assert(h.a == s.a);
    assert(h.b == s.b);
    assert(h.c == s.c);
    assert(h.d == s.d);

    return 0;

}
