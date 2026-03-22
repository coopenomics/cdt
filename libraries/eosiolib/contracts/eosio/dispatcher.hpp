#pragma once
#include "action.hpp"

#include <bluegrass/meta/preprocessor.hpp>
#include <tuple>

namespace eosio {

  /**
   * @defgroup dispatcher Диспетчер (Dispatcher)
   * @ingroup contracts
   * @brief Определяет функции C++ для маршрутизации действия к соответствующему обработчику внутри контракта
   */


   /// @cond IMPLEMENTATIONS

   template<typename Contract, typename FirstAction>
   bool dispatch( uint64_t code, uint64_t act ) {
      if( code == FirstAction::get_account() && FirstAction::get_name() == act ) {
         Contract().on( unpack_action_data<FirstAction>() );
         return true;
      }
      return false;
   }

   /// @endcond

   /**
    * Динамически направляет входящие действия к обработчикам вида
    *
    * ```
    * static Contract::on( ActionType )
    * ```
    *
    * Контракт, принимающий действия, должен наследовать eosio::contract
    *
    * @ingroup dispatcher
    *
    */
   template<typename Contract, typename FirstAction, typename SecondAction, typename... Actions>
   bool dispatch( uint64_t code, uint64_t act ) {
      if( code == FirstAction::get_account() && FirstAction::get_name() == act ) {
         Contract().on( unpack_action_data<FirstAction>() );
         return true;
      }
      return eosio::dispatch<Contract,SecondAction,Actions...>( code, act );
   }




   /**
    * Распаковывает полученное действие и вызывает соответствующий обработчик
    *
    * @ingroup dispatcher
    * @tparam T — класс контракта с обработчиком действия; должен наследовать eosio::contract
    * @tparam Args — типы аргументов обработчика (поля действия)
    * @param self — имя, передаваемое в конструктор экземпляра контракта
    * @param code — код, передаваемый в конструктор экземпляра контракта
    * @param func — указатель на метод-обработчик действия
    * @return true — если действие распаковано и обработчик вызван
    */
   template<typename T, typename... Args>
   bool execute_action( name self, name code, void (T::*func)(Args...)  ) {
      size_t size = action_data_size();

      //using malloc/free here potentially is not exception-safe, although WASM doesn't support exceptions
      constexpr size_t max_stack_buffer_size = 512;
      void* buffer = nullptr;
      if( size > 0 ) {
         buffer = max_stack_buffer_size < size ? malloc(size) : alloca(size);
         read_action_data( buffer, size );
      }

      std::tuple<std::decay_t<Args>...> args;
      datastream<const char*> ds((char*)buffer, size);
      ds >> args;

      T inst(self, code, ds);

      auto f2 = [&]( auto... a ){
         ((&inst)->*func)( a... );
      };

      std::apply( f2, args );
      if ( max_stack_buffer_size < size ) {
         free(buffer);
      }
      return true;
   }

  /// @cond INTERNAL

 // Helper macro for EOSIO_DISPATCH_INTERNAL
 #define EOSIO_DISPATCH_INTERNAL( OP, elem ) \
    case eosio::name( BLUEGRASS_META_STRINGIZE(elem) ).value: \
       eosio::execute_action( eosio::name(receiver), eosio::name(code), &OP::elem ); \
       break;

 // Helper macro for EOSIO_DISPATCH
 #define EOSIO_DISPATCH_HELPER( TYPE,  MEMBERS ) \
    BLUEGRASS_META_FOREACH_SEQ( EOSIO_DISPATCH_INTERNAL, TYPE, MEMBERS )

/// @endcond

/**
 * Удобный макрос для создания обработчика apply контракта
 *
 * @ingroup dispatcher
 * @note Чтобы использовать макрос, класс контракта должен наследовать eosio::contract
 * @param TYPE — имя класса контракта
 * @param MEMBERS — последовательность поддерживаемых действий контракта
 *
 * Пример:
 * @code
 * EOSIO_DISPATCH( eosio::bios, (setpriv)(setalimits)(setglimits)(setprods)(reqauth) )
 * @endcode
 */
#define EOSIO_DISPATCH( TYPE, MEMBERS ) \
extern "C" { \
   [[eosio::wasm_entry]] \
   void apply( uint64_t receiver, uint64_t code, uint64_t action ) { \
      if( code == receiver ) { \
         switch( action ) { \
            EOSIO_DISPATCH_HELPER( TYPE, MEMBERS ) \
         } \
         /* does not allow destructor of thiscontract to run: eosio_exit(0); */ \
      } \
   } \
} \

}
