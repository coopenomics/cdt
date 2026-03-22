/**
 *  @file
 *  @copyright см. eos/LICENSE
 */
#pragma once
#include "datastream.hpp"

#include <array>
#include <algorithm>
#include <functional>
#include <type_traits>

namespace eosio {

   /// @cond IMPLEMENTATIONS

   template<size_t Size>
   class fixed_bytes;

   template<size_t Size>
   bool operator ==(const fixed_bytes<Size> &c1, const fixed_bytes<Size> &c2);

   template<size_t Size>
   bool operator !=(const fixed_bytes<Size> &c1, const fixed_bytes<Size> &c2);

   template<size_t Size>
   bool operator >(const fixed_bytes<Size> &c1, const fixed_bytes<Size> &c2);

   template<size_t Size>
   bool operator <(const fixed_bytes<Size> &c1, const fixed_bytes<Size> &c2);

   template<size_t Size>
   bool operator >=(const fixed_bytes<Size> &c1, const fixed_bytes<Size> &c2);

   template<size_t Size>
   bool operator <=(const fixed_bytes<Size> &c1, const fixed_bytes<Size> &c2);

   /// @endcond

    /**
     *  @defgroup fixed_bytes Массив байт фиксированного размера
     *  @ingroup core
     *  @ingroup types
     *  @brief Массив байт фиксированного размера с лексикографическим упорядочиванием
     */

   /**
    *  Массив байт фиксированного размера с лексикографическим сравнением по словам
    *
    *  @ingroup fixed_bytes
    *  @tparam Size — размер в байтах
    */
   template<size_t Size>
   class fixed_bytes {
      private:

         template<bool...> struct bool_pack;
         template<bool... bs>
         using all_true = std::is_same< bool_pack<bs..., true>, bool_pack<true, bs...> >;

         template<typename Word, size_t NumWords>
         static void set_from_word_sequence(const Word* arr_begin, const Word* arr_end, fixed_bytes<Size>& key)
         {
            auto itr = key._data.begin();
            word_t temp_word = 0;
            const size_t sub_word_shift = 8 * sizeof(Word);
            const size_t num_sub_words = sizeof(word_t) / sizeof(Word);
            auto sub_words_left = num_sub_words;
            for( auto w_itr = arr_begin; w_itr != arr_end; ++w_itr ) {
               if( sub_words_left > 1 ) {
                   temp_word |= static_cast<word_t>(*w_itr);
                   temp_word <<= sub_word_shift;
                   --sub_words_left;
                   continue;
               }

               eosio::check( sub_words_left == 1, "unexpected error in fixed_bytes constructor" );
               temp_word |= static_cast<word_t>(*w_itr);
               sub_words_left = num_sub_words;

               *itr = temp_word;
               temp_word = 0;
               ++itr;
            }
            if( sub_words_left != num_sub_words ) {
               if( sub_words_left > 1 )
                  temp_word <<= 8 * (sub_words_left-1);
               *itr = temp_word;
            }
         }

      public:

         typedef uint128_t word_t;

         /**
          * Число «слов» (word_t) внутри объекта; размер слова — 16 байт (uint128_t)
          */

         static constexpr size_t num_words() { return (Size + sizeof(word_t) - 1) / sizeof(word_t); }

         /**
          * Число дополняющих (неиспользуемых) байт в последнем слове после выравнивания по word_t
          */
         static constexpr size_t padded_bytes() { return num_words() * sizeof(word_t) - Size; }

         /**
         * Конструктор по умолчанию; все байты нулевые
         */
         constexpr fixed_bytes() : _data() {}

         /**
         * Конструктор из std::array из num_words() элементов word_t
         *
         * @param arr — исходные данные
         */
         fixed_bytes(const std::array<word_t, num_words()>& arr)
         {
           std::copy(arr.begin(), arr.end(), _data.begin());
         }

         /**
         * Конструктор из std::array слов Word меньше word_t (сборка в слова хранения)
         *
         * @param arr — исходные данные
         */
         template<typename Word, size_t NumWords,
                  typename Enable = typename std::enable_if<std::is_integral<Word>::value &&
                                                             std::is_unsigned<Word>::value &&
                                                             !std::is_same<Word, bool>::value &&
                                                             std::less<size_t>{}(sizeof(Word), sizeof(word_t))>::type >
         fixed_bytes(const std::array<Word, NumWords>& arr)
         {
            static_assert( sizeof(word_t) == (sizeof(word_t)/sizeof(Word)) * sizeof(Word),
                           "size of the backing word size is not divisible by the size of the array element" );
            static_assert( sizeof(Word) * NumWords <= Size, "too many words supplied to fixed_bytes constructor" );

            set_from_word_sequence<Word, NumWords>(arr.data(), arr.data() + arr.size(), *this);
         }

         /**
         * Конструктор из C-массива слов Word меньше word_t
         *
         * @param arr — исходные данные
         */
         template<typename Word, size_t NumWords,
                  typename Enable = typename std::enable_if<std::is_integral<Word>::value &&
                                                             std::is_unsigned<Word>::value &&
                                                             !std::is_same<Word, bool>::value &&
                                                             std::less<size_t>{}(sizeof(Word), sizeof(word_t))>::type >
         fixed_bytes(const Word(&arr)[NumWords])
         {
            static_assert( sizeof(word_t) == (sizeof(word_t)/sizeof(Word)) * sizeof(Word),
                           "size of the backing word size is not divisible by the size of the array element" );
            static_assert( sizeof(Word) * NumWords <= Size, "too many words supplied to fixed_bytes constructor" );

            set_from_word_sequence<Word, NumWords>(arr, arr + NumWords, *this);
         }

         /**
         *  Создаёт fixed_bytes из последовательности слов (аргументы функции)
         *
         *  @tparam FirstWord — тип первого слова
         *  @tparam Rest — типы остальных слов
         *  @param first_word — первое слово
         *  @param rest — остальные слова
         */
         template<typename FirstWord, typename... Rest>
         static
         fixed_bytes<Size>
         make_from_word_sequence(typename std::enable_if<std::is_integral<FirstWord>::value &&
                                                          std::is_unsigned<FirstWord>::value &&
                                                          !std::is_same<FirstWord, bool>::value &&
                                                          sizeof(FirstWord) <= sizeof(word_t) &&
                                                          all_true<(std::is_same<FirstWord, Rest>::value)...>::value,
                                                         FirstWord>::type first_word,
                                 Rest... rest)
         {
            static_assert( sizeof(word_t) == (sizeof(word_t)/sizeof(FirstWord)) * sizeof(FirstWord),
                           "size of the backing word size is not divisible by the size of the words supplied as arguments" );
            static_assert( sizeof(FirstWord) * (1 + sizeof...(Rest)) <= Size, "too many words supplied to make_from_word_sequence" );

            fixed_bytes<Size> key;
            std::array<FirstWord, 1+sizeof...(Rest)> arr{{ first_word, rest... }};
            set_from_word_sequence<FirstWord, 1+sizeof...(Rest)>(arr.data(), arr.data() + arr.size(), key);
            return key;
         }

         /**
          * Константная ссылка на внутренний std::array слов
          */
         const auto& get_array()const { return _data; }

         /**
          * Указатель на сырые данные массива слов
          */
         auto data() { return _data.data(); }

         /// @cond INTERNAL

         /**
          * Указатель на сырые данные (const)
          */
         auto data()const { return _data.data(); }

         /// @endcond

         /**
          * Число слов word_t во внутреннем массиве
          */
         auto size()const { return _data.size(); }


         /**
          * Извлекает байты фиксированной длины Size в порядке старшинства для сериализации
          *
          * @return массив из Size байт
          */
         std::array<uint8_t, Size> extract_as_byte_array()const {
            std::array<uint8_t, Size> arr;

            const size_t num_sub_words = sizeof(word_t);

            auto arr_itr  = arr.begin();
            auto data_itr = _data.begin();

            for( size_t counter = _data.size(); counter > 0; --counter, ++data_itr ) {
               size_t sub_words_left = num_sub_words;

               auto temp_word = *data_itr;
               if( counter == 1 ) { // If last word in _data array...
                  sub_words_left -= padded_bytes();
                  temp_word >>= 8*padded_bytes();
               }
               for( ; sub_words_left > 0; --sub_words_left ) {
                  *(arr_itr + sub_words_left - 1) = static_cast<uint8_t>(temp_word & 0xFF);
                  temp_word >>= 8;
               }
               arr_itr += num_sub_words;
            }

            return arr;
         }

         /**
          * Печать в шестнадцатеричном виде в лог контракта COOPOS
          *
          */
         inline void print()const {
            auto arr = extract_as_byte_array();
            printhex(static_cast<const void*>(arr.data()), arr.size());
         }

         /// @cond OPERATORS

         friend bool operator == <>(const fixed_bytes<Size> &c1, const fixed_bytes<Size> &c2);

         friend bool operator != <>(const fixed_bytes<Size> &c1, const fixed_bytes<Size> &c2);

         friend bool operator > <>(const fixed_bytes<Size> &c1, const fixed_bytes<Size> &c2);

         friend bool operator < <>(const fixed_bytes<Size> &c1, const fixed_bytes<Size> &c2);

         friend bool operator >= <>(const fixed_bytes<Size> &c1, const fixed_bytes<Size> &c2);

         friend bool operator <= <>(const fixed_bytes<Size> &c1, const fixed_bytes<Size> &c2);

         /// @endcond

      private:

         std::array<word_t, num_words()> _data;
    };

  /// @cond IMPLEMENTATIONS

   /**
    * Лексикографическое сравнение: равенство
    *
    * @param c1 — первый операнд
    * @param c2 — второй операнд
    * @return true — c1 равен c2
    */
   template<size_t Size>
   bool operator ==(const fixed_bytes<Size> &c1, const fixed_bytes<Size> &c2) {
      return c1._data == c2._data;
   }

   /**
    * Лексикографическое сравнение: неравенство
    *
    * @param c1 — первый операнд
    * @param c2 — второй операнд
    * @return true — c1 не равен c2
    */
   template<size_t Size>
   bool operator !=(const fixed_bytes<Size> &c1, const fixed_bytes<Size> &c2) {
      return c1._data != c2._data;
   }

   /**
    * Лексикографическое сравнение: больше
    *
    * @param c1 — первый операнд
    * @param c2 — второй операнд
    * @return true — c1 больше c2
    */
   template<size_t Size>
   bool operator >(const fixed_bytes<Size>& c1, const fixed_bytes<Size>& c2) {
      return c1._data > c2._data;
   }

   /**
    * Лексикографическое сравнение: меньше
    *
    * @param c1 — первый операнд
    * @param c2 — второй операнд
    * @return true — c1 меньше c2
    */
   template<size_t Size>
   bool operator <(const fixed_bytes<Size> &c1, const fixed_bytes<Size> &c2) {
      return c1._data < c2._data;
   }

   /**
    * Лексикографическое сравнение: больше или равно
    *
    * @param c1 — первый операнд
    * @param c2 — второй операнд
    * @return true — c1 не меньше c2
    */
   template<size_t Size>
   bool operator >=(const fixed_bytes<Size>& c1, const fixed_bytes<Size>& c2) {
      return c1._data >= c2._data;
   }

   /**
    * Лексикографическое сравнение: меньше или равно
    *
    * @param c1 — первый операнд
    * @param c2 — второй операнд
    * @return true — c1 не больше c2
    */
   template<size_t Size>
   bool operator <=(const fixed_bytes<Size> &c1, const fixed_bytes<Size> &c2) {
      return c1._data <= c2._data;
   }


   using checksum160 = fixed_bytes<20>;
   using checksum256 = fixed_bytes<32>;
   using checksum512 = fixed_bytes<64>;

   /**
    *  Сериализация fixed_bytes в поток
    *
    *  @brief Сериализация fixed_bytes
    *  @param ds — поток записи
    *  @param d — значение
    *  @tparam DataStream — тип буфера потока данных
    *  @return DataStream& — ссылка на поток
    */
   template<typename DataStream, size_t Size>
   inline DataStream& operator<<(DataStream& ds, const fixed_bytes<Size>& d) {
      auto arr = d.extract_as_byte_array();
      ds.write( (const char*)arr.data(), arr.size() );
      return ds;
   }

   /**
    *  Десериализация fixed_bytes из потока
    *
    *  @brief Десериализация fixed_bytes
    *  @param ds — поток чтения
    *  @param d — приёмник значения
    *  @tparam DataStream — тип буфера потока данных
    *  @return DataStream& — ссылка на поток
    */
   template<typename DataStream, size_t Size>
   inline DataStream& operator>>(DataStream& ds, fixed_bytes<Size>& d) {
      std::array<uint8_t, Size> arr;
      ds.read( (char*)arr.data(), arr.size() );
      d = fixed_bytes<Size>( arr );
      return ds;
   }

   /// @endcond
}
