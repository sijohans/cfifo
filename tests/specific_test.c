#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "cfifo_specific.h"

int main(void)
{

    CFIFO_SPECIFIC_TYPE a;
    CFIFO_SPECIFIC_TYPE b;
    size_t size;
    size_t i;

    CFIFO_SPECIFIC_TYPE rdata[16];
    CFIFO_SPECIFIC_TYPE data[16];

    CFIFO_SPECIFIC_CREATE_STATIC(fifo, 16);
    assert(cfifo_specific_available(fifo) == 16);
    assert(cfifo_specific_peek(fifo, &a) == CFIFO_ERR_EMPTY);
    a = 3;
    assert(cfifo_specific_contains(fifo, &a) == 0);

    /* Empty queue from stat */
    assert(cfifo_specific_size(fifo) == 0);
    assert(cfifo_specific_pop(fifo, &b) == CFIFO_ERR_EMPTY);

    /* One queue item */
    a = 1;
    assert(cfifo_specific_push(fifo, &a) == CFIFO_SUCCESS);
    b = 1;
    assert(cfifo_specific_contains(fifo, &b) == 1);
    assert(cfifo_specific_size(fifo) == 1);
    assert(cfifo_specific_pop(fifo, &b) == CFIFO_SUCCESS);
    assert(b == a);
    assert(cfifo_specific_size(fifo) == 0);

    /* Fill and empty queue */
    for (i = 0; i < 16; i++)
    {
        a = (CFIFO_SPECIFIC_TYPE) i;
        assert(cfifo_specific_push(fifo, &a) == CFIFO_SUCCESS);
        assert(cfifo_specific_size(fifo) == (i + 1));
    }

    assert(cfifo_specific_push(fifo, &a) == CFIFO_ERR_FULL);

    for (i = 0; i < 16; i++)
    {
        assert(cfifo_specific_pop(fifo, &a) == CFIFO_SUCCESS);
        assert(a == i);
        assert(cfifo_specific_size(fifo) == 15 - i);
    }

    assert(cfifo_specific_pop(fifo, &b) == CFIFO_ERR_EMPTY);

    for (i = 0; i < 16; i++)
    {
        data[i] = i + 1;
    }

    size = 16;

    assert(cfifo_specific_write(fifo, data, &size) == CFIFO_SUCCESS);
    assert(size == 16);
    assert(cfifo_specific_size(fifo) == 16);


    size = 16;
    assert(cfifo_specific_read(fifo, rdata, &size) == CFIFO_SUCCESS);
    for (i = 0; i < 16; i++)
    {
        assert(rdata[i] == data[i]);
    }
    assert(size == 16);
    assert(cfifo_specific_size(fifo) == 0);

    size = 13;

    assert(cfifo_specific_write(fifo, data, &size) == CFIFO_SUCCESS);
    assert(size == 13);
    assert(cfifo_specific_size(fifo) == 13);

    assert(cfifo_specific_pop(fifo, &a) == CFIFO_SUCCESS);
    assert(a == data[0]);
    assert(cfifo_specific_pop(fifo, &a) == CFIFO_SUCCESS);
    assert(a == data[1]);
    assert(cfifo_specific_size(fifo) == 11);

    size = 7;
    assert(cfifo_specific_read(fifo, rdata, &size) == CFIFO_SUCCESS);
    assert(size == 7);
    for (i = 0; i < 7; i++)
    {
        assert(rdata[i] == data[2 + i]);
    }
    assert(cfifo_specific_size(fifo) == 4);

    /* Simulate fifo counter overflow */
    fifo->write_pos = UINT32_MAX;
    fifo->read_pos = UINT32_MAX;
    assert(cfifo_specific_size(fifo) == 0);

    /* One queue item */
    a = 67;
    assert(cfifo_specific_push(fifo, &a) == CFIFO_SUCCESS);
    assert(cfifo_specific_size(fifo) == 1);
    assert(cfifo_specific_peek(fifo, &b) == CFIFO_SUCCESS);
    assert(b == a);
    assert(cfifo_specific_size(fifo) == 1);
    assert(cfifo_specific_pop(fifo, &b) == CFIFO_SUCCESS);
    assert(b == a);
    assert(cfifo_specific_size(fifo) == 0);

    for (i = 0; i < 16; i++)
    {
        data[i] = i + 1;
    }

    size = 17;

    assert(cfifo_specific_write(fifo, data, &size) == CFIFO_SUCCESS);
    assert(size == 16);
    assert(cfifo_specific_size(fifo) == 16);
    assert(cfifo_specific_flush(fifo) == CFIFO_SUCCESS);
    assert(cfifo_specific_size(fifo) == 0);

    /* Simulate fifo counter overflow */
    fifo->write_pos = 0 - 5;
    fifo->read_pos = 0 - 5;
    for (i = 0; i < 16; i++)
    {
        a = (CFIFO_SPECIFIC_TYPE) i;
        assert(cfifo_specific_push(fifo, &a) == CFIFO_SUCCESS);
        assert(cfifo_specific_size(fifo) == (i + 1));
    }

    for (i = 0; i < 16; i++)
    {
        assert(cfifo_specific_size(fifo) == 16 - i);
        assert(cfifo_specific_pop(fifo, &a) == CFIFO_SUCCESS);
    }

    assert(cfifo_specific_size(fifo) == 0);

    assert(cfifo_specific_push(NULL, &a) == CFIFO_ERR_NULL);
    assert(cfifo_specific_push(NULL, NULL) == CFIFO_ERR_NULL);
    assert(cfifo_specific_push(fifo, NULL) == CFIFO_ERR_NULL);

    assert(cfifo_specific_write(NULL, &a, &size) == CFIFO_ERR_NULL);
    assert(cfifo_specific_write(NULL, NULL, &size) == CFIFO_ERR_NULL);
    assert(cfifo_specific_write(NULL, NULL, NULL) == CFIFO_ERR_NULL);
    assert(cfifo_specific_write(NULL, &a, NULL) == CFIFO_ERR_NULL);
    assert(cfifo_specific_write(fifo, &a, NULL) == CFIFO_ERR_NULL);
    assert(cfifo_specific_write(NULL, &a, NULL) == CFIFO_ERR_NULL);
    assert(cfifo_specific_write(fifo, NULL, NULL) == CFIFO_ERR_NULL);
    assert(cfifo_specific_write(fifo, NULL, &size) == CFIFO_ERR_NULL);


    assert(cfifo_specific_read(NULL, &a, &size) == CFIFO_ERR_NULL);
    assert(cfifo_specific_read(NULL, NULL, &size) == CFIFO_ERR_NULL);
    assert(cfifo_specific_read(NULL, NULL, NULL) == CFIFO_ERR_NULL);
    assert(cfifo_specific_read(NULL, &a, NULL) == CFIFO_ERR_NULL);
    assert(cfifo_specific_read(fifo, &a, NULL) == CFIFO_ERR_NULL);
    assert(cfifo_specific_read(NULL, &a, NULL) == CFIFO_ERR_NULL);
    assert(cfifo_specific_read(fifo, NULL, NULL) == CFIFO_ERR_NULL);
    assert(cfifo_specific_read(fifo, NULL, &size) == CFIFO_ERR_NULL);


    assert(cfifo_specific_pop(NULL, &a) == CFIFO_ERR_NULL);
    assert(cfifo_specific_pop(NULL, NULL) == CFIFO_ERR_NULL);
    assert(cfifo_specific_pop(fifo, NULL) == CFIFO_ERR_NULL);

    assert(cfifo_specific_peek(NULL, &a) == CFIFO_ERR_NULL);
    assert(cfifo_specific_peek(NULL, NULL) == CFIFO_ERR_NULL);
    assert(cfifo_specific_peek(fifo, NULL) == CFIFO_ERR_NULL);

    assert(cfifo_specific_flush(NULL) == CFIFO_ERR_NULL);

    assert(cfifo_specific_init(NULL, NULL, 0) == CFIFO_ERR_NULL);
    memset(fifo, 0x00, sizeof(struct cfifo_specific));
    assert(cfifo_specific_init(fifo, rdata, 16) == CFIFO_SUCCESS);
    memset(fifo, 0x00, sizeof(struct cfifo_specific));
    assert(cfifo_specific_init(fifo, rdata, 15) == CFIFO_ERR_BAD_SIZE);
    memset(fifo, 0x00, sizeof(struct cfifo_specific));
    assert(!cfifo_specific_contains(fifo, &a));
    assert(cfifo_specific_init(fifo, rdata, 15) == CFIFO_ERR_BAD_SIZE);
    assert(cfifo_specific_push(fifo, &a) == CFIFO_ERR_INVALID_STATE);
    assert(cfifo_specific_write(fifo, &a, &size) == CFIFO_ERR_INVALID_STATE);
    assert(cfifo_specific_read(fifo, &a, &size) == CFIFO_ERR_INVALID_STATE);
    assert(cfifo_specific_pop(fifo, &a) == CFIFO_ERR_INVALID_STATE);
    assert(cfifo_specific_peek(fifo, &a) == CFIFO_ERR_INVALID_STATE);
    assert(cfifo_specific_flush(fifo) == CFIFO_ERR_INVALID_STATE);


    assert(cfifo_specific_size(fifo) == 0);
    assert(cfifo_specific_available(fifo) == 0);

    printf("cfifo test passed!\r\n");

    return 0;

}