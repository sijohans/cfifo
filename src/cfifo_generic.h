#ifndef _CFIFO_H_
#define _CFIFO_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file cfifo_generic.h
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
        ((CFIFO_IS_POW_2(x) && (((x)*(y)) <= SIZE_MAX)) ? (int) ((x)*(y)) : (int) -1)

/**
 * @brief Macro to define a cfifo structure.
 *
 * Designated initializers, i.e.:
 * {
 *     .pbuf = ...,
 *     .num_items = ...,
 * }
 *
 * Is not used to support c89 and C++.
 *
 */
#define CFIFO_GENERIC_STRUCT_DEF(type, capacity, buf)                   \
    {                                                                   \
        buf,                                                            \
        ((capacity) - 1),                                               \
        sizeof(type),                                                   \
        0,                                                              \
        0                                                               \
    }

/**
 * @brief Creates a cfifo structure and the buffer.
 *
 * Used to create a cfifo. The capacity (number of elements) MUST be
 * power or two. I.e., 2, 4, 8, ... 2^n. If the capacity is not power
 * of two the usade of this macro will result in a compile error.
 *
 * CFIFO_GENERIC_CREATE_STATIC(my_fifo, uint32_t, 32);
 *
 * uint32_t a = 4;
 * cfifo_generic_put(my_fifo, &a);
 *
 */
#define CFIFO_GENERIC_CREATE(p_cfifo, type, capacity)                   \
    uint8_t                                                             \
            p_cfifo##cfifo_buf##buf##__LINE__                           \
            [CFIFO_BUF_SIZE((sizeof(type)),(capacity))] = {0};          \
    struct cfifo_generic                                                \
        p_cfifo##data##__LINE__ = CFIFO_GENERIC_STRUCT_DEF(             \
        type, capacity, p_cfifo##cfifo_buf##buf##__LINE__               \
    );                                                                  \
    cfifo_generic_t p_cfifo = &p_cfifo##data##__LINE__

/**
 * @brief Statically creates a cfifo structure and the buffer.
 *
 * Used to create a cfifo. The capacity (number of elements) MUST be
 * power or two. I.e., 2, 4, 8, ... 2^n. If the capacity is not power
 * of two the usade of this macro will result in a compile error.
 *
 * CFIFO_GENERIC_CREATE_STATIC(my_fifo, uint32_t, 32);
 *
 * uint32_t a = 4;
 * cfifo_generic_put(my_fifo, &a);
 *
 */
#define CFIFO_GENERIC_CREATE_STATIC(p_cfifo, type, capacity)            \
    static uint8_t                                                      \
            p_cfifo##cfifo_buf##buf##__LINE__                           \
            [CFIFO_BUF_SIZE((sizeof(type)),(capacity))];                \
    static struct cfifo_generic                                         \
            p_cfifo##data##__LINE__ = CFIFO_GENERIC_STRUCT_DEF(         \
        type, capacity, p_cfifo##cfifo_buf##buf##__LINE__               \
    );                                                                  \
    static cfifo_generic_t p_cfifo = &p_cfifo##data##__LINE__

/* Return codes */
#ifndef CFIFO_RETURN_CODES
#define CFIFO_RETURN_CODES
#define CFIFO_SUCCESS               (0)
#define CFIFO_ERR_NULL              (-1)
#define CFIFO_ERR_EMPTY             (-2)
#define CFIFO_ERR_FULL              (-3)
#define CFIFO_ERR_BAD_SIZE          (-4)
#define CFIFO_ERR_INVALID_STATE     (-5)
#endif

/*======= Type Definitions and declarations =================================*/

typedef struct cfifo_generic *cfifo_generic_t;

struct cfifo_generic {
    uint8_t         *p_buf;
    size_t          num_items_mask;
    size_t          item_size;
    volatile size_t read_pos;
    volatile size_t write_pos;
};

/*======= Public function declarations ======================================*/

/**
 * @brief Initiates a cfifo.
 *
 * The buffer MUST be power of 2. Also, the buffer size
 * must be equal to num_iteam * item_size.
 *
 */
int cfifo_generic_init(cfifo_generic_t p_cfifo,
               void *p_buf,
               size_t capacity,
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
int cfifo_generic_put(cfifo_generic_t p_cfifo,
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
int cfifo_generic_write(cfifo_generic_t p_cfifo,
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
int cfifo_generic_get(cfifo_generic_t p_cfifo,
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
int cfifo_generic_read(cfifo_generic_t p_cfifo,
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
int cfifo_generic_peek(cfifo_generic_t p_cfifo,
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
size_t cfifo_generic_contains(cfifo_generic_t p_cfifo,
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
size_t cfifo_generic_size(cfifo_generic_t p_cfifo);

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
size_t cfifo_generic_available(cfifo_generic_t p_cfifo);


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
int cfifo_generic_flush(cfifo_generic_t p_cfifo);

#ifdef __cplusplus
}
#endif

#endif /* _CFIFO_H_ */
