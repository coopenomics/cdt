/**
 *  @file
 *  @copyright см. eos/LICENSE
 */
#pragma once

#include <string>
#include <string_view>

namespace eosio {

   namespace internal_use_do_not_use {
      extern "C" {
         __attribute__((eosio_wasm_import))
         void eosio_assert( uint32_t test, const char* msg );

         __attribute__((eosio_wasm_import))
         void eosio_assert_message( uint32_t test, const char* msg, uint32_t msg_len );

         __attribute__((eosio_wasm_import))
         void eosio_assert_code( uint32_t test, uint64_t code );
      }
   }

   /**
    *  @defgroup system Система
    *  @ingroup core
    *  @brief Обёртки над eosio_assert
    */


   /**
    *  Прерывает выполнение контракта, если pred == false; сообщение msg передаётся в среду COOPOS.
    *
    *  @ingroup system
    *
    *  Пример:
    *  @code
    *  eosio::check(a == b, "a does not equal b");
    *  @endcode
    */
   inline void check(bool pred, std::string_view msg) {
      if (!pred)
         internal_use_do_not_use::eosio_assert_message(false, msg.data(), msg.size());
   }

   /**
    *  Прерывает выполнение контракта, если pred == false; сообщение — C-строка msg.
    *
    *  @ingroup system
    *
    *  Пример:
    *  @code
    *  eosio::check(a == b, "a does not equal b");
    *  @endcode
    */
   inline void check(bool pred, const char* msg) {
      if (!pred) {
         internal_use_do_not_use::eosio_assert(false, msg);
      }
   }

    /**
    *  Прерывает выполнение контракта, если pred == false; сообщение — std::string.
    *
    *  @ingroup system
    *
    *  Пример:
    *  @code
    *  eosio::check(a == b, "a does not equal b");
    *  @endcode
    */
   inline void check(bool pred, const std::string& msg) {
      if (!pred) {
         internal_use_do_not_use::eosio_assert_message(false, msg.data(), msg.size());
      }
   }

   /**
    *  Прерывает выполнение контракта, если pred == false; сообщение — rvalue std::string.
    *
    *  @ingroup system
    *
    *  Пример:
    *  @code
    *  eosio::check(a == b, "a does not equal b");
    *  @endcode
    */
   inline void check(bool pred, std::string&& msg) {
      if (!pred) {
         internal_use_do_not_use::eosio_assert_message(false, msg.data(), msg.size());
      }
   }


   /**
    *  Прерывает выполнение контракта, если pred == false; в среду передаются первые n байт сообщения msg.
    *
    *  @ingroup system
    *
    *  Пример:
    *  @code
    *  const char* msg = "a does not equal b b does not equal a";
    *  eosio::check(a == b, "a does not equal b", 18);
    *  @endcode
    */
   inline void check(bool pred, const char* msg, size_t n) {
      if (!pred) {
         internal_use_do_not_use::eosio_assert_message(false, msg, n);
      }
   }

   /**
    *  Прерывает выполнение контракта, если pred == false; в среду передаются первые n символов строки msg.
    *
    *  @ingroup system
    *
    *  Пример:
    *  @code
    *  std::string msg = "a does not equal b b does not equal a";
    *  eosio::check(a == b, msg, 18);
    *  @endcode
    */
   inline void check(bool pred, const std::string& msg, size_t n) {
      if (!pred) {
         internal_use_do_not_use::eosio_assert_message(false, msg.data(), n);
      }
   }

    /**
    *  Прерывает выполнение контракта, если pred == false; передаётся числовой код ошибки code (eosio_assert_code).
    *
    *  @ingroup system
    *
    *  Пример:
    *  @code
    *  eosio::check(a == b, 13);
    *  @endcode
    */
   inline void check(bool pred, uint64_t code) {
      if (!pred) {
         internal_use_do_not_use::eosio_assert_code(false, code);
      }
   }
} // namespace eosio
