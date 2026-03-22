#pragma once

#include "serialize.hpp"
#include "print.hpp"
#include "check.hpp"
#include "symbol.hpp"

#include <tuple>
#include <limits>

namespace eosio {

   char* write_decimal( char* begin, char* end, bool dry_run, uint64_t number, uint8_t num_decimal_places, bool negative );

   /**
    *  @defgroup asset Актив
    *  @ingroup core
    *  @brief Определяет C++ API для работы с активами
    */

   /**
    *  Хранит данные актива (количество и символ) для владельца.
    *
    *  @ingroup asset
    */
   struct asset {
      /**
       * Количество актива
       */
      int64_t      amount = 0;

      /**
       * Символ актива
       */
      symbol  symbol;

      /**
       * Максимально допустимое значение количества для этого актива; ограничено 2^62 − 1
       */
      static constexpr int64_t max_amount    = (1LL << 62) - 1;

      asset() {}

      /**
       * Создаёт новый актив по символу и количеству
       *
       * @param a — количество актива
       * @param s — символ
       */
      asset( int64_t a, class symbol s )
      :amount(a),symbol{s}
      {
         eosio::check( is_amount_within_range(), "magnitude of asset amount must be less than 2^62" );
         eosio::check( symbol.is_valid(),        "invalid symbol name" );
      }

      /**
       * Проверяет, что количество не выходит за пределы максимума
       *
       * @return true — количество в допустимом диапазоне
       * @return false — иначе
       */
      bool is_amount_within_range()const { return -max_amount <= amount && amount <= max_amount; }

      /**
       * Проверяет корректность актива. Корректный актив: количество ≤ max_amount и допустимый символ
       *
       * @return true — актив корректен
       * @return false — иначе
       */
      bool is_valid()const               { return is_amount_within_range() && symbol.is_valid(); }

      /**
       * Задаёт количество актива
       *
       * @param a — новое количество
       */
      void set_amount( int64_t a ) {
         amount = a;
         eosio::check( is_amount_within_range(), "magnitude of asset amount must be less than 2^62" );
      }

      /// @cond OPERATORS

      /**
       * Унарный минус
       *
       * @return asset — новый актив с противоположным по знаку количеством
       */
      asset operator-()const {
         asset r = *this;
         r.amount = -r.amount;
         return r;
      }

      /**
       * Оператор вычитания с присваиванием
       *
       * @param a — вычитаемый актив
       * @return asset& — ссылка на этот актив
       * @post из количества этого актива вычитается количество a
       */
      asset& operator-=( const asset& a ) {
         eosio::check( a.symbol == symbol, "attempt to subtract asset with different symbol" );
         amount -= a.amount;
         eosio::check( -max_amount <= amount, "subtraction underflow" );
         eosio::check( amount <= max_amount,  "subtraction overflow" );
         return *this;
      }

      /**
       * Оператор сложения с присваиванием
       *
       * @param a — прибавляемый актив
       * @return asset& — ссылка на этот актив
       * @post к количеству этого актива прибавляется количество a
       */
      asset& operator+=( const asset& a ) {
         eosio::check( a.symbol == symbol, "attempt to add asset with different symbol" );
         amount += a.amount;
         eosio::check( -max_amount <= amount, "addition underflow" );
         eosio::check( amount <= max_amount,  "addition overflow" );
         return *this;
      }

      /**
       * Оператор сложения
       *
       * @param a — первый слагаемый актив
       * @param b — второй слагаемый актив
       * @return asset — сумма
       */
      inline friend asset operator+( const asset& a, const asset& b ) {
         asset result = a;
         result += b;
         return result;
      }

      /**
       * Оператор вычитания
       *
       * @param a — уменьшаемый актив
       * @param b — вычитаемый актив
       * @return asset — разность a − b
       */
      inline friend asset operator-( const asset& a, const asset& b ) {
         asset result = a;
         result -= b;
         return result;
      }

      /**
       * Умножение количества на число с присваиванием
       *
       * @details Умножает количество этого актива на число и записывает результат обратно в актив.
       * @param a — множитель
       * @return asset& — ссылка на этот актив
       * @post количество умножено на a
       */
      asset& operator*=( int64_t a ) {
         int128_t tmp = (int128_t)amount * (int128_t)a;
         eosio::check( tmp <= max_amount, "multiplication overflow" );
         eosio::check( tmp >= -max_amount, "multiplication underflow" );
         amount = (int64_t)tmp;
         return *this;
      }

      /**
       * Оператор умножения; множитель справа (asset × int64_t)
       *
       * @brief Оператор умножения; множитель справа (asset × int64_t)
       * @param a — умножаемый актив
       * @param b — множитель (целое)
       * @return asset — произведение
       */
      friend asset operator*( const asset& a, int64_t b ) {
         asset result = a;
         result *= b;
         return result;
      }


      /**
       * Оператор умножения; множитель слева (int64_t × asset)
       *
       * @param a — множитель (целое)
       * @param b — умножаемый актив
       * @return asset — произведение
       */
      friend asset operator*( int64_t b, const asset& a ) {
         asset result = a;
         result *= b;
         return result;
      }

      /**
       * @brief Оператор деления количества на число с присваиванием
       *
       * @details Делит количество этого актива на число и записывает результат обратно в актив.
       * @param a — делитель
       * @return asset& — ссылка на этот актив
       * @post количество разделено на a
       */
      asset& operator/=( int64_t a ) {
         eosio::check( a != 0, "divide by zero" );
         eosio::check( !(amount == std::numeric_limits<int64_t>::min() && a == -1), "signed division overflow" );
         amount /= a;
         return *this;
      }

      /**
       * Оператор деления актива на число
       *
       * @param a — делимый актив
       * @param b — делитель (целое)
       * @return asset — частное
       */
      friend asset operator/( const asset& a, int64_t b ) {
         asset result = a;
         result /= b;
         return result;
      }

      /**
       * Оператор деления одного актива на другой (по количествам)
       *
       * @param a — актив, количество которого — делимое
       * @param b — актив, количество которого — делитель
       * @return int64_t — целочисленное частное двух количеств (a.amount / b.amount)
       * @pre у обоих активов один и тот же символ
       */
      friend int64_t operator/( const asset& a, const asset& b ) {
         eosio::check( b.amount != 0, "divide by zero" );
         eosio::check( a.symbol == b.symbol, "attempt to divide assets with different symbol" );
         return a.amount / b.amount;
      }

      /**
       * Оператор равенства
       *
       * @param a — первый актив
       * @param b — второй актив
       * @return true — количества равны
       * @return false — иначе
       * @pre у обоих активов один и тот же символ
       */
      friend bool operator==( const asset& a, const asset& b ) {
         eosio::check( a.symbol == b.symbol, "comparison of assets with different symbols is not allowed" );
         return a.amount == b.amount;
      }

      /**
       * Оператор неравенства
       *
       * @param a — первый актив
       * @param b — второй актив
       * @return true — количества различаются
       * @return false — иначе
       * @pre у обоих активов один и тот же символ
       */
      friend bool operator!=( const asset& a, const asset& b ) {
         return !( a == b);
      }

      /**
       * Оператор «меньше»
       *
       * @param a — первый актив
       * @param b — второй актив
       * @return true — количество a меньше количества b
       * @return false — иначе
       * @pre у обоих активов один и тот же символ
       */
      friend bool operator<( const asset& a, const asset& b ) {
         eosio::check( a.symbol == b.symbol, "comparison of assets with different symbols is not allowed" );
         return a.amount < b.amount;
      }

      /**
       * Оператор «меньше или равно»
       *
       * @param a — первый актив
       * @param b — второй актив
       * @return true — количество a не больше количества b
       * @return false — иначе
       * @pre у обоих активов один и тот же символ
       */
      friend bool operator<=( const asset& a, const asset& b ) {
         eosio::check( a.symbol == b.symbol, "comparison of assets with different symbols is not allowed" );
         return a.amount <= b.amount;
      }

      /**
       * Оператор «больше»
       *
       * @param a — первый актив
       * @param b — второй актив
       * @return true — количество a больше количества b
       * @return false — иначе
       * @pre у обоих активов один и тот же символ
       */
      friend bool operator>( const asset& a, const asset& b ) {
         eosio::check( a.symbol == b.symbol, "comparison of assets with different symbols is not allowed" );
         return a.amount > b.amount;
      }

      /**
       * Оператор «больше или равно»
       *
       * @param a — первый актив
       * @param b — второй актив
       * @return true — количество a не меньше количества b
       * @return false — иначе
       * @pre у обоих активов один и тот же символ
       */
      friend bool operator>=( const asset& a, const asset& b ) {
         eosio::check( a.symbol == b.symbol, "comparison of assets with different symbols is not allowed" );
         return a.amount >= b.amount;
      }

      /// @endcond

      /**
       *  Записывает представление актива в виде строки в буфер char
       *
       *  @brief Записывает asset в виде строки в переданный буфер char
       *  @pre is_valid() == true
       *  @pre диапазон [begin, end) — допустимая область памяти для записи
       *  @param begin — начало буфера
       *  @param end — конец буфера (не включая end)
       *  @param dry_run — если true, ничего не записывать, только вычислить длину
       *  @return char* — указатель сразу за последним символом, который был бы записан при dry_run == false и достаточном размере буфера (смысл только если возвращаемый указатель ≥ begin)
       *  @post если строка помещается в [begin, end) и dry_run == false, то [begin, возврат) содержит строковое представление актива; при dry_run == true, недостаточном месте или переполнении при расчёте конца запись не выполняется
       */
      char* write_as_string( char* begin, char* end, bool dry_run = false )const {
         bool negative = (amount < 0);
         uint64_t abs_amount = static_cast<uint64_t>(negative ? -amount : amount);
         // 0 <= abs_amount <= std::numeric_limits<int64_t>::max() < 10^19 < std::numeric_limits<uint64_t>::max()

         uint8_t precision = symbol.precision();

         int sufficient_size = std::max(static_cast<int>(precision), 19) + 11;
         if( dry_run || (begin + sufficient_size < begin) || (begin + sufficient_size > end) ) {
            char* start_of_symbol = write_decimal( begin, end, true, abs_amount, precision, negative ) + 1;
            char* actual_end = symbol.code().write_as_string( start_of_symbol, end, true );
            if( dry_run || (actual_end < begin) || (actual_end > end) ) return actual_end;
         }

         char* end_of_number = write_decimal( begin, end, false, abs_amount, precision, negative );
         *(end_of_number) = ' ';

         return symbol.code().write_as_string( end_of_number + 1, end );
      }

      /**
       * Преобразование %asset в std::string
       *
       * @brief Преобразование %asset в std::string
       */
      std::string to_string()const {
         int buffer_size = std::max(static_cast<int>(symbol.precision()), 19) + 11;
         char buffer[buffer_size];
         char* end = write_as_string( buffer, buffer + buffer_size );
         check( end <= buffer + buffer_size, "insufficient space in buffer" ); // should never fail

         return {buffer, end};
      }

      /**
       * Вывод актива (печать в лог контракта COOPOS)
       *
       * @brief Вывод asset
       */
      void print()const {
         int buffer_size = std::max(static_cast<int>(symbol.precision()), 19) + 11;
         char buffer[buffer_size];
         char* end = write_as_string( buffer, buffer + buffer_size );
         check( end <= buffer + buffer_size, "insufficient space in buffer" ); // should never fail

         if( buffer < end )
            printl( buffer, (end-buffer) );
      }

      EOSLIB_SERIALIZE( asset, (amount)(symbol) )
   };

  /**
   *  Расширенный актив: количество и контракт-эмитент токена в сети COOPOS
   *
   *  @ingroup asset
   */
   struct extended_asset {
      /**
       * Сам актив (количество и символ)
       */
      asset quantity;

      /**
       * Имя контракта, выпускающего токен
       */
      name contract;

      /**
       * Возвращает расширенный символ (символ + контракт)
       *
       * @return extended_symbol — расширенный символ
       */
      extended_symbol get_extended_symbol()const { return extended_symbol{ quantity.symbol, contract }; }

      /**
       * Конструктор по умолчанию
       */
      extended_asset() = default;

       /**
       * Создаёт расширенный актив по количеству и расширенному символу
       */
      extended_asset( int64_t v, extended_symbol s ):quantity(v,s.get_symbol()),contract(s.get_contract()){}
      /**
       * Создаёт расширенный актив по активу и имени контракта
       */
      extended_asset( asset a, name c ):quantity(a),contract(c){}

      /**
       * Вывод расширенного актива (печать в лог контракта COOPOS)
       */
      void print()const {
         quantity.print();
         ::eosio::print("@", contract);
      }

      /// @cond OPERATORS

      // Унарный минус
      extended_asset operator-()const {
         return {-quantity, contract};
      }

      // Оператор вычитания
      friend extended_asset operator - ( const extended_asset& a, const extended_asset& b ) {
         eosio::check( a.contract == b.contract, "type mismatch" );
         return {a.quantity - b.quantity, a.contract};
      }

      // Оператор сложения
      friend extended_asset operator + ( const extended_asset& a, const extended_asset& b ) {
         eosio::check( a.contract == b.contract, "type mismatch" );
         return {a.quantity + b.quantity, a.contract};
      }

      /// Оператор сложения с присваиванием.
      friend extended_asset& operator+=( extended_asset& a, const extended_asset& b ) {
         eosio::check( a.contract == b.contract, "type mismatch" );
         a.quantity += b.quantity;
         return a;
      }

      /// Оператор вычитания с присваиванием.
      friend extended_asset& operator-=( extended_asset& a, const extended_asset& b ) {
         eosio::check( a.contract == b.contract, "type mismatch" );
         a.quantity -= b.quantity;
         return a;
      }

      /// Умножение на число с присваиванием.
      extended_asset& operator*=( int64_t b ) {
         quantity *= b;
         return *this;
      }

      /// Умножение актива на число.
      friend extended_asset operator*( const extended_asset& a, int64_t b ) {
         return {a.quantity * b, a.contract};
      }

      /// Умножение числа на актив.
      friend extended_asset operator*( int64_t a, const extended_asset& b ) {
         return {a * b.quantity, b.contract};
      }

      /// Деление расширенных активов по количеству (аналог int64_t operator/( const asset& a, const asset& b )).
      friend int64_t operator/( const extended_asset& a, const extended_asset& b ) {
         eosio::check( a.contract == b.contract, "type mismatch" );
         return a.quantity / b.quantity;
      }

      /// Деление на число с присваиванием.
      extended_asset& operator/=( int64_t b ) {
         quantity /= b;
         return *this;
      }

      /// Деление актива на число.
      friend extended_asset operator/( const extended_asset& a, int64_t b ) {
         return {a.quantity / b, a.contract};
      }


      /// Оператор «меньше»
      friend bool operator<( const extended_asset& a, const extended_asset& b ) {
         eosio::check( a.contract == b.contract, "type mismatch" );
         return a.quantity < b.quantity;
      }

      /// Оператор «больше»
      friend bool operator>( const extended_asset& a, const extended_asset& b ) {
         eosio::check( a.contract == b.contract, "type mismatch" );
         return a.quantity > b.quantity;
      }

      /// Оператор равенства
      friend bool operator==( const extended_asset& a, const extended_asset& b ) {
         return std::tie(a.quantity, a.contract) == std::tie(b.quantity, b.contract);
      }

      /// Оператор неравенства
      friend bool operator!=( const extended_asset& a, const extended_asset& b ) {
         return std::tie(a.quantity, a.contract) != std::tie(b.quantity, b.contract);
      }

      /// Оператор «меньше или равно»
      friend bool operator<=( const extended_asset& a, const extended_asset& b ) {
         eosio::check( a.contract == b.contract, "type mismatch" );
         return a.quantity <= b.quantity;
      }

      /// Оператор «больше или равно»
      friend bool operator>=( const extended_asset& a, const extended_asset& b ) {
         eosio::check( a.contract == b.contract, "type mismatch" );
         return a.quantity >= b.quantity;
      }

      /// @endcond

      EOSLIB_SERIALIZE( extended_asset, (quantity)(contract) )
   };
}
