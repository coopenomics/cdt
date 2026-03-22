/**
 *  @file
 *  @copyright см. eos/LICENSE
 */
#pragma once

#include "check.hpp"
#include "serialize.hpp"
#include "reflect.hpp"

#include <string>
#include <string_view>

namespace eosio {
   namespace internal_use_do_not_use {
      extern "C" {
         __attribute__((eosio_wasm_import))
         void printn(uint64_t);
      }
   }

   /**
    * @defgroup name Имя
    * @ingroup core
    * @ingroup types
    * @brief Тип имени COOPOS
    */

   /**
    * Обёртка над %uint64_t для имён аккаунтов и контрактов в COOPOS: значение передаётся только там,
    * где ожидается %name, без арифметики над «именем». Позволяет специализировать печать.
    *
    * @ingroup name
    */
   struct name {
   public:
      enum class raw : uint64_t {};

      /**
       * Конструктор по умолчанию; имя с нулевым значением
       *
       * @brief Конструктор name со значением по умолчанию 0
       *
       */
      constexpr name() : value(0) {}

      /**
       * Создаёт name из uint64_t
       *
       * @brief Конструктор name; инициализация поля value значением v
       * @param v — значение uint64_t
       *
       */
      constexpr explicit name( uint64_t v )
      :value(v)
      {}

      /**
       * Создаёт name из типа name::raw
       *
       * @brief Конструктор name; инициализация поля value значением r
       * @param r — сырое значение (strong typedef к uint64_t)
       *
       */
      constexpr explicit name( name::raw r )
      :value(static_cast<uint64_t>(r))
      {}

      /**
       * Создаёт name из строки (правила кодирования имён COOPOS)
       *
       * @brief Конструктор name; инициализация поля value из строки str
       * @param str — строка; после проверки преобразуется в uint64_t
       *
       */
      constexpr explicit name( std::string_view str )
      :value(0)
      {
         if( str.size() > 13 ) {
            eosio::check( false, "string is too long to be a valid name" );
         }
         if( str.empty() ) {
            return;
         }

         auto n = std::min( (uint32_t)str.size(), (uint32_t)12u );
         for( decltype(n) i = 0; i < n; ++i ) {
            value <<= 5;
            value |= char_to_value( str[i] );
         }
         value <<= ( 4 + 5*(12 - n) );
         if( str.size() == 13 ) {
            uint64_t v = char_to_value( str[12] );
            if( v > 0x0Full ) {
               eosio::check(false, "thirteenth character in name cannot be a letter that comes after j");
            }
            value |= v;
         }
      }

      /**
       *  Преобразует символ алфавита имён (Base32-подобный) в числовое значение
       *
       *  @param c — символ
       *  @return числовое значение символа
       */
      static constexpr uint8_t char_to_value( char c ) {
         if( c == '.')
            return 0;
         else if( c >= '1' && c <= '5' )
            return (c - '1') + 1;
         else if( c >= 'a' && c <= 'z' )
            return (c - 'a') + 6;
         else
            eosio::check( false, "character is not in allowed character set for names" );

         return 0; // control flow will never reach here; just added to suppress warning
      }

      /**
       *  Длина имени в символах
       */
      constexpr uint8_t length()const {
         constexpr uint64_t mask = 0xF800000000000000ull;

         if( value == 0 )
            return 0;

         uint8_t l = 0;
         uint8_t i = 0;
         for( auto v = value; i < 13; ++i, v <<= 5 ) {
            if( (v & mask) > 0 ) {
               l = i;
            }
         }

         return l + 1;
      }

      /**
       *  Суффикс имени после последней значащей точки
       */
      constexpr name suffix()const {
         uint32_t remaining_bits_after_last_actual_dot = 0;
         uint32_t tmp = 0;
         for( int32_t remaining_bits = 59; remaining_bits >= 4; remaining_bits -= 5 ) { // Note: remaining_bits must remain signed integer
            // Get characters one-by-one in name in order from left to right (not including the 13th character)
            auto c = (value >> remaining_bits) & 0x1Full;
            if( !c ) { // if this character is a dot
               tmp = static_cast<uint32_t>(remaining_bits);
            } else { // if this character is not a dot
               remaining_bits_after_last_actual_dot = tmp;
            }
         }

         uint64_t thirteenth_character = value & 0x0Full;
         if( thirteenth_character ) { // if 13th character is not a dot
            remaining_bits_after_last_actual_dot = tmp;
         }

         if( remaining_bits_after_last_actual_dot == 0 ) // there is no actual dot in the %name other than potentially leading dots
            return name{value};

         // At this point remaining_bits_after_last_actual_dot has to be within the range of 4 to 59 (and restricted to increments of 5).

         // Mask for remaining bits corresponding to characters after last actual dot, except for 4 least significant bits (corresponds to 13th character).
         uint64_t mask = (1ull << remaining_bits_after_last_actual_dot) - 16;
         uint32_t shift = 64 - remaining_bits_after_last_actual_dot;

         return name{ ((value & mask) << shift) + (thirteenth_character << (shift-1)) };
      }

      /**
       *  Префикс имени до последней значащей точки
       */
      constexpr name prefix() const {
         uint64_t result = value;
         bool not_dot_character_seen = false;
         uint64_t mask = 0xFull;

         // Get characters one-by-one in name in order from right to left
         for( int32_t offset = 0; offset <= 59; ) {
            auto c = (value >> offset) & mask;

            if( !c ) { // if this character is a dot
               if(not_dot_character_seen) { // we found the rightmost dot character
                  result = (value >> offset) << offset;
                  break;
               }
            } else {
               not_dot_character_seen = true;
            }

            if (offset == 0) {
               offset += 4;
               mask = 0x1Full;
            } else {
               offset += 5;
            }
         }

         return name{ result };
      }

      /**
       * Приведение к name::raw
       *
       * @return значение в виде raw
       */
      constexpr operator raw()const { return raw(value); }

      /**
       * Явное приведение к bool по полю value
       *
       * @return true — если value ≠ 0; иначе false
       */
      constexpr explicit operator bool()const { return value != 0; }

      /**
       *  Записывает строковое представление %name в буфер char
       *
       *  @pre диапазон [begin, end) — допустимая область памяти для записи
       *  @param begin — начало буфера
       *  @param end — конец буфера (не включая end)
       *  @param dry_run — если true, ничего не записывать
       *  @return char* — указатель сразу за последним символом, который был бы записан (при достаточном буфере и dry_run == false; смысл только если возврат ≥ begin)
       *  @post при успехе и dry_run == false в [begin, возврат) — строка имени
       */
      char* write_as_string( char* begin, char* end, bool dry_run = false )const {
         static const char* charmap = ".12345abcdefghijklmnopqrstuvwxyz";
         constexpr uint64_t mask = 0xF800000000000000ull;

         if( dry_run || (begin + 13 < begin) || (begin + 13 > end) ) {
            char* actual_end = begin + length();
            if( dry_run || (actual_end < begin) || (actual_end > end) ) return actual_end;
         }

         auto v = value;
         for( auto i = 0; i < 13; ++i, v <<= 5 ) {
            if( v == 0 ) return begin;

            auto indx = (v & mask) >> (i == 12 ? 60 : 59);
            *begin = charmap[indx];
            ++begin;
         }

         return begin;
      }

      /**
       *  Строковое представление имени через write_as_string()
       *
       *  @brief Возвращает имя как строку через write_as_string() и буфер, заполненный этой функцией
       */
      std::string to_string()const {
         char buffer[13];
         auto end = write_as_string( buffer, buffer + sizeof(buffer) );
         return {buffer, end};
      }

      /**
       * Печать имени в лог контракта COOPOS (нативный вывод)
       *
       */
      inline void print()const {
        internal_use_do_not_use::printn(value);
      }

      /// @cond INTERNAL

      /**
       * Оператор равенства
       *
       * @return true — значения совпадают
       */
      friend constexpr bool operator == ( const name& a, const name& b ) {
         return a.value == b.value;
      }

      /**
       * Оператор неравенства
       *
       * @return true — значения различаются
       */
      friend constexpr bool operator != ( const name& a, const name& b ) {
         return a.value != b.value;
      }

      /**
       * Оператор «меньше» по uint64_t
       *
       * @return true — a меньше b
       */
      friend constexpr bool operator < ( const name& a, const name& b ) {
         return a.value < b.value;
      }

      /// @endcond

      uint64_t value = 0;

      CDT_REFLECT(value);
      EOSLIB_SERIALIZE( name, (value) )
   };

   namespace detail {
      template <char... Str>
      struct to_const_char_arr {
         static constexpr const char value[] = {Str...};
      };
   } /// namespace detail
} /// namespace eosio

/**
 * @ingroup name
 * @brief Литерал "foo"_n — сокращение для name("foo")
 */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-string-literal-operator-template"
template <typename T, T... Str>
inline constexpr eosio::name operator""_n() {
   constexpr auto x = eosio::name{std::string_view{eosio::detail::to_const_char_arr<Str...>::value, sizeof...(Str)}};
   return x;
}
#pragma clang diagnostic pop
