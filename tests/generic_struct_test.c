#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "cfifo_generic.h"

struct test {
    uint8_t a;
    uint16_t b;
    uint32_t c;
    uint8_t *d;
};

int main(void)
{
    CFIFO_GENERIC_CREATE_STATIC(fifo, struct test, 16);
    uint8_t b = 4;
    struct test s;
    struct test h;

    assert(CFIFO_BUF_SIZE(sizeof(struct test), 7) == -1);
    assert(CFIFO_BUF_SIZE(sizeof(struct test), 16) == (sizeof(struct test) * 16));
    assert((fifo->num_items_mask + 1) == 16);

    s.a = 1;
    s.b = 2;
    s.c = 3;
    s.d = &b;

    assert(cfifo_generic_push(fifo, &s) == CFIFO_SUCCESS);
    assert(cfifo_generic_size(fifo) == 1);
    assert(cfifo_generic_pop(fifo, &h) == CFIFO_SUCCESS);
    assert(h.a == s.a);
    assert(h.b == s.b);
    assert(h.c == s.c);
    assert(h.d == s.d);

    return 0;

}
