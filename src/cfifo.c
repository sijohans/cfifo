/**
 * @file cfifo.c
 *
 * Description
 *
 */

/*======= Includes ==========================================================*/

/* Local includes */
#include "cfifo.h"

/* C-Library includes */
#include <string.h> /* For memcpy */

/*======= Local Macro Definitions ===========================================*/

#ifndef MIN
#define MIN(a,b) ((a) < (b)) ? (a) : (b)
#endif

#define CFIFO_WRITE_POS (p_cfifo->write_pos % p_cfifo->buf_size)
#define CFIFO_READ_POS  (p_cfifo->read_pos % p_cfifo->buf_size)
#define CFIFO_WRITE_OFFSET  (CFIFO_WRITE_POS * p_cfifo->item_size)
#define CFIFO_READ_OFFSET   (CFIFO_READ_POS * p_cfifo->item_size)

/*======= Local function prototypes =========================================*/

static void cfifoi_put(cfifo_t *p_cfifo, const void * const p_item);
static void cfifoi_get(cfifo_t *p_cfifo, void *p_item);

/*======= Global function implementations ===================================*/

cfifo_ret_t cfifo_init(cfifo_t *p_cfifo,
                       uint8_t *p_buf,
                       uint32_t buf_size,
                       uint32_t item_size)
{
    if (p_cfifo == NULL || p_buf == NULL) {
        /* Error, null pointers. */
        return CFIFO_ERR_NULL;
    }

    p_cfifo->p_buf = p_buf;
    p_cfifo->buf_size = buf_size;
    p_cfifo->item_size = item_size;
    p_cfifo->read_pos = 0;
    p_cfifo->write_pos = 0;

    return CFIFO_SUCCESS;
}

cfifo_ret_t cfifo_put(cfifo_t *p_cfifo,
                      const void * const p_item)
{
    uint32_t size;

    if (p_cfifo == 0 || p_item == 0) {
        /* Error, null pointers. */
        return CFIFO_ERR_NULL;
    }

    size = p_cfifo->write_pos;
    size -= p_cfifo->read_pos;

    if (size < p_cfifo->buf_size) {
        cfifoi_put(p_cfifo, p_item);
        return CFIFO_SUCCESS;
    }
    return CFIFO_ERR_FULL;
}

cfifo_ret_t cfifo_write(cfifo_t *p_cfifo,
                        const void * const p_items,
                        uint32_t *p_num_items)
{
    uint32_t i;
    uint8_t *p_src = (uint8_t *) p_items;

    if (p_cfifo == 0 || p_items == 0 || p_num_items == 0) {
        /* Error, null pointers. */
        return CFIFO_ERR_NULL;
    }

    i = p_cfifo->write_pos;
    i -= p_cfifo->read_pos;

    (*p_num_items) = MIN((*p_num_items), p_cfifo->buf_size - i);

    for (i = 0; i < (*p_num_items); i++)
    {
        cfifoi_put(p_cfifo, &p_src[i * p_cfifo->item_size]);
    }

    return CFIFO_SUCCESS;

}

cfifo_ret_t cfifo_get(cfifo_t *p_cfifo,
                      void *p_item)
{
    uint32_t size;

    if (p_cfifo == NULL || p_item == NULL) {
        /* Error, null pointers. */
        return CFIFO_ERR_NULL;
    }

    size = p_cfifo->write_pos;
    size -= p_cfifo->read_pos;

    if (size > 0)
    {
        cfifoi_get(p_cfifo, p_item);
        return CFIFO_SUCCESS;
    }
    return CFIFO_ERR_EMPTY;
}

cfifo_ret_t cfifo_read(cfifo_t *p_cfifo,
                       void *p_items,
                       uint32_t *p_num_items)
{

    uint32_t i;
    uint8_t *p_dest = (uint8_t *) p_items;

    if (p_cfifo == NULL || p_items == NULL || p_num_items == NULL) {
        /* Error, null pointers. */
        return CFIFO_ERR_NULL;
    }

    i = p_cfifo->write_pos;
    i -= p_cfifo->read_pos;

    (*p_num_items) = MIN((*p_num_items), i);

    for (i = 0; i < (*p_num_items); i++) {
        cfifoi_get(p_cfifo, &p_dest[i * p_cfifo->item_size]);
    }

    return CFIFO_SUCCESS;

}

cfifo_ret_t cfifo_peek(cfifo_t *p_cfifo,
                       void *p_item)
{
    if (p_cfifo == NULL || p_item == NULL) {
        /* Error, null pointers. */
        return CFIFO_ERR_NULL;
    }

    memcpy(p_item,
           &p_cfifo->p_buf[CFIFO_READ_OFFSET],
           p_cfifo->item_size);

    return CFIFO_SUCCESS;

}

uint32_t cfifo_contains(cfifo_t *p_cfifo,
                        void *p_item)
{

    /* Store original read pos. */
    uint32_t i;
    uint32_t size;
    uint32_t read_pos;
    uint32_t items_found = 0;

    if (p_cfifo == NULL || p_item == NULL) {
        /* Error, null pointers. */
        return 0;
    }

    size = p_cfifo->write_pos;
    size -= p_cfifo->read_pos;
    read_pos = p_cfifo->read_pos;

    for (i = 0; i < size; i++) {
        if (memcmp(p_item,
                   &p_cfifo->p_buf[CFIFO_READ_OFFSET],
                   p_cfifo->item_size) == 0) {
            items_found++;
        }
        p_cfifo->read_pos++;
    }

    p_cfifo->read_pos = read_pos;

    return items_found;
}

uint32_t cfifo_size(cfifo_t *p_cfifo)
{
    return (p_cfifo) ? (p_cfifo->write_pos - p_cfifo->read_pos) : 0;
}

cfifo_ret_t cfifo_flush(cfifo_t *p_cfifo)
{
    if (p_cfifo == NULL) {
        /* Error, null pointers. */
        return CFIFO_ERR_NULL;
    }

    p_cfifo->write_pos = 0;
    p_cfifo->read_pos = 0;

    return CFIFO_SUCCESS;
}

/*======= Local function implementations ====================================*/

static void cfifoi_put(cfifo_t *p_cfifo, const void * const p_item)
{
    memcpy(&p_cfifo->p_buf[CFIFO_WRITE_OFFSET],
           p_item,
           p_cfifo->item_size);
    p_cfifo->write_pos++;
}

static void cfifoi_get(cfifo_t *p_cfifo, void *p_item)
{
    memcpy(p_item,
           &p_cfifo->p_buf[CFIFO_READ_OFFSET],
           p_cfifo->item_size);
    p_cfifo->read_pos++;
}
