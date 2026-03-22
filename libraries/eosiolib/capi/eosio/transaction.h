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
 * @addtogroup transaction_c Transaction API
 * @ingroup c_api
 * @brief Определяет C API для отправки транзакций и inline-действий
 *
 * @details Отложенные транзакции не выполняются до одного из будущих блоков. Поэтому
 * при корректном виде они не влияют на успех или неудачу родительской транзакции.
 * Если по иным причинам родительская транзакция помечается как неуспешная,
 * отложенная транзакция никогда не будет обработана.
 *
 * Отложенные транзакции должны укладываться в разрешения, доступные родительской
 * транзакции, либо (в будущем) делегированные учётной записи контракта.
 *
 * Inline-сообщение позволяет одному контракту отправить другому сообщение,
 * обрабатываемое сразу после завершения текущего сообщения; успех или неудача
 * родительской транзакции зависит от успеха этого сообщения. При сбое обработки
 * inline-сообщения всё дерево транзакций и действий, укоренённое в блоке,
 * помечается как неуспешное, и эффекты в базе состояния не сохраняются.
 *
 * Inline-действия и отложенные транзакции должны укладываться в разрешения,
 * доступные родительской транзакции, либо (в будущем) делегированные учётной
 * записи контракта.
 * @{
 */

 /**
  *  Sends a deferred transaction.
  *
  *  @param sender_id - ID of sender
  *  @param payer - Account paying for RAM
  *  @param serialized_transaction - Pointer of serialized transaction to be deferred
  *  @param size - Size to reserve
  *  @param replace_existing - f this is `0` then if the provided sender_id is already in use by an in-flight transaction from this contract, which will be a failing assert. If `1` then transaction will atomically cancel/replace the inflight transaction
  */
__attribute__((eosio_wasm_import))
void send_deferred(const uint128_t* sender_id, capi_name payer, const char *serialized_transaction, size_t size, uint32_t replace_existing);

 /**
  *  Cancels a deferred transaction.
  *
  *  @brief Отменить отложенную транзакцию
  *  @param sender_id - The id of the sender
  *
  *  @pre The deferred transaction ID exists.
  *  @pre The deferred transaction ID has not yet been published.
  *  @post Deferred transaction canceled.
  *
  *  @return 1 if transaction was canceled, 0 if transaction was not found
  *
  *  Example:
*
  *  @code
  *  id = 0xffffffffffffffff
  *  cancel_deferred( id );
  *  @endcode
  */
__attribute__((eosio_wasm_import))
int cancel_deferred(const uint128_t* sender_id);

/**
 * Access a copy of the currently executing transaction.
 *
 * @brief Получить копию текущей выполняемой транзакции
 * @param buffer - a buffer to write the current transaction to
 * @param size - the size of the buffer, 0 to return required size
 * @return the size of the transaction written to the buffer, or number of bytes that can be copied if size==0 passed
 */
__attribute__((eosio_wasm_import))
size_t read_transaction(char *buffer, size_t size);

/**
 * Gets the size of the currently executing transaction.
 *
 * @brief Получить размер текущей выполняемой транзакции
 * @return size of the currently executing transaction
 */
__attribute__((eosio_wasm_import))
size_t transaction_size( void );

/**
 * Gets the block number used for TAPOS on the currently executing transaction.
 *
 * @brief Получить номер блока TAPOS для текущей выполняемой транзакции
 * @return block number used for TAPOS on the currently executing transaction
 * Example:
 * @code
 * int tbn = tapos_block_num();
 * @endcode
 */
__attribute__((eosio_wasm_import))
int tapos_block_num( void );

/**
 * Gets the block prefix used for TAPOS on the currently executing transaction.
 *
 * @brief Получить префикс блока TAPOS для текущей выполняемой транзакции
 * @return block prefix used for TAPOS on the currently executing transaction
 * Example:
 * @code
 * int tbp = tapos_block_prefix();
 * @endcode
 */
__attribute__((eosio_wasm_import))
int tapos_block_prefix( void );

/**
 * Gets the expiration of the currently executing transaction.
 *
 * @brief Получить срок действия (expiration) текущей выполняемой транзакции
 * @return expiration of the currently executing transaction in seconds since Unix epoch
 * Example:
 * @code
 * uint32_t tm = expiration();
 * eosio_print(tm);
 * @endcode
 */
__attribute__((eosio_wasm_import))
uint32_t expiration( void );

/**
 * Извлечь указанное действие из активной транзакции
 *
 * @brief Извлечь указанное действие из активной транзакции
 * @param type - 0 for context free action, 1 for action
 * @param index - the index of the requested action
 * @param buff - output packed buff of the action
 * @param size - amount of buff read, pass 0 to have size returned
 * @return the size of the action, -1 on failure
 */
__attribute__((eosio_wasm_import))
int get_action( uint32_t type, uint32_t index, char* buff, size_t size );

/**
 * Retrieve the signed_transaction.context_free_data[index].
 *
 * @brief Извлечь signed_transaction.context_free_data[index]
 * @param index - the index of the context_free_data entry to retrieve
 * @param buff - output buff of the context_free_data entry
 * @param size - amount of context_free_data[index] to retrieve into buff, 0 to report required size
 * @return size copied, or context_free_data[index].size() if 0 passed for size, or -1 if index not valid
 */
__attribute__((eosio_wasm_import))
int get_context_free_data( uint32_t index, char* buff, size_t size );

#ifdef __cplusplus
}
#endif
///}@
