/**
 *  @file
 *  @copyright defined in eos/LICENSE
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <wchar.h>

/**
 *  @defgroup c_types
 *  @ingroup c_api
 *  @brief Задаёт встроенные типы, typedef и псевдонимы
 */

/**
 *  @addtogroup c_types
 *  @brief Задаёт встроенные типы, typedef и псевдонимы
 *  @{
 */

/**
 * Macro to align/overalign a type to ensure calls to intrinsics with pointers/references are properly aligned
 */

/* macro to align/overalign a type to ensure calls to intrinsics with pointers/references are properly aligned */
#define ALIGNED(X) __attribute__ ((aligned (16))) X

typedef uint64_t capi_name;

/**
 * Публичный ключ COOPOS. Ключи K1 и R1 — 34 байта. Более новые типы ключей могут быть переменной длины
 */
struct __attribute__((deprecated("newer public key types cannot be represented as a fixed size structure", "char[]")))
capi_public_key {
   char data[34];
};

/**
 * Подпись COOPOS. Подписи K1 и R1 — 66 байт. Более новые типы подписей могут быть переменной длины
 */
struct __attribute__((deprecated("newer signature types cannot be represented as a fixed size structure", "char[]")))
capi_signature {
   uint8_t data[66];
};

/**
 * 256-bit hash
 */
struct ALIGNED(capi_checksum256) {
   uint8_t hash[32];
};

/**
 * 160-bit hash
 */
struct ALIGNED(capi_checksum160) {
   uint8_t hash[20];
};

/**
 * 512-bit hash
 */
struct ALIGNED(capi_checksum512) {
   uint8_t hash[64];
};

/// @}
