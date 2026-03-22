#pragma once
#include "multi_index.hpp"
#include "system.hpp"

namespace  eosio {

   /**
    *  @defgroup singleton Singleton-таблица
    *  @ingroup contracts
    *  @brief Определяет singleton-таблицу COOPOS для использования с %multiindex
    */

   /**
    *  Обёртка использует одну таблицу для хранения именованных объектов различных типов.
    *
    *  @ingroup singleton
    *  @tparam SingletonName — имя этой singleton-переменной
    *  @tparam T — тип singleton
    */
   template<name::raw SingletonName, typename T>
   class singleton
   {
      /**
       * Первичный ключ данных в singleton-таблице
       */
      constexpr static uint64_t pk_value = static_cast<uint64_t>(SingletonName);

      /**
       * Структура данных в singleton-таблице
       */
      struct row {
         /**
          * Значение, хранимое в singleton-таблице
          */
         T value;

         /**
          * Возвращает первичный ключ данных
          *
          * @return uint64_t — первичный ключ
          */
         uint64_t primary_key() const { return pk_value; }

         EOSLIB_SERIALIZE( row, (value) )
      };

      typedef eosio::multi_index<SingletonName, row> table;

      public:

         /**
          * Создаёт объект singleton по владельцу таблицы и области видимости (scope)
          *
          * @param code — владелец таблицы
          * @param scope — scope таблицы
          */
         singleton( name code, uint64_t scope ) : _t( code, scope ) {}

         /**
          *  Проверяет существование singleton-таблицы
          *
          * @return true — если существует
          * @return false — иначе
          */
         bool exists() {
            return _t.find( pk_value ) != _t.end();
         }

         /**
          * Возвращает значение из singleton-таблицы. Выбрасывает исключение, если записи нет
          *
          * @brief Возвращает значение, хранящееся в singleton-таблице
          * @return T — сохранённое значение
          */
         T get() {
            auto itr = _t.find( pk_value );
            eosio::check( itr != _t.end(), "singleton does not exist" );
            return itr->value;
         }

         /**
          * Возвращает значение из singleton-таблицы. Если записи нет — указанное значение по умолчанию
          *
          * @param def — значение по умолчанию, если данных нет
          * @return T — сохранённое значение
          */
         T get_or_default( const T& def = T() ) {
            auto itr = _t.find( pk_value );
            return itr != _t.end() ? itr->value : def;
         }

         /**
          * Возвращает значение из singleton-таблицы. Если записи нет — создаёт её с указанным значением по умолчанию
          *
          * @param bill_to_account — аккаунт, с которого списывается оплата за новые данные, если записи не было
          * @param def — значение по умолчанию при создании записи
          * @return T — сохранённое значение
          */
         T get_or_create( name bill_to_account, const T& def = T() ) {
            auto itr = _t.find( pk_value );
            return itr != _t.end() ? itr->value
               : _t.emplace(bill_to_account, [&](row& r) { r.value = def; })->value;
         }

         /**
          * Записывает новое значение в singleton-таблицу
          *
          * @param value — новое значение
          * @param bill_to_account — аккаунт, с которого списывается оплата за запись
          */
         void set( const T& value, name bill_to_account ) {
            auto itr = _t.find( pk_value );
            if( itr != _t.end() ) {
               _t.modify(itr, bill_to_account, [&](row& r) { r.value = value; });
            } else {
               _t.emplace(bill_to_account, [&](row& r) { r.value = value; });
            }
         }

         /**
          * Удаляет единственную запись в singleton-таблице
          */
         void remove( ) {
            auto itr = _t.find( pk_value );
            if( itr != _t.end() ) {
               _t.erase(itr);
            }
         }

      private:
         table _t;
   };
} /// namespace eosio
