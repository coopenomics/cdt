/**
 *  @file
 *  @copyright см. eos/LICENSE
 */
#pragma once
#include <utility>
#include <string>



namespace eosio {
   namespace internal_use_do_not_use {
      extern "C" {
         __attribute__((eosio_wasm_import))
         void prints(const char*);

         __attribute__((eosio_wasm_import))
         void prints_l(const char*, uint32_t);

         __attribute__((eosio_wasm_import))
         void printi(int64_t);

         __attribute__((eosio_wasm_import))
         void printui(uint64_t);

         __attribute__((eosio_wasm_import))
         void printi128(const int128_t*);

         __attribute__((eosio_wasm_import))
         void printui128(const uint128_t*);

         __attribute__((eosio_wasm_import))
         void printsf(float);

         __attribute__((eosio_wasm_import))
         void printdf(double);

         __attribute__((eosio_wasm_import))
         void printqf(const long double*);

         __attribute__((eosio_wasm_import))
         void printn(uint64_t);

         __attribute__((eosio_wasm_import))
         void printhex(const void*, uint32_t);
      }
   };

   /**
    *  @defgroup console Консоль
    *  @ingroup core
    *  @brief Определяет C++-обёртку для вывода текстовых сообщений в лог/консоль
    *
    *  @details Этот API использует вариадические шаблоны C++ и определение типов,
    *  чтобы упростить вывод любых встроенных типов. Можно перегрузить
    *  метод `print()` и для своих типов.
    *
    *  **Пример:**
    *  ```
    *     print( "hello world, this is a number: ", 5 );
    *  ```
    *
    *  @section override Перегрузка print для своих типов
    *
    *  Два способа перегрузить print:
    *  1. реализовать void print( const T& )
    *  2. реализовать T::print() const
    */

   /**
    *  Выводит блок байт в шестнадцатеричном виде
    *
    *  @ingroup console
    *  @param ptr  - Указатель на байты
    *  @param size - Число байт для вывода
    */
   inline void printhex( const void* ptr, uint32_t size) {
      internal_use_do_not_use::printhex(ptr, size);
   }

   /**
    *  Выводит строку заданной длины
    *
    *  @ingroup console
    *  @param ptr - Строка
    *  @param len - Число символов для вывода
    */
   inline void printl( const char* ptr, size_t len ) {
     internal_use_do_not_use::prints_l(ptr, len);
   }

   /**
    *  Выводит строку
    *
    *  @ingroup console
    *  @param ptr - Строка с нулевым завершением
    */
   inline void print( const char* ptr ) {
     internal_use_do_not_use::prints(ptr);
   }

   /**
    * Выводит знаковое целое 8–128 бит
    *
    * @param num Значение для вывода
    */
   template <typename T, std::enable_if_t<std::is_integral<std::decay_t<T>>::value &&
                                          std::is_signed<std::decay_t<T>>::value, int> = 0>
   inline void print( T num ) {
      if constexpr(std::is_same<T, int128_t>::value)
        internal_use_do_not_use::printi128(&num);
      else if constexpr(std::is_same<T, char>::value)
        internal_use_do_not_use::prints_l( &num, 1 );
      else
        internal_use_do_not_use::printi(num);
   }

   /**
    *  Выводит беззнаковое целое 8–128 бит
    *
    *  @param num Значение для вывода
    */
   template <typename T, std::enable_if_t<std::is_integral<std::decay_t<T>>::value &&
                                          !std::is_signed<std::decay_t<T>>::value, int> = 0>
   inline void print( T num ) {
      if constexpr(std::is_same<T, uint128_t>::value)
         internal_use_do_not_use::printui128(&num);
      else if constexpr(std::is_same<T, bool>::value)
         internal_use_do_not_use::prints(num?"true":"false");
      else
        internal_use_do_not_use::printui(num);
   }

   /**
    *  Выводит число с плавающей точкой одинарной точности (float)
    *
    *  @ingroup console
    *  @param num Значение для вывода
    */
   inline void print( float num ) { internal_use_do_not_use::printsf( num ); }

   /**
    *  Выводит число с плавающей точкой двойной точности (double)
    *
    *  @ingroup console
    *  @param num Значение для вывода
    */
   inline void print( double num ) { internal_use_do_not_use::printdf( num ); }

   /**
    *  Выводит число с плавающей точкой расширенной точности (long double)
    *
    *  @ingroup console
    *  @param num Значение для вывода
    */
   inline void print( long double num ) { internal_use_do_not_use::printqf( &num ); }

  /**
    *  Выводит объект класса
    *
    *  @ingroup console
    *  @param t Значение для вывода
    *  @pre У T должна быть реализована функция print()
    */
   template<typename T, std::enable_if_t<!std::is_integral<std::decay_t<T>>::value, int> = 0>
   inline void print( T&& t ) {
      if constexpr (std::is_same<std::decay_t<T>, std::string>::value)
         internal_use_do_not_use::prints_l( t.c_str(), t.size() );
      else if constexpr (std::is_same<std::decay_t<T>, char*>::value)
         internal_use_do_not_use::prints(t);
      else
         t.print();
   }

   /**
    *  Выводит строку с нулевым завершением
    *
    *  @ingroup console
    *  @param s Строка с нулевым завершением
    */
   inline void print_f( const char* s ) {
     internal_use_do_not_use::prints(s);
   }

   /**
    *  Выводит форматированную строку. Поведение близко к C printf.
    *
    *  @tparam Arg - Тип значения для подстановки вместо спецификатора формата
    *  @tparam Args - Типы остальных значений для подстановки
    *  @param s - Строка с нулевым завершением (может содержать спецификатор формата)
    *  @param val - Значение для первой подстановки
    *  @param rest - Остальные значения для подстановки
    *
    *  Пример:
    *  @code
    *  print_f("Number of apples: %", 10);
    *  @endcode
    */
   template <typename Arg, typename... Args>
   inline void print_f( const char* s, Arg val, Args... rest ) {
      while ( *s != '\0' ) {
         if ( *s == '%' ) {
            print( val );
            print_f( s+1, rest... );
            return;
         }
         internal_use_do_not_use::prints_l( s, 1 );
         s++;
      }
   }

    /**
     *  Выводит значение или список значений
     *
     *  @tparam Arg - Тип первого значения
     *  @tparam Args - Типы остальных значений
     *  @param a - Первое значение для вывода
     *  @param args - Остальные значения для вывода
     *
     *  Пример:
     *
     *  @code
     *  const char *s = "Hello World!";
     *  uint64_t unsigned_64_bit_int = 1e+18;
     *  uint128_t unsigned_128_bit_int (87654323456);
     *  uint64_t string_as_unsigned_64_bit = "abcde"_n;
     *  print(s , unsigned_64_bit_int, unsigned_128_bit_int, string_as_unsigned_64_bit);
     *  // Ouput: Hello World!100000000000000000087654323456abcde
     *  @endcode
     */
   template<typename Arg, typename... Args>
   void print( Arg&& a, Args&&... args ) {
      print(std::forward<Arg>(a));
      print(std::forward<Args>(args)...);
   }

   /**
    * Имитация потоков в стиле C++
    *
    * @ingroup console
    */
   class iostream {};

   /// @cond OPERATORS

   /**
    *  Перегрузка оператора для iostream C++
    *
    *  @tparam T - Тип выводимого значения
    *  @param out - Выходной поток
    *  @param v - Значение для вывода
    *  @return iostream& - Ссылка на поток ввода-вывода
    *
    *  Пример:
    *
    *  @code
    *  const char *s = "Hello World!";
    *  uint64_t unsigned_64_bit_int = 1e+18;
    *  uint128_t unsigned_128_bit_int (87654323456);
    *  uint64_t string_as_unsigned_64_bit = "abcde"_n;
    *  std::out << s << " " << unsigned_64_bit_int << " "  << unsigned_128_bit_int << " " << string_as_unsigned_64_bit;
    *  // Output: Hello World! 1000000000000000000 87654323456 abcde
    *  @endcode
    */
   template<typename T>
   inline iostream& operator<<( iostream& out, const T& v ) {
      print( v );
      return out;
   }

   /// @endcond

   static iostream cout;
}
