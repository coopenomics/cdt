/**
 *  @file
 *  @copyright см. eos/LICENSE
 */
#pragma once

namespace eosio {
   /**
    * @defgroup varint Целое переменной длины
    * @ingroup core
    * @ingroup types
    * @brief Определяет тип целого переменной длины для более эффективной сериализации
    */

   /**
    *  Целое без знака переменной длины. Обеспечивает более эффективную сериализацию 32-битного беззнакового int.
    *  Сериализует 32-битное беззнаковое целое минимально возможным числом байт.
    *  `varuint32` без знака и использует [VLQ или кодирование Base-128](https://en.wikipedia.org/wiki/Variable-length_quantity)
    *
    *  @ingroup varint
    */
   struct unsigned_int {
       /**
        * Создаёт новый объект беззнакового целого
        *
        * @param v - Источник
        */
       constexpr unsigned_int( uint32_t v = 0 ):value(v){}

       /**
        * Создаёт новый объект беззнакового целого из типа, приводимого к uint32_t
        *
        * @tparam T - Тип источника
        * @param v - Источник
        * @pre T должен быть приводим к uint32_t
        */
       template<typename T>
       constexpr unsigned_int( T v ):value(v){}

       //operator uint32_t()const { return value; }
       //operator uint64_t()const { return value; }

       /**
        * Приводит unsigned_int к типу T
        *
        * @tparam T - Целевой тип преобразования
        * @return T - Результат преобразования
        */
       template<typename T>
       constexpr operator T()const { return static_cast<T>(value); }

       /// @cond OPERATORS

       /**
        * Присваивает 32-битное беззнаковое целое
        *
        * @param v - Источник
        * @return unsigned_int& - Ссылка на этот объект
        */
       constexpr unsigned_int& operator=( uint32_t v ) { value = v; return *this; }

       /// @endcond

       /**
        * Содержащееся значение
        */
       uint32_t value;

       /// @cond OPERATORS

       /**
        * Проверка равенства между unsigned_int и 32-битным беззнаковым целым
        *
        * @param i - Объект unsigned_int для сравнения
        * @param v - 32-битное беззнаковое целое для сравнения
        * @return true - если равны
        * @return false - иначе
        */
       constexpr friend bool operator==( const unsigned_int& i, const uint32_t& v ) { return i.value == v; }

       /**
        * Проверка равенства между 32-битным беззнаковым целым и unsigned_int
        *
        * @param i - 32-битное беззнаковое целое для сравнения
        * @param v - Объект unsigned_int для сравнения
        * @return true - если равны
        * @return false - иначе
        */
       constexpr friend bool operator==( const uint32_t& i, const unsigned_int& v ) { return i == v.value; }

       /**
        * Проверка равенства двух объектов unsigned_int
        *
        * @param i - Первый unsigned_int для сравнения
        * @param v - Второй unsigned_int для сравнения
        * @return true - если равны
        * @return false - иначе
        */
       constexpr friend bool operator==( const unsigned_int& i, const unsigned_int& v ) { return i.value == v.value; }

       /**
        * Проверка неравенства между unsigned_int и 32-битным беззнаковым целым
        *
        * @param i - Объект unsigned_int для сравнения
        * @param v - 32-битное беззнаковое целое для сравнения
        * @return true - если не равны
        * @return false - иначе
        */
       constexpr friend bool operator!=( const unsigned_int& i, const uint32_t& v ) { return i.value != v; }

       /**
        * Проверка неравенства между 32-битным беззнаковым целым и unsigned_int
        *
        * @param i - 32-битное беззнаковое целое для сравнения
        * @param v - Объект unsigned_int для сравнения
        * @return true - если не равны
        * @return false - иначе
        */
       constexpr friend bool operator!=( const uint32_t& i, const unsigned_int& v ) { return i != v.value; }

       /**
        * Проверка неравенства двух объектов unsigned_int
        *
        * @param i - Первый unsigned_int для сравнения
        * @param v - Второй unsigned_int для сравнения
        * @return true - если не равны
        * @return false - иначе
        */
       constexpr friend bool operator!=( const unsigned_int& i, const unsigned_int& v ) { return i.value != v.value; }

       /**
        * Проверяет, меньше ли unsigned_int заданного 32-битного беззнакового целого
        *
        * @param i - Объект unsigned_int для сравнения
        * @param v - 32-битное беззнаковое целое для сравнения
        * @return true - если i меньше v
        * @return false - иначе
        */
       constexpr friend bool operator<( const unsigned_int& i, const uint32_t& v ) { return i.value < v; }

       /**
        * Проверяет, меньше ли 32-битное беззнаковое целое заданного unsigned_int
        *
        * @param i - 32-битное беззнаковое целое для сравнения
        * @param v - Объект unsigned_int для сравнения
        * @return true - если i меньше v
        * @return false - иначе
        */
       constexpr friend bool operator<( const uint32_t& i, const unsigned_int& v ) { return i < v.value; }

       /**
        * Проверяет, меньше ли первый unsigned_int второго
        *
        * @param i - Первый unsigned_int для сравнения
        * @param v - Второй unsigned_int для сравнения
        * @return true - если i меньше v
        * @return false - иначе
        */
       constexpr friend bool operator<( const unsigned_int& i, const unsigned_int& v ) { return i.value < v.value; }

       /**
        * Проверяет, больше или равен ли unsigned_int заданному 32-битному беззнаковому целому
        *
        * @param i - Объект unsigned_int для сравнения
        * @param v - 32-битное беззнаковое целое для сравнения
        * @return true - если i больше или равен v
        * @return false - иначе
        */
       constexpr friend bool operator>=( const unsigned_int& i, const uint32_t& v ) { return i.value >= v; }

       /**
        * Проверяет, больше или равно ли 32-битное беззнаковое целое заданному unsigned_int
        *
        * @param i - 32-битное беззнаковое целое для сравнения
        * @param v - Объект unsigned_int для сравнения
        * @return true - если i больше или равен v
        * @return false - иначе
        */
       constexpr friend bool operator>=( const uint32_t& i, const unsigned_int& v ) { return i >= v.value; }

       /**
        * Проверяет, больше или равен ли первый unsigned_int второму
        *
        * @param i - Первый unsigned_int для сравнения
        * @param v - Второй unsigned_int для сравнения
        * @return true - если i больше или равен v
        * @return false - иначе
        */
       constexpr friend bool operator>=( const unsigned_int& i, const unsigned_int& v ) { return i.value >= v.value; }


       /// @endcond

       /// @cond IMPLEMENTATIONS

       /**
        *  Сериализует unsigned_int минимально возможным числом байт
        *
        *  @param ds - Поток для записи
        *  @param v - Значение для сериализации
        *  @tparam DataStream - Тип потока данных
        *  @return DataStream& - Ссылка на поток данных
        */
       template<typename DataStream>
       friend DataStream& operator << ( DataStream& ds, const unsigned_int& v ){
          uint64_t val = v.value;
          do {
             uint8_t b = uint8_t(val) & 0x7f;
             val >>= 7;
             b |= ((val > 0) << 7);
             ds.write((char*)&b,1);//.put(b);
          } while( val );
          return ds;
       }

       /**
        *  Десериализует unsigned_int
        *
        *  @param ds - Поток для чтения
        *  @param vi - Назначение для десериализованного значения
        *  @tparam DataStream - Тип потока данных
        *  @return DataStream& - Ссылка на поток данных
        */
       template<typename DataStream>
       friend DataStream& operator >> ( DataStream& ds, unsigned_int& vi ){
         uint64_t v = 0; char b = 0; uint8_t by = 0;
         do {
            ds.get(b);
            v |= uint32_t(uint8_t(b) & 0x7f) << by;
            by += 7;
         } while( uint8_t(b) & 0x80 );
         vi.value = static_cast<uint32_t>(v);
         return ds;
       }

       /// @endcond
   };

   /**
    *  Целое со знаком переменной длины. Обеспечивает более эффективную сериализацию 32-битного знакового int.
    *  Сериализует 32-битное знаковое целое минимально возможным числом байт.
    *
    *  @ingroup varint
    *  @note `varint32` со знаком и использует [кодирование Zig-Zag](https://developers.google.com/protocol-buffers/docs/encoding#signed-integers)
    */
   struct signed_int {
       /**
        * Создаёт новый объект знакового целого
        *
        * @param v - Источник
        */
       constexpr signed_int( int32_t v = 0 ):value(v){}

       /// @cond OPERATORS

       /**
        * Приводит signed_int к примитивному 32-битному знаковому целому
        *
        * @return int32_t - Результат преобразования
        */
       constexpr operator int32_t()const { return value; }


       /**
        * Присваивает объект, приводимый к int32_t
        *
        * @tparam T - Тип присваиваемого объекта
        * @param v - Источник
        * @return signed_int& - Ссылка на этот объект
        */
       template<typename T>
       constexpr signed_int& operator=( const T& v ) { value = v; return *this; }

       /**
        * Оператор инкремента (постфиксный)
        *
        * @return signed_int - Новый signed_int со значением, увеличенным относительно текущего
        */
       constexpr signed_int operator++(int) { return value++; }

       /**
        * Оператор инкремента (префиксный)
        *
        * @return signed_int& - Ссылка на текущий объект
        */
       constexpr signed_int& operator++(){ ++value; return *this; }

       /// @endcond

       /**
        * Содержащееся значение
        */
       int32_t value;

       /// @cond OPERATORS

       /**
        * Проверка равенства между signed_int и 32-битным целым
        *
        * @param i - Объект signed_int для сравнения
        * @param v - 32-битное целое для сравнения
        * @return true - если равны
        * @return false - иначе
        */
       constexpr friend bool operator==( const signed_int& i, const int32_t& v ) { return i.value == v; }

       /**
        * Проверка равенства между 32-битным целым и signed_int
        *
        * @param i - 32-битное целое для сравнения
        * @param v - Объект signed_int для сравнения
        * @return true - если равны
        * @return false - иначе
        */
       constexpr friend bool operator==( const int32_t& i, const signed_int& v ) { return i == v.value; }

       /**
        * Проверка равенства двух объектов signed_int
        *
        * @param i - Первый signed_int для сравнения
        * @param v - Второй signed_int для сравнения
        * @return true - если равны
        * @return false - иначе
        */
       constexpr friend bool operator==( const signed_int& i, const signed_int& v ) { return i.value == v.value; }


       /**
        * Проверка неравенства между signed_int и 32-битным целым
        *
        * @param i - Объект signed_int для сравнения
        * @param v - 32-битное целое для сравнения
        * @return true - если не равны
        * @return false - иначе
        */
       constexpr friend bool operator!=( const signed_int& i, const int32_t& v )    { return i.value != v; }

       /**
        * Проверка неравенства между 32-битным целым и signed_int
        *
        * @param i - 32-битное целое для сравнения
        * @param v - Объект signed_int для сравнения
        * @return true - если не равны
        * @return false - иначе
        */
       constexpr friend bool operator!=( const int32_t& i, const signed_int& v ) { return i != v.value; }

       /**
        * Проверка неравенства двух объектов signed_int
        *
        * @param i - Первый signed_int для сравнения
        * @param v - Второй signed_int для сравнения
        * @return true - если не равны
        * @return false - иначе
        */
       constexpr friend bool operator!=( const signed_int& i, const signed_int& v ) { return i.value != v.value; }

       /**
        * Проверяет, меньше ли signed_int заданного 32-битного целого
        *
        * @param i - Объект signed_int для сравнения
        * @param v - 32-битное целое для сравнения
        * @return true - если i меньше v
        * @return false - иначе
        */
       constexpr friend bool operator<( const signed_int& i, const int32_t& v ) { return i.value < v; }

       /**
        * Проверяет, меньше ли 32-битное целое заданного signed_int
        *
        * @param i - 32-битное целое для сравнения
        * @param v - Объект signed_int для сравнения
        * @return true - если i меньше v
        * @return false - иначе
        */
       constexpr friend bool operator<( const int32_t& i, const signed_int& v ) { return i < v.value; }

       /**
        * Проверяет, меньше ли первый signed_int второго
        *
        * @param i - Первый signed_int для сравнения
        * @param v - Второй signed_int для сравнения
        * @return true - если i меньше v
        * @return false - иначе
        */
       constexpr friend bool operator<( const signed_int& i, const signed_int& v ) { return i.value < v.value; }


       /**
        * Проверяет, больше или равен ли signed_int заданному 32-битному целому
        *
        * @param i - Объект signed_int для сравнения
        * @param v - 32-битное целое для сравнения
        * @return true - если i больше или равен v
        * @return false - иначе
        */
       constexpr friend bool operator>=( const signed_int& i, const int32_t& v ) { return i.value >= v; }

       /**
        * Проверяет, больше или равно ли 32-битное целое заданному signed_int
        *
        * @param i - 32-битное целое для сравнения
        * @param v - Объект signed_int для сравнения
        * @return true - если i больше или равен v
        * @return false - иначе
        */
       constexpr friend bool operator>=( const int32_t& i, const signed_int& v ) { return i >= v.value; }

       /**
        * Проверяет, больше или равен ли первый signed_int второму
        *
        * @param i - Первый signed_int для сравнения
        * @param v - Второй signed_int для сравнения
        * @return true - если i больше или равен v
        * @return false - иначе
        */
       constexpr friend bool operator>=( const signed_int& i, const signed_int& v ) { return i.value >= v.value; }

       /// @endcond

       /// @cond IMPLEMENTATIONS

       /**
        *  Сериализует signed_int минимально возможным числом байт
        *
        *  @param ds - Поток для записи
        *  @param v - Значение для сериализации
        *  @tparam DataStream - Тип потока данных
        *  @return DataStream& - Ссылка на поток данных
        */
       template<typename DataStream>
       friend DataStream& operator << ( DataStream& ds, const signed_int& v ){
         uint32_t val = uint32_t((v.value<<1) ^ (v.value>>31));
         do {
            uint8_t b = uint8_t(val) & 0x7f;
            val >>= 7;
            b |= ((val > 0) << 7);
            ds.write((char*)&b,1);//.put(b);
         } while( val );
          return ds;
       }

       /**
        *  Десериализует signed_int
        *
        *  @param ds - Поток для чтения
        *  @param vi - Назначение для десериализованного значения
        *  @tparam DataStream - Тип потока данных
        *  @return DataStream& - Ссылка на поток данных
        */
       template<typename DataStream>
       friend DataStream& operator >> ( DataStream& ds, signed_int& vi ){
         uint32_t v = 0; char b = 0; int by = 0;
         do {
            ds.get(b);
            v |= uint32_t(uint8_t(b) & 0x7f) << by;
            by += 7;
         } while( uint8_t(b) & 0x80 );
         vi.value = (v>>1) ^ (~(v&1)+1ull);
         return ds;
       }

       /// @endcond
   };
}
