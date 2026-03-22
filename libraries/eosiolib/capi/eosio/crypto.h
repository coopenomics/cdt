/**
 *  @file
 *  @copyright defined in eos/LICENSE
 */
#pragma once
#include "types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @addtogroup crypto Crypto
 *  @brief Определяет C API для вычисления и проверки хешей
 *  @{
 */

/**
 *  Tests if the sha256 hash generated from data matches the provided checksum.
 *
 *  @param data - Data you want to hash
 *  @param length - Data length
 *  @param hash - `capi_checksum256*` hash to compare to
 *
 *  @pre **assert256 hash** of `data` equals provided `hash` parameter.
 *  @post Executes next statement. If was not `true`, hard return.
 *
 *  @note This method is optimized to a NO-OP when in fast evaluation mode.
 *
 *  Example:
 *
 *  @code
 *  checksum hash;
 *  char data;
 *  uint32_t length;
 *  assert_sha256( data, length, hash )
 *  //If the sha256 hash generated from data does not equal provided hash, anything below will never fire.
 *  eosio::print("sha256 hash generated from data equals provided hash");
 *  @endcode
 */
__attribute__((eosio_wasm_import))
void assert_sha256( const char* data, uint32_t length, const struct capi_checksum256* hash );

/**
 *  Tests if the sha1 hash generated from data matches the provided checksum.
 *
 *  @note This method is optimized to a NO-OP when in fast evaluation mode.
 *  @param data - Data you want to hash
 *  @param length - Data length
 *  @param hash - `capi_checksum160*` hash to compare to
 *
 *  @pre **sha1 hash** of `data` equals provided `hash` parameter.
 *  @post Executes next statement. If was not `true`, hard return.
 *
 *  Example:
*
 *  @code
 *  checksum hash;
 *  char data;
 *  uint32_t length;
 *  assert_sha1( data, length, hash )
 *  //If the sha1 hash generated from data does not equal provided hash, anything below will never fire.
 *  eosio::print("sha1 hash generated from data equals provided hash");
 *  @endcode
 */
__attribute__((eosio_wasm_import))
void assert_sha1( const char* data, uint32_t length, const struct capi_checksum160* hash );

/**
 *  Tests if the sha512 hash generated from data matches the provided checksum.
 *
 *  @note This method is optimized to a NO-OP when in fast evaluation mode.
 *  @param data - Data you want to hash
 *  @param length - Data length
 *  @param hash - `capi_checksum512*` hash to compare to
 *
 *  @pre **assert512 hash** of `data` equals provided `hash` parameter.
 *  @post Executes next statement. If was not `true`, hard return.
 *
 *  Example:
*
 *  @code
 *  checksum hash;
 *  char data;
 *  uint32_t length;
 *  assert_sha512( data, length, hash )
 *  //If the sha512 hash generated from data does not equal provided hash, anything below will never fire.
 *  eosio::print("sha512 hash generated from data equals provided hash");
 *  @endcode
 */
__attribute__((eosio_wasm_import))
void assert_sha512( const char* data, uint32_t length, const struct capi_checksum512* hash );

/**
 *  Tests if the ripemod160 hash generated from data matches the provided checksum.
 *
 *  @param data - Data you want to hash
 *  @param length - Data length
 *  @param hash - `capi_checksum160*` hash to compare to
 *
 *  @pre **assert160 hash** of `data` equals provided `hash` parameter.
 *  @post Executes next statement. If was not `true`, hard return.
 *
 *  Example:
*
 *  @code
 *  checksum hash;
 *  char data;
 *  uint32_t length;
 *  assert_ripemod160( data, length, hash )
 *  //If the ripemod160 hash generated from data does not equal provided hash, anything below will never fire.
 *  eosio::print("ripemod160 hash generated from data equals provided hash");
 *  @endcode
 */
__attribute__((eosio_wasm_import))
void assert_ripemd160( const char* data, uint32_t length, const struct capi_checksum160* hash );

/**
 *  Hashes `data` using `sha256` and stores result in memory pointed to by hash.
 *
 *  @param data - Data you want to hash
 *  @param length - Data length
 *  @param hash - Hash pointer
 *
 *  Example:
*
 *  @code
 *  checksum calc_hash;
 *  sha256( data, length, &calc_hash );
 *  eos_assert( calc_hash == hash, "invalid hash" );
 *  @endcode
 */
__attribute__((eosio_wasm_import))
void sha256( const char* data, uint32_t length, struct capi_checksum256* hash );

/**
 *  Hashes `data` using `sha1` and stores result in memory pointed to by hash.
 *
 *  @param data - Data you want to hash
 *  @param length - Data length
 *  @param hash - Hash pointer
 *
 *  Example:
*
 *  @code
 *  checksum calc_hash;
 *  sha1( data, length, &calc_hash );
 *  eos_assert( calc_hash == hash, "invalid hash" );
 *  @endcode
 */
__attribute__((eosio_wasm_import))
void sha1( const char* data, uint32_t length, struct capi_checksum160* hash );

/**
 *  Hashes `data` using `sha512` and stores result in memory pointed to by hash.
 *
 *  @param data - Data you want to hash
 *  @param length - Data length
 *  @param hash - Hash pointer
 *
 *  Example:
*
 *  @code
 *  checksum calc_hash;
 *  sha512( data, length, &calc_hash );
 *  eos_assert( calc_hash == hash, "invalid hash" );
 *  @endcode
 */
__attribute__((eosio_wasm_import))
void sha512( const char* data, uint32_t length, struct capi_checksum512* hash );

/**
 *  Hashes `data` using `ripemod160` and stores result in memory pointed to by hash.
 *
 *  @param data - Data you want to hash
 *  @param data_len - Data length
 *  @param hash - Hash pointer
 *
 *  Example:
*
 *  @code
 *  checksum calc_hash;
 *  ripemod160( data, length, &calc_hash );
 *  eos_assert( calc_hash == hash, "invalid hash" );
 *  @endcode
 */
__attribute__((eosio_wasm_import))
void ripemd160( const char* data, uint32_t length, struct capi_checksum160* hash );

/**
 *  Calculates the public key used for a given signature and hash used to create a message.
 *
 *  @param digest - Hash used to create a message
 *  @param sig - Signature
 *  @param siglen - Signature length
 *  @param pub - Public key
 *  @param publen - Public key length
*   @return int - number of bytes written to pub
 *
 *  Example:
*
 *  @code
 *  @endcode
 */
__attribute__((eosio_wasm_import))
int recover_key( const struct capi_checksum256* digest, const char* sig, size_t siglen, char* pub, size_t publen );

/**
 *  Tests a given public key with the generated key from digest and the signature.
 *
 *  @param digest - What the key will be generated from
 *  @param sig - Signature
 *  @param siglen - Signature length
 *  @param pub - Public key
 *  @param publen - Public key length
 *
 *  @pre **assert recovery key** of `pub` equals the key generated from the `digest` parameter
 *  @post Executes next statement. If was not `true`, hard return.
 *
 *  Example:
*
 *  @code
 *  checksum digest;
 *  char sig;
 *  size_t siglen;
 *  char pub;
 *  size_t publen;
 *  assert_recover_key( digest, sig, siglen, pub, publen )
 *  // If the given public key does not match with the generated key from digest and the signature, anything below will never fire.
 *  eosio::print("pub key matches the pub key generated from digest");
 *  @endcode
 */
__attribute__((eosio_wasm_import))
void assert_recover_key( const struct capi_checksum256* digest, const char* sig, size_t siglen, const char* pub, size_t publen );

/**
 * Tests a signature against a hash, verifies the recovered public key matches the expected one,
 * and checks that this public key belongs to the specified account permission.
 *
 * @ingroup crypto
 * @param digest - Pointer to the digest/hash of the message that was signed
 * @param sig - Pointer to the signature
 * @param siglen - Length of the signature
 * @param pub - Pointer to the expected public key
 * @param publen - Length of the expected public key
 * @param account - The account name to verify key ownership
 * @param permission - The permission name to check (e.g., active, owner)
 *
 * @pre `digest` is a valid pointer to a `capi_checksum256` struct
 * @pre `sig` is a valid pointer to a buffer containing the signature
 * @pre `pub` is a valid pointer to a buffer containing the expected public key
 *
 * @post Throws if signature is invalid, key doesn't match, or key doesn't belong to account permission
 *
 * Example:
 * @code
 *  capi_checksum256 digest;
 *  char sig[65];
 *  size_t siglen = 65;
 *  char pub[34];
 *  size_t publen = 34;
 *  capi_name account = N(myaccount);
 *  capi_name permission = N(active);
 *  assert_recover_key_account( &digest, sig, siglen, pub, publen, account, permission );
 *  // If all checks pass, code continues here
 * @endcode
 */
__attribute__((eosio_wasm_import))
void assert_recover_key_account( const struct capi_checksum256* digest, const char* sig, size_t siglen, const char* pub, size_t publen, uint64_t account, uint64_t permission );

#ifdef __cplusplus
}
#endif
/// @}
