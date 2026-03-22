#pragma once
#include "producer_schedule.hpp"
#include "system.hpp"
#include "../../core/eosio/crypto.hpp"
#include "../../core/eosio/name.hpp"
#include "../../core/eosio/serialize.hpp"

namespace eosio {

   namespace internal_use_do_not_use {
      extern "C" {
         __attribute__((eosio_wasm_import))
         bool is_privileged( uint64_t account );

        __attribute__((eosio_wasm_import))
         void get_account_ram_usage( uint64_t account, int64_t* used_ram_bytes );

         __attribute__((eosio_wasm_import))
         void get_resource_limits( uint64_t account, int64_t* ram_bytes, int64_t* net_weight, int64_t* cpu_weight );

         __attribute__((eosio_wasm_import))
         void set_resource_limits( uint64_t account, int64_t ram_bytes, int64_t net_weight, int64_t cpu_weight );

         __attribute__((eosio_wasm_import))
         void set_privileged( uint64_t account, bool is_priv );

         __attribute__((eosio_wasm_import))
         void set_blockchain_parameters_packed( char* data, uint32_t datalen );

         __attribute__((eosio_wasm_import))
         uint32_t get_blockchain_parameters_packed( char* data, uint32_t datalen );

         __attribute((eosio_wasm_import))
         int64_t set_proposed_producers( char*, uint32_t );

         __attribute__((eosio_wasm_import))
         void preactivate_feature( const capi_checksum256* feature_digest );

         __attribute__((eosio_wasm_import))
         int64_t set_proposed_producers_ex( uint64_t producer_data_format, char *producer_data, uint32_t producer_data_size );
      }
   }

  /**
   *  @defgroup privileged Привилегированный API
   *  @ingroup contracts
   *  @brief Определяет привилегированный C++ API COOPOS
   */

   /**
    *  Настраиваемые параметры цепочки COOPOS, изменяемые по консенсусу
    *  @ingroup privileged
    */
   struct blockchain_parameters {

      /**
      * Максимальное использование сети в инструкциях для блока
      * @brief максимальное использование сети в инструкциях для блока
      */
      uint64_t max_block_net_usage;

      /**
      * Целевой процент (1% == 100, 100% == 10 000) от максимального использования сети; при превышении включается обработка перегрузки
      * @brief целевой процент (1% == 100, 100% == 10 000) от максимального использования сети; превышение включает обработку перегрузки
      */
      uint32_t target_block_net_usage_pct;

      /**
      * Максимальное объективно измеряемое использование сети, которое цепочка COOPOS допускает независимо от лимитов аккаунтов
      * @brief максимальное объективно измеряемое использование сети, которое цепочка допускает независимо от лимитов аккаунтов
      */
      uint32_t max_transaction_net_usage;

      /**
       * Базовый объём использования сети, тарифицируемый для транзакции (накладные расходы)
       */
      uint32_t base_per_transaction_net_usage;

      /**
       * Запас по использованию сети при выполнении транзакции (в конце транзакции проверка выполняется по новым лимитам без запаса)
       * @brief запас по использованию сети при выполнении транзакции (в конце транзакции проверка всё равно выполняется по новым лимитам без запаса)
       */
      uint32_t net_usage_leeway;

      /**
      * Числитель скидки на использование сети для данных без контекста (context-free)
      * @brief числитель скидки на использование сети для данных без контекста (context-free)
      */
      uint32_t context_free_discount_net_usage_num;

      /**
      * Знаменатель скидки на использование сети для данных без контекста (context-free)
      * @brief знаменатель скидки на использование сети для данных без контекста (context-free)
      */
      uint32_t context_free_discount_net_usage_den;

      /**
      * Максимальная тарифицируемая нагрузка CPU (в микросекундах) для блока
      * @brief максимальная тарифицируемая нагрузка CPU (в микросекундах) для блока
      */
      uint32_t max_block_cpu_usage;

      /**
      * Целевой процент (1% == 100, 100% == 10 000) от максимального использования CPU; при превышении включается обработка перегрузки
      * @brief целевой процент (1% == 100, 100% == 10 000) от максимального использования CPU; превышение включает обработку перегрузки
      */
      uint32_t target_block_cpu_usage_pct;

      /**
      * Максимальная тарифицируемая нагрузка CPU (в микросекундах), которую цепочка COOPOS допускает независимо от лимитов аккаунтов
      * @brief максимальная тарифицируемая нагрузка CPU (в микросекундах), которую цепочка допускает независимо от лимитов аккаунтов
      */
      uint32_t max_transaction_cpu_usage;

      /**
      * Минимальная тарифицируемая нагрузка CPU (в микросекундах), которую требует цепочка COOPOS
      * @brief минимальная тарифицируемая нагрузка CPU (в микросекундах), которую требует цепочка
      */
      uint32_t min_transaction_cpu_usage;

      /**
       * Максимальное время жизни транзакции
       * @brief максимальное время жизни транзакции
       */
      uint32_t max_transaction_lifetime;

      /**
      * Число секунд от момента, когда отложенная транзакция может впервые выполниться, до её истечения
      * @brief число секунд от момента, когда отложенная транзакция может впервые выполниться, до её истечения
      */
      uint32_t deferred_trx_expiration_window;


      /**
      * Максимальное число секунд задержки, которое могут потребовать проверки авторизации
      * @brief максимальное число секунд задержки, которое могут потребовать проверки авторизации
      */
      uint32_t max_transaction_delay;

      /**
       * Максимальный размер встроенного (inline) действия
       * @brief максимальный размер встроенного (inline) действия
       */
      uint32_t max_inline_action_size;

      /**
       * Максимальная глубина вложенности встроенных (inline) действий
       * @brief максимальная глубина вложенности встроенных (inline) действий
       */
      uint16_t max_inline_action_depth;

      /**
       * Максимальная глубина полномочий (authority)
       * @brief максимальная глубина полномочий (authority)
       */
      uint16_t max_authority_depth;


      EOSLIB_SERIALIZE( blockchain_parameters,
                        (max_block_net_usage)(target_block_net_usage_pct)
                        (max_transaction_net_usage)(base_per_transaction_net_usage)(net_usage_leeway)
                        (context_free_discount_net_usage_num)(context_free_discount_net_usage_den)

                        (max_block_cpu_usage)(target_block_cpu_usage_pct)
                        (max_transaction_cpu_usage)(min_transaction_cpu_usage)

                        (max_transaction_lifetime)(deferred_trx_expiration_window)(max_transaction_delay)
                        (max_inline_action_size)(max_inline_action_depth)(max_authority_depth)
      )
   };

   /**
    *  Устанавливает параметры цепочки COOPOS
    *
    *  @ingroup privileged
    *  @param params — новые параметры цепочки
    */
   void set_blockchain_parameters(const eosio::blockchain_parameters& params);

   /**
    *  Возвращает параметры цепочки COOPOS
    *
    *  @ingroup privileged
    *  @param params — заполняется полученными параметрами цепочки
    */
   void get_blockchain_parameters(eosio::blockchain_parameters& params);


    /**
    *  Возвращает использование RAM аккаунтом
    *
    *  @ingroup privileged
    *  @param account — имя аккаунта
    *  @param used_ram_bytes — выход: использовано RAM в байтах
    */
   inline void get_account_ram_usage( name account, int64_t& used_ram_bytes ) {
      internal_use_do_not_use::get_account_ram_usage( account.value, &used_ram_bytes);
   }


    /**
    *  Возвращает лимиты ресурсов аккаунта
    *
    *  @ingroup privileged
    *  @param account — имя аккаунта
    *  @param ram_bytes — выход: лимит RAM в байтах
    *  @param net_weight — выход: вес сети (net)
    *  @param cpu_weight — выход: вес CPU
    */
   inline void get_resource_limits( name account, int64_t& ram_bytes, int64_t& net_weight, int64_t& cpu_weight ) {
      internal_use_do_not_use::get_resource_limits( account.value, &ram_bytes, &net_weight, &cpu_weight );
   }

   /**
    *  Задаёт лимиты ресурсов аккаунта
    *
    *  @ingroup privileged
    *  @param account — имя аккаунта
    *  @param ram_bytes — лимит RAM в байтах
    *  @param net_weight — доля лимита сети пропорционально (вес / сумма весов всех аккаунтов)
    *  @param cpu_weight — доля лимита CPU пропорционально (вес / сумма весов всех аккаунтов)
    */
   inline void set_resource_limits( name account, int64_t ram_bytes, int64_t net_weight, int64_t cpu_weight ) {
      internal_use_do_not_use::set_resource_limits( account.value, ram_bytes, net_weight, cpu_weight );
   }

   /**
    *  Предлагает смену расписания в устаревшем формате ключей производителей
    *
    *  @ingroup privileged
    *  @note Когда блок с предложением становится необратимым, расписание автоматически переходит в состояние «ожидающее» (pending). Когда блок, продвигающий расписание, становится необратимым, расписание становится «активным» (active)
    *  @param producers — вектор ключей производителей
    *
    *  @return при успехе — номер версии нового предложенного расписания; иначе пусто
    */
   std::optional<uint64_t> set_proposed_producers( const std::vector<producer_key>& prods );

   /**
    *  Предлагает смену расписания в более гибком формате полномочий
    *
    *  @ingroup privileged
    *  @note Когда блок с предложением становится необратимым, расписание автоматически переходит в «pending». Когда блок, продвигающий расписание, становится необратимым, расписание становится «active»
    *  @param producers — вектор полномочий производителей
    *
    *  @return при успехе — версия нового предложенного расписания; иначе пусто
    */
   inline std::optional<uint64_t> set_proposed_producers( const std::vector<producer_authority>& prods ) {
      auto packed_prods = eosio::pack( prods );
      int64_t ret = internal_use_do_not_use::set_proposed_producers_ex(1, (char*)packed_prods.data(), packed_prods.size());
      if (ret >= 0)
        return static_cast<uint64_t>(ret);
      return {};
   }

   /**
    *  Проверяет, является ли аккаунт привилегированным
    *
    *  @ingroup privileged
    *  @param account — имя проверяемого аккаунта
    *  @return true, если аккаунт привилегированный
    *  @return false, если аккаунт не привилегированный
    */
   inline bool is_privileged( name account ) {
      return internal_use_do_not_use::is_privileged( account.value );
   }

   /**
    *  Задаёт привилегированный статус аккаунта
    *
    *  @ingroup privileged
    *  @param account — имя аккаунта
    *  @param is_priv — признак привилегированного статуса
    */
   inline void set_privileged( name account, bool is_priv ) {
      internal_use_do_not_use::set_privileged( account.value, is_priv );
   }

   /**
    *  Предварительно активирует возможность протокола COOPOS
    *
    * @ingroup privileged
    * @param feature_digest — дайджест возможности протокола для предактивации
    */
   inline void preactivate_feature( const checksum256& feature_digest ) {
      auto feature_digest_data = feature_digest.extract_as_byte_array();
      internal_use_do_not_use::preactivate_feature(
         reinterpret_cast<const internal_use_do_not_use::capi_checksum256*>( feature_digest_data.data() )
      );
   }

}
