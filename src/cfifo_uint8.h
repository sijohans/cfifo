#ifndef _cfifo_uint8_H_
#define _cfifo_uint8_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file cfifo_uint8.h.h
 *
 * Description
 *
 */

#include <stdint.h>

#define CFIFO_SPECIFIC_TYPE         uint8_t
#define CFIFO_SPECIFIC_NAME         uint8
#define CFIFO_SPECIFIC_USE_MEMCMP   0
#define CFIFO_SPECIFIC_USE_MEMCPY   0

#include "cfifo_specific_template.h"

#ifdef __cplusplus
}
#endif

#endif /* _cfifo_uint8_H_ */
