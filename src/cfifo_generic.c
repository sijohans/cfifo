/**
 * @file cfifo_generic.c
 *
 * Description
 *
 */

/*======= Includes ==========================================================*/

/* C-Library includes */
#include <string.h> /* For memcpy */

/* Local includes */
#include "cfifo_generic.h"

/*======= Local Macro Definitions ===========================================*/

#ifndef MIN
#define MIN(a,b) ((a) < (b)) ? (a) : (b)
#endif

#define CFIFO_WRITE_POS     (p_cfifo->write_pos & p_cfifo->num_items_mask)
#define CFIFO_READ_POS      (p_cfifo->read_pos & p_cfifo->num_items_mask)
#define CFIFO_WRITE_OFFSET  (CFIFO_WRITE_POS * p_cfifo->item_size)
#define CFIFO_READ_OFFSET   (CFIFO_READ_POS * p_cfifo->item_size)
#define CFIFO_SIZE          cfifoi_size(p_cfifo)
#define CFIFO_AVAILABLE     cfifoi_available(p_cfifo)

/*======= Local function prototypes =========================================*/

static size_t cfifoi_available(cfifo_t p_cfifo);
static size_t cfifoi_size(cfifo_t p_cfifo);
static void cfifoi_put(cfifo_t p_cfifo, const void * const p_item);
static void cfifoi_get(cfifo_t p_cfifo, void *p_item);

/*======= Global function implementations ===================================*/

int cfifo_init(cfifo_t p_cfifo,
               void *p_buf,
               size_t capacity,
               size_t item_size,
               size_t buf_size)
{
    if (NULL == p_cfifo || NULL == p_buf)
    {
        /* Error, null pointers. */
        return CFIFO_ERR_NULL;
    }

    if (!CFIFO_IS_POW_2(capacity))
    {
        return CFIFO_ERR_BAD_SIZE;
    }

    if (!((item_size > 0) && (buf_size / item_size == capacity)))
    {
        return CFIFO_ERR_BAD_SIZE;
    }

    p_cfifo->p_buf = (uint8_t *) p_buf;
    p_cfifo->num_items_mask = capacity - 1;
    p_cfifo->item_size = item_size;
    p_cfifo->read_pos = 0;
    p_cfifo->write_pos = 0;

    return CFIFO_SUCCESS;
}

int cfifo_put(cfifo_t p_cfifo,
              const void * const p_item)
{
    if (NULL == p_cfifo || NULL == p_item)
    {
        /* Error, null pointers. */
        return CFIFO_ERR_NULL;
    }

    if (NULL == p_cfifo->p_buf)
    {
        return CFIFO_ERR_INVALID_STATE;
    }

    if (CFIFO_AVAILABLE > 0)
    {
        cfifoi_put(p_cfifo, p_item);
        return CFIFO_SUCCESS;
    }
    return CFIFO_ERR_FULL;
}

int cfifo_write(cfifo_t p_cfifo,
                const void * const p_items,
                size_t *p_num_items)
{
    size_t i;
    const uint8_t * const p_src = (const uint8_t * const) p_items;

    if (NULL == p_cfifo || NULL == p_items || NULL == p_num_items)
    {
        /* Error, null pointers. */
        return CFIFO_ERR_NULL;
    }

    if (NULL == p_cfifo->p_buf)
    {
        return CFIFO_ERR_INVALID_STATE;
    }

    (*p_num_items) = MIN((*p_num_items), CFIFO_AVAILABLE);

    for (i = 0; i < (*p_num_items); i++)
    {
        cfifoi_put(p_cfifo, &p_src[i * p_cfifo->item_size]);
    }

    return CFIFO_SUCCESS;

}

int cfifo_get(cfifo_t p_cfifo,
              void *p_item)
{

    if (NULL == p_cfifo || NULL == p_item)
    {
        /* Error, null pointers. */
        return CFIFO_ERR_NULL;
    }

    if (NULL == p_cfifo->p_buf)
    {
        return CFIFO_ERR_INVALID_STATE;
    }

    if (CFIFO_SIZE > 0)
    {
        cfifoi_get(p_cfifo, p_item);
        return CFIFO_SUCCESS;
    }
    return CFIFO_ERR_EMPTY;
}

int cfifo_read(cfifo_t p_cfifo,
               void *p_items,
               size_t *p_num_items)
{

    size_t i;
    uint8_t *p_dest = (uint8_t *) p_items;

    if (NULL == p_cfifo || NULL == p_items || NULL == p_num_items)
    {
        /* Error, null pointers. */
        return CFIFO_ERR_NULL;
    }

    if (NULL == p_cfifo->p_buf)
    {
        return CFIFO_ERR_INVALID_STATE;
    }

    (*p_num_items) = MIN((*p_num_items), CFIFO_SIZE);

    for (i = 0; i < (*p_num_items); i++)
    {
        cfifoi_get(p_cfifo, &p_dest[i * p_cfifo->item_size]);
    }

    return CFIFO_SUCCESS;

}

int cfifo_peek(cfifo_t p_cfifo,
               void *p_item)
{
    if (NULL == p_cfifo || NULL == p_item)
    {
        /* Error, null pointers. */
        return CFIFO_ERR_NULL;
    }

    if (NULL == p_cfifo->p_buf)
    {
        return CFIFO_ERR_INVALID_STATE;
    }

    if (CFIFO_SIZE > 0)
    {
        memcpy(p_item,
               &p_cfifo->p_buf[CFIFO_READ_OFFSET],
               p_cfifo->item_size);
        return CFIFO_SUCCESS;
    }
    return CFIFO_ERR_EMPTY;

}

size_t cfifo_contains(cfifo_t p_cfifo,
                      void *p_item)
{

    /* Store original read pos. */
    size_t i;
    size_t size;
    size_t read_pos;
    size_t items_found = 0;

    if (NULL == p_cfifo || NULL == p_item || NULL == p_cfifo->p_buf)
    {
        /* Error, null pointers. */
        return 0;
    }

    read_pos = p_cfifo->read_pos;
    size = p_cfifo->write_pos - read_pos;

    for (i = 0; i < size; i++)
    {
        if (memcmp(p_item,
                   &p_cfifo->p_buf[CFIFO_READ_OFFSET],
                   p_cfifo->item_size) == 0)
        {
            items_found++;
        }
        p_cfifo->read_pos++;
    }

    p_cfifo->read_pos = read_pos;

    return items_found;
}

size_t cfifo_size(cfifo_t p_cfifo)
{
    return (NULL != p_cfifo && p_cfifo->p_buf != NULL) ? CFIFO_SIZE : 0;
}

size_t cfifo_available(cfifo_t p_cfifo)
{
    return (NULL != p_cfifo && p_cfifo->p_buf != NULL) ? CFIFO_AVAILABLE : 0;
}

int cfifo_flush(cfifo_t p_cfifo)
{
    if (NULL == p_cfifo)
    {
        /* Error, null pointers. */
        return CFIFO_ERR_NULL;
    }

    if (NULL == p_cfifo->p_buf)
    {
        return CFIFO_ERR_INVALID_STATE;
    }

    p_cfifo->write_pos = 0;
    p_cfifo->read_pos = 0;

    return CFIFO_SUCCESS;
}

/*======= Local function implementations ====================================*/

static size_t cfifoi_available(cfifo_t p_cfifo)
{
    return p_cfifo->num_items_mask + 1 - cfifoi_size(p_cfifo);
}
static size_t cfifoi_size(cfifo_t p_cfifo)
{
    size_t tmp = p_cfifo->read_pos;
    return p_cfifo->write_pos - tmp;
}

static void cfifoi_put(cfifo_t p_cfifo, const void * const p_item)
{
    memcpy(&p_cfifo->p_buf[CFIFO_WRITE_OFFSET],
           p_item,
           p_cfifo->item_size);
    p_cfifo->write_pos++;
}

static void cfifoi_get(cfifo_t p_cfifo, void *p_item)
{
    memcpy(p_item,
           &p_cfifo->p_buf[CFIFO_READ_OFFSET],
           p_cfifo->item_size);
    p_cfifo->read_pos++;
}
