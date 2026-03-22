/**
 *  @file
 *  @copyright как определено в eos/LICENSE
 */
#pragma once
#include "../../core/eosio/time.hpp"
#include "../../core/eosio/check.hpp"
#include "../../core/eosio/fixed_bytes.hpp"
#include "../../core/eosio/name.hpp"

namespace eosio {
  namespace internal_use_do_not_use {
    extern "C" {
      __attribute__((eosio_wasm_import, noreturn))
      void eosio_exit( int32_t code );

      struct  __attribute__((aligned (16))) capi_checksum256 {
         uint8_t hash[32];
      };

      __attribute__((eosio_wasm_import))
      bool is_feature_activated( const capi_checksum256* feature_digest );

      __attribute__((eosio_wasm_import))
      uint64_t get_sender();
      
      __attribute__((eosio_wasm_import))
      uint32_t get_block_num();

    }
  }

  /**
   *  @addtogroup system Системные функции (System)
   *  @ingroup contracts
   *  @brief Определяет функции, связанные со временем, и eosio_exit
   */

   /**
    *  Прерывает выполнение WASM без признания контракта провалившимся.
    *  Используется, чтобы обойти обычную очистку и деструкторы при нормальном выходе.
    *
       <html><p><b>
       ВНИМАНИЕ: метод немедленно обрывает выполнение WASM-кода на стеке, который
                выполнился бы при обычном возврате из функции.
                Возможны проблемы с кэшами записи, RAII, подсчётом ссылок,
                если выполнение WASM прерывается сразу.
       </b></p></html>
    *  @ingroup system
    *
    *  @param code — код выхода
    *    Пример:
    *
    *      @code
    *      eosio_exit(0);
    *      eosio_exit(1);
    *      eosio_exit(2);
    *      eosio_exit(3);
    *      @endcode
   */
   inline void eosio_exit( int32_t code ) {
     internal_use_do_not_use::eosio_exit(code);
   }

   /**
   *  Время текущего блока в микросекундах от 1970 года как time_point
   *
   *  @ingroup system
   *  @return время текущего блока в микросекундах от 1970 года (time_point)
   */
   time_point current_time_point();

   /**
   *  Время текущего блока в микросекундах от 1970 года как block_timestamp
   *
   *  @ingroup system
   *  @return время текущего блока в микросекундах от 1970 года (block_timestamp)
   */
   block_timestamp current_block_time();

   using block_num_t = uint32_t;

   /**
   *  Номер текущего блока
   *
   *  @ingroup system
   *  @return номер текущего блока
   */
   inline block_num_t current_block_number() {
      return internal_use_do_not_use::get_block_num();
   }

   /**
    * Проверяет, активирована ли указанная протокольная возможность в COOPOS
    *
    * @ingroup system
    * @param feature_digest — дайджест протокольной возможности
    * @return true, если возможность активирована, иначе false
    */
   inline bool is_feature_activated( const checksum256& feature_digest ) {
      auto feature_digest_data = feature_digest.extract_as_byte_array();
      return internal_use_do_not_use::is_feature_activated(
         reinterpret_cast<const internal_use_do_not_use::capi_checksum256*>( feature_digest_data.data() )
      );
   }

   /**
    * Имя аккаунта, отправившего текущее inline-действие
    *
    * @ingroup system
    * @return имя отправителя текущего inline-действия (пустое имя, если вызов не из inline-действия)
    */
   inline name get_sender() {
      return name( internal_use_do_not_use::get_sender() );
   }
}
