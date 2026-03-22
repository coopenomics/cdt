/**
 *  @file
 *  @copyright см. eos/LICENSE
 */
#pragma once

#include "check.hpp"
#include "name.hpp"
#include "serialize.hpp"
#include "print.hpp"
#include "datastream.hpp"

#include <tuple>
#include <limits>
#include <string_view>

namespace eosio {
  /**
   *  @defgroup symbol Символ
   *  @ingroup core
   *  @brief Определяет C++ API для работы с символами
   */

   /**
    *  Код символа токена, упакованный в uint64_t
    *
    *  @ingroup symbol
    */
   class symbol_code {
   public:

      /**
       * Конструктор по умолчанию; создаёт symbol_code со значением 0
       *
       * @brief Конструктор symbol_code со значением по умолчанию 0
       *
       */
      constexpr symbol_code() : value(0) {}

      /**
       * Создаёт symbol_code из сырого значения uint64_t.
       *
       * @brief Конструктор symbol_code; инициализация поля value из raw
       * @param raw — сырое значение (uint64_t)
       *
       */
      constexpr explicit symbol_code( uint64_t raw )
      :value(raw)
      {}

      /**
       * Создаёт symbol_code из строки (до 7 заглавных букв A–Z).
       *
       * @brief Конструктор symbol_code; инициализация поля value из строки str
       * @param str — строка; после проверки преобразуется в uint64_t
       *
       */
      constexpr explicit symbol_code( std::string_view str )
      :value(0)
      {
         if( str.size() > 7 ) {
            eosio::check( false, "string is too long to be a valid symbol_code" );
         }
         for( auto itr = str.rbegin(); itr != str.rend(); ++itr ) {
            if( *itr < 'A' || *itr > 'Z') {
               eosio::check( false, "only uppercase letters allowed in symbol_code string" );
            }
            value <<= 8;
            value |= *itr;
         }
      }

      /**
       * Проверяет корректность кода символа
       * @return true — символ допустим
       */
      constexpr bool is_valid()const {
         auto sym = value;
         for ( int i=0; i < 7; i++ ) {
            char c = (char)(sym & 0xFF);
            if ( !('A' <= c && c <= 'Z') ) return false;
            sym >>= 8;
            if ( !(sym & 0xFF) ) {
               do {
                  sym >>= 8;
                  if ( (sym & 0xFF) ) return false;
                  i++;
               } while( i < 7 );
            }
         }
         return true;
      }

      /**
       * Возвращает длину кода символа в символах
       *
       * @return length — число значащих символов
       */
      constexpr uint32_t length()const {
         auto sym = value;
         uint32_t len = 0;
         while (sym & 0xFF && len <= 7) {
            len++;
            sym >>= 8;
         }
         return len;
      }

      /**
       * Возвращает сырое значение uint64_t
       *
       * @return значение поля value
       */
      constexpr uint64_t raw()const { return value; }

      /**
       * Явное приведение к bool
       *
       * @return true — если value не равен 0; иначе false
       */
      constexpr explicit operator bool()const { return value != 0; }

      /**
       *  Записывает symbol_code в виде строки в буфер char
       *
       *
       *  @brief Записывает symbol_code в виде строки в переданный буфер char
       *  @pre is_valid() == true
       *  @pre диапазон [begin, end) — допустимая область памяти для записи
       *  @param begin — начало буфера
       *  @param end — конец буфера (не включая end)
       *  @param dry_run — если true, ничего не записывать
       *  @return char* — указатель сразу за последним записанным символом (при dry_run == false и достаточном буфере; смысл только если возврат ≥ begin)
       *  @post при успехе и dry_run == false в [begin, возврат) — строка; иначе запись может не выполняться
       */
      char* write_as_string( char* begin, char* end, bool dry_run = false )const {
         constexpr uint64_t mask = 0xFFull;

         if( dry_run || (begin + 7 < begin) || (begin + 7 > end) ) {
            char* actual_end = begin + length();
            if( dry_run || (actual_end < begin) || (actual_end > end) ) return actual_end;
         }

         auto v = value;
         for( auto i = 0; i < 7; ++i, v >>= 8 ) {
            if( v == 0 ) return begin;

            *begin = static_cast<char>(v & mask);
            ++begin;
         }

         return begin;
      }

      /**
       *  Строковое представление кода символа через write_as_string()
       */
      std::string to_string()const {
         char buffer[7];
         auto end = write_as_string( buffer, buffer + sizeof(buffer) );
         return {buffer, end};
      }

      /**
       * Печать symbol_code в лог контракта COOPOS
       *
       */
      inline void print()const {
         char buffer[7];
         auto end = write_as_string( buffer, buffer + sizeof(buffer) );
         if( buffer < end )
           printl( buffer, (end-buffer) );
      }

      /**
       * Оператор равенства
       *
       * @return true — коды совпадают
       */
      friend constexpr bool operator == ( const symbol_code& a, const symbol_code& b ) {
         return a.value == b.value;
      }

      /**
       * Оператор неравенства
       *
       * @return true — коды различаются
       */
      friend constexpr bool operator != ( const symbol_code& a, const symbol_code& b ) {
         return a.value != b.value;
      }

      /**
       * Оператор «меньше»
       * @brief Оператор «меньше»
       * @return true — значение a меньше b
       */
      friend constexpr bool operator < ( const symbol_code& a, const symbol_code& b ) {
         return a.value < b.value;
      }

   private:
      uint64_t value = 0;
   };

   /**
    *  Сериализация symbol_code в поток
    *
    *  @param ds — поток записи
    *  @param sym_code — значение для сериализации
    *  @tparam DataStream — тип буфера потока данных
    *  @return DataStream& — ссылка на поток
    */
   template<typename DataStream>
   inline DataStream& operator<<(DataStream& ds, const eosio::symbol_code sym_code) {
     uint64_t raw = sym_code.raw();
     ds.write( (const char*)&raw, sizeof(raw));
     return ds;
   }

   /**
    *  Десериализация symbol_code из потока
    *
    *  @param ds — поток чтения
    *  @param sym_code — приёмник значения
    *  @tparam DataStream — тип буфера потока данных
    *  @return DataStream& — ссылка на поток
    */
   template<typename DataStream>
   inline DataStream& operator>>(DataStream& ds, eosio::symbol_code& sym_code) {
     uint64_t raw = 0;
     ds.read((char*)&raw, sizeof(raw));
     sym_code = symbol_code(raw);
     return ds;
   }

   /**
    *  Символ токена: код (до 7 символов) и точность (число десятичных знаков).
    *
    *  @ingroup symbol
    */
   class symbol {
   public:
      /**
       * Конструктор по умолчанию; значение 0
       */
      constexpr symbol() : value(0) {}

      /**
       * Создаёт symbol из упакованного uint64_t (код и точность).
       *
       * @param raw — сырое значение uint64_t
       */
      constexpr explicit symbol( uint64_t raw ) : value(raw) {}

      /**
       * Создаёт symbol по коду символа и точности.
       *
       * @param sc — код символа
       * @param precision — число десятичных знаков
       */
      constexpr symbol( symbol_code sc, uint8_t precision )
      : value( (sc.raw() << 8) | static_cast<uint64_t>(precision) )
      {}

      /**
       * Создаёт symbol по строке кода и точности.
       *
       * @param ss — строка с кодом символа
       * @param precision — число десятичных знаков
       */
      constexpr symbol( std::string_view ss, uint8_t precision )
      : value( (symbol_code(ss).raw() << 8)  | static_cast<uint64_t>(precision) )
      {}

      /**
       * Допустим ли символ
       */
      constexpr bool is_valid()const                 { return code().is_valid(); }

      /**
       * Точность символа (десятичные знаки)
       */
      constexpr uint8_t precision()const             { return static_cast<uint8_t>( value & 0xFFull ); }

      /**
       * Код символа (symbol_code)
       */
      constexpr symbol_code code()const              { return symbol_code{value >> 8};   }

      /**
       * Упакованное представление symbol в uint64_t
       */
      constexpr uint64_t raw()const                  { return value; }

      constexpr explicit operator bool()const { return value != 0; }

      /**
       * Печать символа в лог контракта COOPOS
       */
      void print( bool show_precision = true )const {
         if( show_precision ){
            ::eosio::print( static_cast<uint64_t>(precision()), "," );
         }
         char buffer[7];
         auto end = code().write_as_string( buffer, buffer + sizeof(buffer) );
         if( buffer < end )
            printl( buffer, (end-buffer) );
      }

      /**
       * Оператор равенства
       *
       * @return true — символы совпадают
       */
      friend constexpr bool operator == ( const symbol& a, const symbol& b ) {
         return a.value == b.value;
      }

      /**
       * Оператор неравенства
       *
       * @return true — символы различаются
       */
      friend constexpr bool operator != ( const symbol& a, const symbol& b ) {
         return a.value != b.value;
      }

      /**
       * Оператор «меньше»
       * @brief Оператор «меньше»
       * @return true — значение a меньше b
       */
      friend constexpr bool operator < ( const symbol& a, const symbol& b ) {
         return a.value < b.value;
      }

   private:
      uint64_t value = 0;
   };

   /**
    *  Сериализация symbol в поток
    *
    *  @brief Сериализация symbol
    *  @param ds — поток записи
    *  @param sym — значение для сериализации
    *  @tparam DataStream — тип буфера потока данных
    *  @return DataStream& — ссылка на поток
    */
   template<typename DataStream>
   inline DataStream& operator<<(DataStream& ds, const eosio::symbol sym) {
     uint64_t raw = sym.raw();
     ds.write( (const char*)&raw, sizeof(raw));
     return ds;
   }

   /**
    *  Десериализация symbol из потока
    *
    *  @brief Десериализация symbol
    *  @param ds — поток чтения
    *  @param sym — приёмник значения
    *  @tparam DataStream — тип буфера потока данных
    *  @return DataStream& — ссылка на поток
    */
   template<typename DataStream>
   inline DataStream& operator>>(DataStream& ds, eosio::symbol& sym) {
     uint64_t raw = 0;
     ds.read((char*)&raw, sizeof(raw));
     sym = symbol(raw);
     return ds;
   }

   /**
    *  Расширенный символ: символ токена и контракт-эмитент в сети COOPOS
    *
    *  @ingroup symbol
    */
   class extended_symbol
   {
   public:

      /**
       * Конструктор по умолчанию
       */
      constexpr extended_symbol() {}

      /**
       * Создаёт extended_symbol по символу и имени контракта
       *
       * @param sym — символ токена
       * @param con — имя контракта (эмитента)
       */
      constexpr extended_symbol( symbol s, name con ) : sym(s), contract(con) {}

      /**
       * Символ без привязки к контракту
       *
       * @return symbol
       */
      constexpr symbol get_symbol() const { return sym; }

      /**
       * Имя контракта-эмитента
       *
       * @return name
       */
      constexpr name  get_contract() const { return contract; }

      /**
       * Печать расширенного символа в лог контракта COOPOS
       *
       * @brief Вывод расширенного символа
       */
      void print( bool show_precision = true )const {
         sym.print( show_precision );
         ::eosio::print("@", contract);
      }

      /**
       * Оператор равенства
       *
       * @return true — пары (символ, контракт) совпадают
       */
      friend constexpr bool operator == ( const extended_symbol& a, const extended_symbol& b ) {
        return std::tie( a.sym, a.contract ) == std::tie( b.sym, b.contract );
      }

      /**
       * Оператор неравенства
       *
       * @return true — значения различаются
       */
      friend constexpr bool operator != ( const extended_symbol& a, const extended_symbol& b ) {
        return std::tie( a.sym, a.contract ) != std::tie( b.sym, b.contract );
      }

      /**
       * Оператор «меньше» (лексикографически по паре символ, контракт)
       *
       * @return true — a меньше b
       */
      friend constexpr bool operator < ( const extended_symbol& a, const extended_symbol& b ) {
        return std::tie( a.sym, a.contract ) < std::tie( b.sym, b.contract );
      }

   private:
      symbol sym; ///< символ токена
      name   contract; ///< контракт, выпускающий токен

      EOSLIB_SERIALIZE( extended_symbol, (sym)(contract) )
   };
}
