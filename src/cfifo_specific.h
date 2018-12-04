#ifndef _CFIFO_H_
#define _CFIFO_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file cfifo_specific.h
 *
 * Description
 *
 */

/*======= Includes ==========================================================*/

/* C-Library includes */
#include <stddef.h> /* for size_t */

/*======= Public macro definitions ==========================================*/

#ifndef CFIFO_SPECIFIC_TYPE
#include <stdint.h> /* for uint8_t */
#define CFIFO_SPECIFIC_TYPE uint8_t
#endif
#ifdef CFIFO_SPECIFIC_USE_MEMCPY
#define CFIFO_INTERNAL_SPECIFIC_USE_MEMCPY
#endif

/*
 * Helper macros that results in compile error if the capacity (number of items)
 * is not a power of 2. I.e., 2, 4, 8, 16, ... 2^n.
 */
#define CFIFO_IS_POW_2(x)   (((x) > 0) && (((x) & (((x) - 1))) == 0))
#define CFIFO_SPECIFIC_BUF_SIZE(x) CFIFO_IS_POW_2(x) ? (int) x : (int) -1

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
#define CFIFO_SPECIFIC_STRUCT_DEF(capacity, buf)                        \
    {                                                                   \
        buf,                                                            \
        ((capacity) - 1),                                               \
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
 * CFIFO_SPECIFIC_CREATE_STATIC(my_fifo, uint32_t, 32);
 *
 * uint32_t a = 4;
 * cfifo_specific_push(my_fifo, &a);
 *
 */
#define CFIFO_SPECIFIC_CREATE(p_cfifo, capacity)                        \
    CFIFO_SPECIFIC_TYPE                                                 \
            p_cfifo##cfifo_buf##buf##__LINE__                           \
            [CFIFO_SPECIFIC_BUF_SIZE(capacity)] = {0};                  \
    struct cfifo_specific                                               \
        p_cfifo##data##__LINE__ = CFIFO_SPECIFIC_STRUCT_DEF(            \
        capacity, p_cfifo##cfifo_buf##buf##__LINE__                     \
    );                                                                  \
    cfifo_specific_t p_cfifo = &p_cfifo##data##__LINE__

/**
 * @brief Statically creates a cfifo structure and the buffer.
 *
 * Used to create a cfifo. The capacity (number of elements) MUST be
 * power or two. I.e., 2, 4, 8, ... 2^n. If the capacity is not power
 * of two the usade of this macro will result in a compile error.
 *
 * CFIFO_SPECIFIC_CREATE_STATIC(my_fifo, uint32_t, 32);
 *
 * uint32_t a = 4;
 * cfifo_specific_push(my_fifo, &a);
 *
 */
#define CFIFO_SPECIFIC_CREATE_STATIC(p_cfifo, capacity)                 \
    static CFIFO_SPECIFIC_TYPE                                          \
            p_cfifo##cfifo_buf##buf##__LINE__                           \
            [CFIFO_SPECIFIC_BUF_SIZE(capacity)];                        \
    static struct cfifo_specific                                        \
            p_cfifo##data##__LINE__ = CFIFO_SPECIFIC_STRUCT_DEF(        \
        capacity, p_cfifo##cfifo_buf##buf##__LINE__                     \
    );                                                                  \
    static cfifo_specific_t p_cfifo = &p_cfifo##data##__LINE__

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

typedef struct cfifo_specific *cfifo_specific_t;

struct cfifo_specific {
  CFIFO_SPECIFIC_TYPE   *p_buf;
  size_t                num_items_mask;
  volatile size_t       read_pos;
  volatile size_t       write_pos;
};

/*======= Public function declarations ======================================*/

/**
 * @brief Initiates a cfifo.
 *
 * The buffer MUST be power of 2. Also, the buffer size
 * must be equal to num_iteam * item_size.
 *
 */
int cfifo_specific_init(cfifo_specific_t p_cfifo,
                        CFIFO_SPECIFIC_TYPE *p_buf,
                        size_t capacity);

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
int cfifo_specific_push(cfifo_specific_t p_cfifo,
                        const CFIFO_SPECIFIC_TYPE * const p_item);

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
int cfifo_specific_write(cfifo_specific_t p_cfifo,
                         const CFIFO_SPECIFIC_TYPE * const p_items,
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
int cfifo_specific_pop(cfifo_specific_t p_cfifo,
                       CFIFO_SPECIFIC_TYPE *p_item);

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
int cfifo_specific_read(cfifo_specific_t p_cfifo,
                        CFIFO_SPECIFIC_TYPE *p_items,
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
int cfifo_specific_peek(cfifo_specific_t p_cfifo,
                        CFIFO_SPECIFIC_TYPE *p_item);

/**
 * @brief [brief description]
 * @details [long description]
 *
 * @param p_cfifo [description]
 * @param p_item [description]
 *
 * @return
 */
size_t cfifo_specific_contains(cfifo_specific_t p_cfifo,
                               CFIFO_SPECIFIC_TYPE *p_item);

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
size_t cfifo_specific_size(cfifo_specific_t p_cfifo);

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
size_t cfifo_specific_available(cfifo_specific_t p_cfifo);


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
int cfifo_specific_flush(cfifo_specific_t p_cfifo);

#ifdef __cplusplus
}
#endif

#endif /* _CFIFO_H_ */
