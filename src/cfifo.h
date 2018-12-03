#ifndef _CFIFO_H_
#define _CFIFO_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file cfifo.h
 *
 * Description
 *
 */

/*======= Includes ==========================================================*/

/* C-Library includes */
#include <stddef.h> /* for size_t */
#include <stdint.h> /* for uint8_t */

/*======= Public macro definitions ==========================================*/


/*
 * Helper macros that results in compile error if the capacity (number of items)
 * is not a power of 2. I.e., 2, 4, 8, 16, ... 2^n.
 */
#define CFIFO_IS_POW_2(x)   (((x) > 0) && (((x) & (((x) - 1))) == 0))
#define CFIFO_BUF_SIZE(y, x) \
        ((CFIFO_IS_POW_2(x) && (((x)*(y)) <= SIZE_MAX)) ? ((x)*(y)) : -1)

#define _CFIFO_CREATE(p_cfifo, type, capacity, scope)                   \
    scope uint8_t                                                       \
            cfifo_buf##buffer##__LINE__                                 \
            [CFIFO_BUF_SIZE((sizeof(type)),(capacity))] = {0};          \
    scope struct cfifo_s p_cfifo##data##__LINE__ = {                    \
        .p_buf = cfifo_buf##buffer##__LINE__,                           \
        .num_items_mask = ((capacity) - 1),                             \
        .item_size = sizeof(type),                                      \
        .read_pos = 0,                                                  \
        .write_pos = 0                                                  \
    }; \
    scope cfifo_t p_cfifo = &p_cfifo##data##__LINE__;

#define CFIFO_CREATE(p_cfifo, type, capacity) \
    _CFIFO_CREATE(p_cfifo, type, capacity,)
#define CFIFO_CREATE_STATIC(p_cfifo, type, capacity) \
    _CFIFO_CREATE(p_cfifo, type, capacity, static)

#define CFIFO_DEF(type, capacity) \
    &(struct cfifo_s) { \
        .p_buf = (uint8_t[CFIFO_BUF_SIZE(sizeof(type),capacity)]) { 0 }, \
        .num_items_mask = ((capacity) - 1), \
        .item_size = sizeof(type), \
        .read_pos = 0, \
        .write_pos = 0 \
    }

/*======= Type Definitions and declarations =================================*/

typedef struct cfifo_s *cfifo_t;

struct cfifo_s {
    uint8_t * const p_buf;
    size_t          num_items_mask;
    size_t          item_size;
    volatile size_t read_pos;
    volatile size_t write_pos;
};

typedef enum cfifo_ret_e {
    CFIFO_SUCCESS,
    CFIFO_ERR_NULL,
    CFIFO_ERR_EMPTY,
    CFIFO_ERR_FULL,
    CFIFO_ERR_BAD_SIZE,
    CFIFO_ERR_INVALID_STATE
} cfifo_ret_t;

/*======= Public function declarations ======================================*/

/**
 * @brief TODO: Brief description.
 *
 * TODO: Write description.
 *
 * @param   p_cfifo
 * @param   p_buf
 * @param   buf_size
 * @param   item_size
 *
 * @return  CFIFO_SUCCESS
 *
 */
cfifo_ret_t cfifo_init(cfifo_t p_cfifo,
                       uint8_t *p_buf,
                       size_t num_items,
                       size_t item_size,
                       size_t buf_size);

/**
 * @brief TODO: Brief description.
 *
 * TODO: Write description.
 *
 * @param   p_cfifo
 * @param   p_item
 *
 * @return  CFIFO_SUCCESS
 *
 */
cfifo_ret_t cfifo_put(cfifo_t p_cfifo,
                      const void * const p_item);

/**
 * @brief TODO: Brief description.
 *
 * TODO: Write description.
 *
 * @param   p_cfifo
 * @param   p_items
 * @param   p_num_items
 *
 * @return  CFIFO_SUCCESS
 *
 */
cfifo_ret_t cfifo_write(cfifo_t p_cfifo,
                        const void * const p_items,
                        size_t * const p_num_items);

/**
 * @brief TODO: Brief description.
 *
 * TODO: Write description.
 *
 * @param   p_cfifo
 * @param   p_item
 *
 * @return  CFIFO_SUCCESS
 *
 */
cfifo_ret_t cfifo_get(cfifo_t p_cfifo,
                      void *p_item);

/**
 * @brief TODO: Brief description.
 *
 * TODO: Write description.
 *
 * @param   p_cfifo
 * @param   p_items
 * @param   p_num_items
 *
 * @return  CFIFO_SUCCESS
 *
 */
cfifo_ret_t cfifo_read(cfifo_t p_cfifo,
                       void *p_items,
                       size_t *p_num_items);

/**
 * @brief TODO: Brief description.
 *
 * TODO: Write description.
 *
 * @param   p_cfifo
 * @param   p_item
 *
 * @return  CFIFO_SUCCESS
 *
 */
cfifo_ret_t cfifo_peek(cfifo_t p_cfifo,
                       void *p_item);

/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param p_cfifo [description]
 * @param p_item [description]
 * 
 * @return
 */
size_t cfifo_contains(cfifo_t p_cfifo,
                        void *p_item);

/**
 * @brief TODO: Brief description.
 *
 * TODO: Write description.
 *
 * @param   p_cfifo
 * @param   p_num_items
 *
 * @return  CFIFO_SUCCESS
 *
 */
size_t cfifo_size(cfifo_t p_cfifo);

/**
 * @brief TODO: Brief description.
 *
 * TODO: Write description.
 *
 * @param   p_cfifo
 * @param   p_num_items
 *
 * @return  CFIFO_SUCCESS
 *
 */
size_t cfifo_available(cfifo_t p_cfifo);


/**
 * @brief TODO: Brief description.
 *
 * TODO: Write description.
 *
 * @param   p_cfifo
 *
 * @return  CFIFO_SUCCESS
 *
 */
cfifo_ret_t cfifo_flush(cfifo_t p_cfifo);

#ifdef __cplusplus
}
#endif

#endif /* _CFIFO_H_ */
