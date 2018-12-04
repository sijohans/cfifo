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

void struct_test(void)
{
    uint8_t b = 4;
    struct test s;
    struct test h;
    CFIFO_GENERIC_CREATE(fifo, struct test, 16);
    assert(CFIFO_BUF_SIZE(sizeof(struct test), 7) == -1);
    assert(CFIFO_BUF_SIZE(sizeof(struct test), 16) == (sizeof(struct test) * 16));
    assert((fifo->num_items_mask + 1) == 16);

    s.a = 1;
    s.b = 2;
    s.c = 3;
    s.d = &b;

    assert(cfifo_put(fifo, &s) == CFIFO_SUCCESS);
    assert(cfifo_size(fifo) == 1);
    assert(cfifo_get(fifo, &h) == CFIFO_SUCCESS);
    assert(h.a == s.a);
    assert(h.b == s.b);
    assert(h.c == s.c);
    assert(h.d == s.d);

}

void contains_test(void) {

    uint8_t b = 4;
    struct test s;
    struct test h;

    CFIFO_GENERIC_CREATE_STATIC(fifo, struct test, 16);

    s.a = 1;
    s.b = 2;
    s.c = 3;
    s.d = &b;

    assert(cfifo_contains(fifo, NULL) == 0);
    assert(cfifo_contains(NULL, NULL) == 0);
    assert(cfifo_contains(NULL, &s) == 0);

    assert(cfifo_put(fifo, &s) == CFIFO_SUCCESS);
    assert(cfifo_contains(fifo, &s) == 1);
    assert(cfifo_put(fifo, &s) == CFIFO_SUCCESS);
    assert(cfifo_put(fifo, &s) == CFIFO_SUCCESS);
    assert(cfifo_contains(fifo, &s) == 3);
    s.a = 3;
    assert(cfifo_contains(fifo, &s) == 0);

    assert(cfifo_get(fifo, &h) == CFIFO_SUCCESS);
    assert(h.a == 1);
    assert(h.b == 2);
    assert(h.c == 3);
    assert(h.d == &b);
}

void malloc_buffer_test(void)
{
    uint32_t *buffer = (uint32_t *) malloc(sizeof(uint32_t) * 4);
    assert(buffer != NULL);
    struct cfifo_generic sfifo;
    cfifo_t fifo = &sfifo;
    int ret = cfifo_init(fifo, buffer, 4, sizeof(uint32_t), (sizeof(uint32_t) * 4));
    assert(ret == CFIFO_SUCCESS);
    uint32_t a = 1;
    assert(cfifo_put(fifo, &a) == CFIFO_SUCCESS); a++;
    assert(cfifo_put(fifo, &a) == CFIFO_SUCCESS); a++;
    assert(cfifo_put(fifo, &a) == CFIFO_SUCCESS); a++;
    assert(cfifo_put(fifo, &a) == CFIFO_SUCCESS); a++;
    assert(cfifo_put(fifo, &a) == CFIFO_ERR_FULL);
    a = 4;
    assert(cfifo_contains(fifo, &a) == 1);
    uint32_t b = 1;
    assert(cfifo_get(fifo, &a) == CFIFO_SUCCESS); assert(a == b); b++;
    assert(cfifo_get(fifo, &a) == CFIFO_SUCCESS); assert(a == b); b++;
    assert(cfifo_get(fifo, &a) == CFIFO_SUCCESS); assert(a == b); b++;
    assert(cfifo_get(fifo, &a) == CFIFO_SUCCESS); assert(a == b); b++;
    free(buffer);
}

int main(void)
{

    uint8_t a;
    uint8_t b;
    size_t size;
    size_t i;

    uint8_t rdata[16];
    uint8_t data[16];

    struct_test();
    contains_test();
    malloc_buffer_test();

    CFIFO_GENERIC_CREATE(fifo, uint8_t, 16);
    assert(cfifo_available(fifo) == 16);
    assert(cfifo_peek(fifo, &a) == CFIFO_ERR_EMPTY);

    /* Empty queue from stat */
    assert(cfifo_size(fifo) == 0);
    assert(cfifo_get(fifo, &b) == CFIFO_ERR_EMPTY);

    /* One queue item */
    a = 1;
    assert(cfifo_put(fifo, &a) == CFIFO_SUCCESS);
    assert(cfifo_size(fifo) == 1);
    assert(cfifo_get(fifo, &b) == CFIFO_SUCCESS);
    assert(b == a);
    assert(cfifo_size(fifo) == 0);

    /* Fill and empty queue */
    for (i = 0; i < 16; i++)
    {
        a = (uint8_t) i;
        assert(cfifo_put(fifo, &a) == CFIFO_SUCCESS);
        assert(cfifo_size(fifo) == (i + 1));
    }

    assert(cfifo_put(fifo, &a) == CFIFO_ERR_FULL);

    for (i = 0; i < 16; i++)
    {
        assert(cfifo_get(fifo, &a) == CFIFO_SUCCESS);
        assert(a == i);
        assert(cfifo_size(fifo) == 15 - i);
    }

    assert(cfifo_get(fifo, &b) == CFIFO_ERR_EMPTY);

    for (i = 0; i < 16; i++)
    {
        data[i] = i + 1;
    }

    size = 16;

    assert(cfifo_write(fifo, data, &size) == CFIFO_SUCCESS);
    assert(size == 16);
    assert(cfifo_size(fifo) == 16);


    size = 16;
    assert(cfifo_read(fifo, rdata, &size) == CFIFO_SUCCESS);
    for (i = 0; i < 16; i++)
    {
        assert(rdata[i] == data[i]);
    }
    assert(size == 16);
    assert(cfifo_size(fifo) == 0);

    size = 13;

    assert(cfifo_write(fifo, data, &size) == CFIFO_SUCCESS);
    assert(size == 13);
    assert(cfifo_size(fifo) == 13);

    assert(cfifo_get(fifo, &a) == CFIFO_SUCCESS);
    assert(a == data[0]);
    assert(cfifo_get(fifo, &a) == CFIFO_SUCCESS);
    assert(a == data[1]);
    assert(cfifo_size(fifo) == 11);

    size = 7;
    assert(cfifo_read(fifo, rdata, &size) == CFIFO_SUCCESS);
    assert(size == 7);
    for (i = 0; i < 7; i++)
    {
        assert(rdata[i] == data[2 + i]);
    }
    assert(cfifo_size(fifo) == 4);

    /* Simulate fifo counter overflow */
    fifo->write_pos = UINT32_MAX;
    fifo->read_pos = UINT32_MAX;
    assert(cfifo_size(fifo) == 0);

    /* One queue item */
    a = 67;
    assert(cfifo_put(fifo, &a) == CFIFO_SUCCESS);
    assert(cfifo_size(fifo) == 1);
    assert(cfifo_peek(fifo, &b) == CFIFO_SUCCESS);
    assert(b == a);
    assert(cfifo_size(fifo) == 1);
    assert(cfifo_get(fifo, &b) == CFIFO_SUCCESS);
    assert(b == a);
    assert(cfifo_size(fifo) == 0);

    for (i = 0; i < 16; i++)
    {
        data[i] = i + 1;
    }

    size = 17;

    assert(cfifo_write(fifo, data, &size) == CFIFO_SUCCESS);
    assert(size == 16);
    assert(cfifo_size(fifo) == 16);
    assert(cfifo_flush(fifo) == CFIFO_SUCCESS);
    assert(cfifo_size(fifo) == 0);

    /* Simulate fifo counter overflow */
    fifo->write_pos = 0 - 5;
    fifo->read_pos = 0 - 5;
    for (i = 0; i < 16; i++)
    {
        a = (uint8_t) i;
        assert(cfifo_put(fifo, &a) == CFIFO_SUCCESS);
        assert(cfifo_size(fifo) == (i + 1));
    }

    for (i = 0; i < 16; i++)
    {
        assert(cfifo_size(fifo) == 16 - i);
        assert(cfifo_get(fifo, &a) == CFIFO_SUCCESS);
    }

    assert(cfifo_size(fifo) == 0);

    assert(cfifo_put(NULL, &a) == CFIFO_ERR_NULL);
    assert(cfifo_put(NULL, NULL) == CFIFO_ERR_NULL);
    assert(cfifo_put(fifo, NULL) == CFIFO_ERR_NULL);

    assert(cfifo_write(NULL, &a, &size) == CFIFO_ERR_NULL);
    assert(cfifo_write(NULL, NULL, &size) == CFIFO_ERR_NULL);
    assert(cfifo_write(NULL, NULL, NULL) == CFIFO_ERR_NULL);
    assert(cfifo_write(NULL, &a, NULL) == CFIFO_ERR_NULL);
    assert(cfifo_write(fifo, &a, NULL) == CFIFO_ERR_NULL);
    assert(cfifo_write(NULL, &a, NULL) == CFIFO_ERR_NULL);
    assert(cfifo_write(fifo, NULL, NULL) == CFIFO_ERR_NULL);
    assert(cfifo_write(fifo, NULL, &size) == CFIFO_ERR_NULL);


    assert(cfifo_read(NULL, &a, &size) == CFIFO_ERR_NULL);
    assert(cfifo_read(NULL, NULL, &size) == CFIFO_ERR_NULL);
    assert(cfifo_read(NULL, NULL, NULL) == CFIFO_ERR_NULL);
    assert(cfifo_read(NULL, &a, NULL) == CFIFO_ERR_NULL);
    assert(cfifo_read(fifo, &a, NULL) == CFIFO_ERR_NULL);
    assert(cfifo_read(NULL, &a, NULL) == CFIFO_ERR_NULL);
    assert(cfifo_read(fifo, NULL, NULL) == CFIFO_ERR_NULL);
    assert(cfifo_read(fifo, NULL, &size) == CFIFO_ERR_NULL);


    assert(cfifo_get(NULL, &a) == CFIFO_ERR_NULL);
    assert(cfifo_get(NULL, NULL) == CFIFO_ERR_NULL);
    assert(cfifo_get(fifo, NULL) == CFIFO_ERR_NULL);

    assert(cfifo_peek(NULL, &a) == CFIFO_ERR_NULL);
    assert(cfifo_peek(NULL, NULL) == CFIFO_ERR_NULL);
    assert(cfifo_peek(fifo, NULL) == CFIFO_ERR_NULL);

    assert(cfifo_flush(NULL) == CFIFO_ERR_NULL);

    assert(cfifo_init(NULL, NULL, 0, 0, 0) == CFIFO_ERR_NULL);
    memset(fifo, 0x00, sizeof(struct cfifo_generic));
    assert(cfifo_init(fifo, rdata, 16, 1, 16) == CFIFO_SUCCESS);
    memset(fifo, 0x00, sizeof(struct cfifo_generic));
    assert(cfifo_init(fifo, rdata, 15, 1, 15) == CFIFO_ERR_BAD_SIZE);
    memset(fifo, 0x00, sizeof(struct cfifo_generic));
    assert(cfifo_init(fifo, rdata, 16, 1, 15) == CFIFO_ERR_BAD_SIZE);
    assert(cfifo_put(fifo, &a) == CFIFO_ERR_INVALID_STATE);
    assert(cfifo_write(fifo, &a, &size) == CFIFO_ERR_INVALID_STATE);
    assert(cfifo_read(fifo, &a, &size) == CFIFO_ERR_INVALID_STATE);
    assert(cfifo_get(fifo, &a) == CFIFO_ERR_INVALID_STATE);
    assert(cfifo_peek(fifo, &a) == CFIFO_ERR_INVALID_STATE);
    assert(cfifo_flush(fifo) == CFIFO_ERR_INVALID_STATE);


    assert(cfifo_size(fifo) == 0);
    assert(cfifo_available(fifo) == 0);

    printf("cfifo test passed!\r\n");

    return 0;

}
