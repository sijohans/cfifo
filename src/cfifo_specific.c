/**
 * @file cfifo_specific.c
 *
 * Description
 *
 */

/*======= Includes ==========================================================*/

/* Local includes */
#include "cfifo_specific.h"

/* C-Library includes */
#ifdef CFIFO_SPECIFIC_USE_MEMCPY
#include <string.h> /* For memcpy */
#endif

/*======= Local Macro Definitions ===========================================*/

#ifndef MIN
#define MIN(a,b) ((a) < (b)) ? (a) : (b)
#endif

#define CFIFO_SPECIFIC_WRITE_POS    (p_cfifo->write_pos & p_cfifo->num_items_mask)
#define CFIFO_SPECIFIC_READ_POS     (p_cfifo->read_pos & p_cfifo->num_items_mask)

/*======= Local function prototypes =========================================*/

static size_t cfifoi_available(cfifo_specific_t p_cfifo);
static size_t cfifoi_size(cfifo_specific_t p_cfifo);
static void cfifoi_push(cfifo_specific_t p_cfifo,
                        const CFIFO_SPECIFIC_TYPE * const p_item);
static void cfifoi_pop(cfifo_specific_t p_cfifo, CFIFO_SPECIFIC_TYPE *p_item);

/*======= Global function implementations ===================================*/

int cfifo_specific_init(cfifo_specific_t p_cfifo,
                        CFIFO_SPECIFIC_TYPE *p_buf,
                        size_t capacity)
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

    p_cfifo->p_buf = p_buf;
    p_cfifo->num_items_mask = capacity - 1;
    p_cfifo->read_pos = 0;
    p_cfifo->write_pos = 0;

    return CFIFO_SUCCESS;
}

int cfifo_specific_push(cfifo_specific_t p_cfifo,
                        const CFIFO_SPECIFIC_TYPE * const p_item)
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

    if (cfifoi_available(p_cfifo) > 0)
    {
        cfifoi_push(p_cfifo, p_item);
        return CFIFO_SUCCESS;
    }
    return CFIFO_ERR_FULL;
}

int cfifo_specific_write(cfifo_specific_t p_cfifo,
                         const CFIFO_SPECIFIC_TYPE * const p_items,
                         size_t *p_num_items)
{
    size_t i;

    if (NULL == p_cfifo || NULL == p_items || NULL == p_num_items)
    {
        /* Error, null pointers. */
        return CFIFO_ERR_NULL;
    }

    if (NULL == p_cfifo->p_buf)
    {
        return CFIFO_ERR_INVALID_STATE;
    }

    (*p_num_items) = MIN((*p_num_items), cfifoi_available(p_cfifo));

    for (i = 0; i < (*p_num_items); i++)
    {
        cfifoi_push(p_cfifo, &p_items[i]);
    }

    return CFIFO_SUCCESS;

}

int cfifo_specific_pop(cfifo_specific_t p_cfifo,
                       CFIFO_SPECIFIC_TYPE *p_item)
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

    if (cfifoi_size(p_cfifo) > 0)
    {
        cfifoi_pop(p_cfifo, p_item);
        return CFIFO_SUCCESS;
    }
    return CFIFO_ERR_EMPTY;
}

int cfifo_specific_read(cfifo_specific_t p_cfifo,
                        CFIFO_SPECIFIC_TYPE *p_items,
                        size_t *p_num_items)
{

    size_t i;

    if (NULL == p_cfifo || NULL == p_items || NULL == p_num_items)
    {
        /* Error, null pointers. */
        return CFIFO_ERR_NULL;
    }

    if (NULL == p_cfifo->p_buf)
    {
        return CFIFO_ERR_INVALID_STATE;
    }

    (*p_num_items) = MIN((*p_num_items), cfifoi_size(p_cfifo));

    for (i = 0; i < (*p_num_items); i++)
    {
        cfifoi_pop(p_cfifo, &p_items[i]);
    }

    return CFIFO_SUCCESS;

}

int cfifo_specific_peek(cfifo_specific_t p_cfifo,
                        CFIFO_SPECIFIC_TYPE *p_item)
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

    if (cfifoi_size(p_cfifo) > 0)
    {
#if CFIFO_SPECIFIC_USE_MEMCPY == 1
        memcpy(p_item,
               &p_cfifo->p_buf[CFIFO_SPECIFIC_READ_POS],
               sizeof(CFIFO_SPECIFIC_TYPE));
#else
        *p_item = p_cfifo->p_buf[CFIFO_SPECIFIC_READ_POS];
#endif
        return CFIFO_SUCCESS;
    }
    return CFIFO_ERR_EMPTY;

}

size_t cfifo_specific_contains(cfifo_specific_t p_cfifo,
                               CFIFO_SPECIFIC_TYPE *p_item)
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
#if CFIFO_SPECIFIC_USE_MEMCMP == 1
        if (memcmp(p_item,
                   &p_cfifo->p_buf[CFIFO_SPECIFIC_READ_POS],
                   sizeof(CFIFO_SPECIFIC_TYPE)) == 0)
        {
#else
        if ((*p_item) == p_cfifo->p_buf[CFIFO_SPECIFIC_READ_POS])
        {
#endif
            items_found++;
        }
        p_cfifo->read_pos++;
    }

    p_cfifo->read_pos = read_pos;

    return items_found;
}

size_t cfifo_specific_size(cfifo_specific_t p_cfifo)
{
    return (NULL != p_cfifo && p_cfifo->p_buf != NULL) ? cfifoi_size(p_cfifo) : 0;
}

size_t cfifo_specific_available(cfifo_specific_t p_cfifo)
{
    return (NULL != p_cfifo && p_cfifo->p_buf != NULL) ? cfifoi_available(p_cfifo) : 0;
}

int cfifo_specific_flush(cfifo_specific_t p_cfifo)
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

static size_t cfifoi_available(cfifo_specific_t p_cfifo)
{
    return p_cfifo->num_items_mask + 1 - cfifoi_size(p_cfifo);
}
static size_t cfifoi_size(cfifo_specific_t p_cfifo)
{
    size_t tmp = p_cfifo->read_pos;
    return p_cfifo->write_pos - tmp;
}

static void cfifoi_push(cfifo_specific_t p_cfifo,
                        const CFIFO_SPECIFIC_TYPE * const p_item)
{
    CFIFO_SPECIFIC_TYPE *p_dest = &p_cfifo->p_buf[CFIFO_SPECIFIC_WRITE_POS];
#if CFIFO_SPECIFIC_USE_MEMCPY == 1
    memcpy(p_dest,
           p_item,
           sizeof(CFIFO_SPECIFIC_TYPE));
#else
    *p_dest = *p_item;
#endif
    p_cfifo->write_pos++;
}

static void cfifoi_pop(cfifo_specific_t p_cfifo, CFIFO_SPECIFIC_TYPE *p_item)
{
#if CFIFO_SPECIFIC_USE_MEMCPY == 1
    memcpy(p_item,
           &p_cfifo->p_buf[CFIFO_SPECIFIC_READ_POS],
           sizeof(CFIFO_SPECIFIC_TYPE));
#else
    (*p_item) = p_cfifo->p_buf[CFIFO_SPECIFIC_READ_POS];
#endif
    p_cfifo->read_pos++;
}
