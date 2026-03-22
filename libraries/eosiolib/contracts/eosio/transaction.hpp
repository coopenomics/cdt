/**
 *  @file
 *  @copyright см. eos/LICENSE
 */
#pragma once
#include "action.hpp"
#include "system.hpp"
#include "../../core/eosio/time.hpp"
#include "../../core/eosio/serialize.hpp"

#include <vector>

namespace eosio {
   namespace internal_use_do_not_use {
      extern "C" {
         __attribute__((eosio_wasm_import))
         void send_deferred(const uint128_t&, uint64_t, const char*, size_t, uint32_t);

         __attribute__((eosio_wasm_import))
         int cancel_deferred(const uint128_t&);

         __attribute__((eosio_wasm_import))
         size_t read_transaction(char*, size_t);

         __attribute__((eosio_wasm_import))
         size_t transaction_size();

         __attribute__((eosio_wasm_import))
         int tapos_block_num();

         __attribute__((eosio_wasm_import))
         int tapos_block_prefix();

         __attribute__((eosio_wasm_import))
         uint32_t expiration();

         __attribute__((eosio_wasm_import))
         int get_action( uint32_t, uint32_t, char*, size_t);

         __attribute__((eosio_wasm_import))
         int get_context_free_data( uint32_t, char*, size_t);
      }
   }

  /**
   *  @defgroup transaction Транзакция
   *  @ingroup contracts
   *  @brief Типобезопасные C++-обёртки над C API транзакций COOPOS
   *
   *  @details Встроенное (inline) сообщение позволяет одному контракту отправить другому контракту сообщение,
   *  которое обрабатывается сразу после завершения обработки текущего сообщения,
   *  так что успех или неудача родительской транзакции зависят от успеха этого сообщения.
   *  Если встроенное сообщение завершается с ошибкой при обработке, то всё дерево транзакций и действий,
   *  укоренённое в блоке, помечается как неуспешное, и ни один эффект в базе данных не сохраняется.
   *
   *  Встроенные действия и отложенные транзакции должны соблюдать разрешения (permissions),
   *  доступные родительской транзакции или, в будущем, делегированные аккаунту контракта для последующего использования.
   *
   *  @note Некоторые методы из @ref transactioncapi можно вызывать напрямую из C++
   */

  /**
   *  @ingroup transaction
   */
   typedef std::tuple<uint16_t, std::vector<char>> extension;

   /**
    *  @ingroup transaction
    */
   typedef std::vector<extension> extensions_type;

   /**
    *  Класс transaction_header содержит сведения о транзакции
    *
    *  @ingroup transaction
    *  @brief Содержит сведения о транзакции
    */

   class transaction_header {
   public:

      /**
       * Создаёт новый transaction_header со сроком действия «сейчас + 60 с».
       *
       * @brief Создаёт объект transaction_header со сроком действия заголовка транзакции «сейчас + 60 с»
       */
      transaction_header( time_point_sec exp = time_point_sec(current_time_point()) + 60)
         :expiration(exp)
      {}

      time_point_sec  expiration;
      uint16_t        ref_block_num;
      uint32_t        ref_block_prefix;
      unsigned_int    max_net_usage_words = 0UL; /// число 8-байтовых слов, в которые может быть сериализована транзакция после сжатия
      uint8_t         max_cpu_usage_ms = 0UL; /// число единиц использования CPU для тарификации транзакции
      unsigned_int    delay_sec = 0UL; /// задержка транзакции в секундах, по умолчанию: 0

      EOSLIB_SERIALIZE( transaction_header, (expiration)(ref_block_num)(ref_block_prefix)(max_net_usage_words)(max_cpu_usage_ms)(delay_sec) )
   };

   /**
    *  Класс transaction содержит действия, context_free_actions и расширения транзакции
    *
    *  @ingroup transaction
    */
   class transaction : public transaction_header {
   public:

      /**
       * Создаёт новую транзакцию со сроком действия «сейчас + 60 с».
       */
      transaction(time_point_sec exp = time_point_sec(current_time_point()) + 60) : transaction_header( exp ) {}

      /**
       *  Отправляет эту транзакцию: упаковывает её и отправляет как отложенную (deferred)
       *
       *  @param sender_id — идентификатор отправителя
       *  @param payer — аккаунт, оплачивающий RAM
       *  @param replace_existing — по умолчанию false: при `0`/false, если указанный sender_id уже занят выполняющейся транзакцией этого контракта, сработает неуспешный assert. При `1` транзакция атомарно отменит/заменит текущую «в полёте»
       */
      void send(const uint128_t& sender_id, name payer, bool replace_existing = false) const {
         auto serialize = pack(*this);
         internal_use_do_not_use::send_deferred(sender_id, payer.value, serialize.data(), serialize.size(), replace_existing);
      }

      std::vector<action>  context_free_actions;
      std::vector<action>  actions;
      extensions_type      transaction_extensions;

      EOSLIB_SERIALIZE_DERIVED( transaction, transaction_header, (context_free_actions)(actions)(transaction_extensions) )
   };

   /**
    *  Структура onerror содержит идентификатор отправителя и упакованную транзакцию
    *
    *  @ingroup transaction
    */
   struct onerror {
      uint128_t          sender_id;
      std::vector<char> sent_trx;

     /**
      *  Распаковывает из текущего действия и возвращает структуру onerror
      *
      *  @ingroup transaction
      */
      static onerror from_current_action() {
         return unpack_action_data<onerror>();
      }

     /**
      * Распаковывает и возвращает транзакцию
      */
      transaction unpack_sent_trx() const {
         return unpack<transaction>(sent_trx);
      }

      EOSLIB_SERIALIZE( onerror, (sender_id)(sent_trx) )
   };

   /**
    *  Отправляет отложенную транзакцию
    *
    *  @ingroup transaction
    *  @param sender_id — имя аккаунта отправителя этой отложенной транзакции
    *  @param payer — имя аккаунта, отвечающего за оплату RAM для этой отложенной транзакции
    *  @param serialized_transaction — упакованная транзакция для отложенного выполнения
    *  @param size — размер упакованной транзакции, необходим для сохранения
    *  @param replace — если true, заменяет существующую транзакцию
    */
   inline void send_deferred(const uint128_t& sender_id, name payer, const char* serialized_transaction, size_t size, bool replace = false) {
     internal_use_do_not_use::send_deferred(sender_id, payer.value, serialized_transaction, size, replace);
   }
   /**
    *  Возвращает указанное действие из активной транзакции
    *
    *  @ingroup transaction
    *  @param type — 0 для действия без контекста (context free), 1 для обычного действия
    *  @param index — индекс запрашиваемого действия
    *  @return запрошенное действие
    */
   inline action get_action( uint32_t type, uint32_t index ) {
      constexpr size_t max_stack_buffer_size = 512;
      int s = internal_use_do_not_use::get_action( type, index, nullptr, 0 );
      eosio::check( s > 0, "get_action size failed" );
      size_t size = static_cast<size_t>(s);
      char* buffer = (char*)( max_stack_buffer_size < size ? malloc(size) : alloca(size) );
      auto size2 = internal_use_do_not_use::get_action( type, index, buffer, size );
      eosio::check( size == static_cast<size_t>(size2), "get_action failed" );
      return eosio::unpack<eosio::action>( buffer, size );
   }

   /**
    *  Читает текущую выполняемую транзакцию в буфер
    *
    *  @ingroup transaction
    *  @return число байт, записанных в ptr
    */
   inline size_t read_transaction(char* ptr, size_t sz) {
      return internal_use_do_not_use::read_transaction( ptr, sz );
   }

   /**
    *  Отменяет отложенную транзакцию
    *
    *  @ingroup transaction
    *  @param sender_id — идентификатор отправителя
    *
    *  @pre Идентификатор отложенной транзакции существует.
    *  @pre Отложенная транзакция с этим идентификатором ещё не опубликована.
    *  @post Отложенная транзакция отменена.
    *
    *  @return 1, если транзакция отменена; 0, если транзакция не найдена
    *
    *  Пример:
     *  @code
     *  id = 0xffffffffffffffff
     *  cancel_deferred( id );
     *  @endcode
     */
   inline int cancel_deferred(const uint128_t& sender_id) {
      return internal_use_do_not_use::cancel_deferred(sender_id);
   }

   /**
    *  Возвращает размер текущей выполняемой транзакции
    *
    *  @ingroup transaction
    *  @return размер текущей выполняемой транзакции (байт)
    */
   inline size_t transaction_size() {
      return internal_use_do_not_use::transaction_size();
   }

   /**
    *  Возвращает номер блока, используемый для TAPOS в текущей выполняемой транзакции.
    *
    *  @ingroup transaction
    *  @return номер блока TAPOS для текущей транзакции
    *  Пример:
    *  @code
    *  int tbn = tapos_block_num();
    *  @endcode
    */
   inline int tapos_block_num() {
      return internal_use_do_not_use::tapos_block_num();
   }

   /**
    *  Возвращает префикс блока, используемый для TAPOS в текущей выполняемой транзакции
    *
    *  @ingroup transaction
    *  @return префикс блока TAPOS для текущей транзакции
    *  Пример:
    *  @code
    *  int tbp = tapos_block_prefix();
    *  @endcode
    */
   inline int tapos_block_prefix() {
      return internal_use_do_not_use::tapos_block_prefix();
   }

   /**
    *  Возвращает срок действия (expiration) текущей выполняемой транзакции.
    *
    *  @ingroup transaction
    *  @brief Возвращает срок действия текущей выполняемой транзакции.
    *  @return срок действия текущей выполняемой транзакции в секундах с начала эпохи Unix
    */
   inline uint32_t expiration() {
      return internal_use_do_not_use::expiration();
   }

   /**
    *  Извлекает signed_transaction.context_free_data[index]
    *
    *  @ingroup transaction
    *  @param index — индекс записи context_free_data
    *  @param buff — выходной буфер для данных context_free_data
    *  @param size — сколько байт context_free_data[index] поместить в buff; 0 — вернуть требуемый размер
    *  @return число скопированных байт, либо context_free_data[index].size() при size == 0, либо -1 при неверном index
    */
   inline int get_context_free_data( uint32_t index, char* buff, size_t size ) {
      return internal_use_do_not_use::get_context_free_data(index, buff, size);
   }
}
