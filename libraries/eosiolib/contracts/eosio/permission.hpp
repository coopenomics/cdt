/**
 *  @file
 *  @copyright см. eos/LICENSE
 */
#pragma once

#include "transaction.hpp"
#include "../../core/eosio/crypto.hpp"
#include "../../core/eosio/name.hpp"
#include "../../core/eosio/time.hpp"

#include <set>
#include <limits>

namespace eosio {
   namespace internal_use_do_not_use {
      extern "C" {
         __attribute__((eosio_wasm_import))
         int32_t check_transaction_authorization(const char*,  uint32_t,
                                          const char*, uint32_t,
                                          const char*, uint32_t);
         __attribute__((eosio_wasm_import))
         int32_t check_permission_authorization( uint64_t, uint64_t,
                                                 const char*, uint32_t,
                                                 const char*, uint32_t,
                                                 uint64_t);
         __attribute__((eosio_wasm_import))
         int64_t get_permission_last_used(uint64_t, uint64_t);

         __attribute__((eosio_wasm_import))
         int64_t get_account_creation_time(uint64_t);
      }
   }

   /**
   *  Проверяет, авторизована ли транзакция заданным набором ключей и разрешений (permissions)
   *  @ingroup permission
   *
   *  @param trx_data — указатель на начало сериализованной транзакции
   *  @param trx_size — размер (в байтах) сериализованной транзакции
   *  @param pubkeys_data — указатель на начало сериализованного вектора открытых ключей
   *  @param pubkeys_size — размер (в байтах) сериализованного вектора ключей (0, если ключи не передаются)
   *  @param perms_data — указатель на начало сериализованного вектора разрешений (пустое имя разрешения — подстановка (совпадает с любым))
   *  @param perms_size — размер (в байтах) сериализованного вектора разрешений
   *
   *  @return 1, если транзакция авторизована; иначе 0
   */
   bool
   check_transaction_authorization( const char* trx_data,     uint32_t trx_size,
                                    const char* pubkeys_data, uint32_t pubkeys_size,
                                    const char* perms_data,   uint32_t perms_size ) {
      return internal_use_do_not_use::check_transaction_authorization( trx_data, trx_size, pubkeys_data, pubkeys_size, perms_data, perms_size );
   }

   /**
   *  Проверяет, авторизовано ли разрешение заданной задержкой и набором ключей и разрешений
   *  @ingroup permission
   *
   *  @param account — аккаунт-владелец разрешения
   *  @param permission — имя проверяемого разрешения
   *  @param pubkeys_data — указатель на начало сериализованного вектора открытых ключей
   *  @param pubkeys_size — размер (в байтах) сериализованного вектора ключей (0, если ключи не передаются)
   *  @param perms_data — указатель на начало сериализованного вектора разрешений (пустое имя — подстановка (совпадает с любым))
   *  @param perms_size — размер (в байтах) сериализованного вектора разрешений
   *  @param delay — задержка в микросекундах (не больше INT64_MAX)
   *
   *  @return 1, если разрешение авторизовано; иначе 0
   */
   bool
   check_permission_authorization( name account,
                                   name permission,
                                   const char* pubkeys_data, uint32_t pubkeys_size,
                                   const char* perms_data,   uint32_t perms_size,
                                   microseconds delay ) {
      int64_t delay_us = delay.count();
      check(delay_us >= 0, "negative delay is not allowed");
      return internal_use_do_not_use::check_permission_authorization( account.value, permission.value, pubkeys_data, pubkeys_size, perms_data, perms_size,  static_cast<uint64_t>(delay_us) );
   }


  /**
   * @defgroup permission Разрешение (permission)
   * @ingroup contracts
   * @ingroup types
   * @brief Определяет функции C++ API COOPOS для проверки авторизации ключей и разрешений (permissions)
   */

   /**
    *  Проверяет, авторизована ли транзакция заданным набором ключей и разрешений
    *  @ingroup permission
    *
    *  @param trx — транзакция, для которой проверяется авторизация
    *  @param provided_permissions — набор разрешений, которыми подписана транзакция (пустое имя — подстановка (совпадает с любым))
    *  @param provided_keys — набор открытых ключей, которыми подписана транзакция
    *
    *  @return true, если транзакция авторизована указанными ключами и разрешениями
    */
   bool
   check_transaction_authorization( const transaction&                 trx,
                                    const std::set<permission_level>&  provided_permissions ,
                                    const std::set<public_key>&        provided_keys = std::set<public_key>()
                                  )
   {
      auto packed_trx = pack(trx);

      std::vector<char> packed_keys;
      auto nkeys = provided_keys.size();
      if( nkeys > 0 ) {
         packed_keys = pack(provided_keys);
      }

      std::vector<char> packed_perms;
      auto nperms = provided_permissions.size();
      if( nperms > 0 ) {
         packed_perms = pack(provided_permissions);
      }

      auto res = internal_use_do_not_use::check_transaction_authorization( packed_trx.data(),
                                                    packed_trx.size(),
                                                    (nkeys > 0)  ? packed_keys.data()  : (const char*)0,
                                                    (nkeys > 0)  ? packed_keys.size()  : 0,
                                                    (nperms > 0) ? packed_perms.data() : (const char*)0,
                                                    (nperms > 0) ? packed_perms.size() : 0
                                                  );

      return (res > 0);
   }

   /**
    *  Проверяет, авторизовано ли разрешение заданной задержкой и набором ключей и разрешений
    *
    *  @ingroup permission
    *
    *  @param account — аккаунт-владелец разрешения
    *  @param permission — имя проверяемого разрешения
    *  @param provided_keys — набор открытых ключей, подписавших операцию
    *  @param provided_permissions — набор разрешений (пустое имя — подстановка (совпадает с любым))
    *  @param provided_delay_us — задержка в микросекундах (не больше INT64_MAX)
    *
    *  @return true, если разрешение удовлетворено указанной задержкой, ключами и разрешениями
    */
   bool
   check_permission_authorization( name                               account,
                                   name                               permission,
                                   const std::set<public_key>&        provided_keys,
                                   const std::set<permission_level>&  provided_permissions = std::set<permission_level>(),
                                   microseconds                       provided_delay = microseconds{std::numeric_limits<int64_t>::max()}
                                 )
   {
      int64_t provided_delay_us = provided_delay.count();
      check(provided_delay_us >= 0, "negative delay is not allowed");
      std::vector<char> packed_keys;
      auto nkeys = provided_keys.size();
      if( nkeys > 0 ) {
         packed_keys = pack(provided_keys);
      }

      std::vector<char> packed_perms;
      auto nperms = provided_permissions.size();
      if( nperms > 0 ) {
         packed_perms = pack(provided_permissions);
      }

      auto res = internal_use_do_not_use::check_permission_authorization( account.value,
                                                   permission.value,
                                                   (nkeys > 0)  ? packed_keys.data()  : (const char*)0,
                                                   (nkeys > 0)  ? packed_keys.size()  : 0,
                                                   (nperms > 0) ? packed_perms.data() : (const char*)0,
                                                   (nperms > 0) ? packed_perms.size() : 0,
                                                   static_cast<uint64_t>(provided_delay_us)
                                                 );

      return (res > 0);
   }

   /**
    *  Возвращает время последнего использования разрешения
    *
    *  @ingroup permission
    *
    *  @param account — аккаунт-владелец разрешения
    *  @param permission — имя разрешения
    *
    *  @return момент последнего использования (микросекунды с начала эпохи Unix)
    */
   time_point get_permission_last_used( name account, name permission ) {
      return time_point(
               microseconds(
                  internal_use_do_not_use::get_permission_last_used(account.value, permission.value)));

   }

   /**
    *  Возвращает время создания аккаунта
    *
    *  @ingroup permission
    *
    *  @param account — аккаунт
    *
    *  @return момент создания (микросекунды с начала эпохи Unix)
    */
   time_point get_account_creation_time( name account ) {
      return time_point(
               microseconds(
                  internal_use_do_not_use::get_account_creation_time(account.value)));

   }
}
