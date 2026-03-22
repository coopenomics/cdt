/**
 *  @file
 *  @copyright см. cdt/LICENSE
 */
#pragma once

#include "check.hpp"
#include "fixed_bytes.hpp"
#include "varint.hpp"
#include "serialize.hpp"

#include <array>

namespace eosio {

   namespace internal_use_do_not_use {
      extern "C" {

         struct __attribute__((aligned (16))) capi_checksum256_ext { uint8_t hash[32]; };

         __attribute__((eosio_wasm_import))
         int32_t alt_bn128_add( const char* op1, uint32_t op1_len, const char* op2, uint32_t op2_len, char* result, uint32_t result_len);

         __attribute__((eosio_wasm_import))
         int32_t alt_bn128_mul( const char* g1, uint32_t g1_len, const char* scalar, uint32_t scalar_len, char* result, uint32_t result_len);

         __attribute__((eosio_wasm_import))
         int32_t alt_bn128_pair( const char* pairs, uint32_t pairs_len);

         __attribute__((eosio_wasm_import))
         int32_t mod_exp( const char* base, uint32_t base_len, const char* exp, uint32_t exp_len, const char* mod, uint32_t mod_len, char* result, uint32_t result_len);

         __attribute__((eosio_wasm_import))
         int32_t blake2_f( uint32_t rounds, const char* state, uint32_t state_len, const char* msg, uint32_t msg_len, const char* t0_offset, uint32_t t0_len, const char* t1_offset, uint32_t t1_len, int32_t final, char* result, uint32_t result_len);

         __attribute__((eosio_wasm_import))
         int32_t k1_recover( const char* sig, uint32_t sig_len, const char* dig, uint32_t dig_len, char* pub, uint32_t pub_len);

         __attribute__((eosio_wasm_import))
         void sha3( const char* data, uint32_t data_len, char* hash, uint32_t hash_len, int32_t keccak );
      }

      static inline auto sha3_helper(const char* data, uint32_t length, bool keccak) {
         internal_use_do_not_use::capi_checksum256_ext hash;
         internal_use_do_not_use::sha3( data, length, (char*)&hash, sizeof(hash), keccak);
         eosio::checksum256 dg;
         eosio::datastream<uint8_t*> ds = {&hash.hash[0], sizeof(hash)};
         ds >> dg;
         return dg;
      }
   }

   /**
    *  @defgroup crypto Криптография
    *  @ingroup core
    *  @brief Определяет API вычисления и проверки хешей, для которых
    *  требуется активация соответствующего криптопротокольного свойства в цепи COOPOS
    */

   /**
    * Абстракция изменяемых точек G1 и G2
    *
    *  @ingroup crypto
    */
   template <std::size_t Size = 32>
   struct ec_point {
      /**
       * Байты координаты x
       */
      std::vector<char> x;

      /**
       * Байты координаты y
       */
      std::vector<char> y;

      /**
       * Создаёт точку по координатам x и y
       *
       * @param x_ - Координата x, вектор символов
       * @param y_ - Координата y, вектор символов
      */
      ec_point(std::vector<char>& x_, std::vector<char>& y_)
      :x(x_), y(y_)
      {
         eosio::check( x_.size() == y_.size(), "x's size must be equal to y's" );
         eosio::check ( x_.size() == Size, "point size must match");
      };

      /**
       * Создаёт точку из сериализованного представления
       *
       * @param p - Сериализованная точка
       */
      ec_point(std::vector<char>& p)
      :x(p.data(), p.data() + Size), y(p.data() + Size, p.data() + p.size())
      {
         eosio::check ( p.size() == Size * 2, "point size must match");
      };

      /**
       *  Возвращает сериализованную точку (только x и y)
       */
      std::vector<char> serialized() const {
         std::vector<char> x_and_y( x );
         x_and_y.insert( x_and_y.end(), y.begin(), y.end() );
         return x_and_y;
      }
   };

   /**
    * Абстракция точек G1 и G2 только для чтения
    *
    *  @ingroup crypto
    */
   template <std::size_t Size = 32>
   struct ec_point_view {
      /**
       * Указатель на координату x
       */
      const char* x;

      /**
       * Указатель на координату y
       */
      const char* y;

      /**
       * Number of bytes in each of x and y
       */
      uint32_t size;

      /**
       * Создаёт представление точки по x и y
       *
       * @param x_     - Координата x, указатель на байты
       * @param x_size - Размер x
       * @param y_     - Координата y, указатель на байты
       * @param y_size - Размер y
       */
      ec_point_view(const char* x_, uint32_t x_size, const char* y_, uint32_t y_size)
      :x(x_), y(y_), size(x_size)
      {
         eosio::check ( x_size == y_size, "x's size must be equal to y's");
         eosio::check ( size == Size, "point size must match");
      };

      /**
       * Создаёт представление точки из сериализованных данных
       *
       * @param p - Сериализованная точка
       */
      ec_point_view(const std::vector<char>& p)
      :x(p.data()), y(p.data() + Size), size(Size)
      {
         eosio::check ( p.size() == Size * 2, "point size must match");
      };

      /**
       * Создаёт представление точки из объекта точки
       *
       * @param p - Точка
       */
      ec_point_view(const ec_point<Size>& p)
      :x(p.x.data()), y(p.y.data()), size(Size)
      {
      };

      /**
       *  Возвращает сериализованную точку (только x и y)
       */
      std::vector<char> serialized() const {
         std::vector<char> x_and_y( x, x + size );
         x_and_y.insert( x_and_y.end(), y, y + size );
         return x_and_y;
      }
   };

   static constexpr size_t g1_coordinate_size = 32;
   static constexpr size_t g2_coordinate_size = 64;

   using g1_point = ec_point<g1_coordinate_size>;
   using g2_point = ec_point<g2_coordinate_size>;
   using g1_point_view = ec_point_view<g1_coordinate_size>;
   using g2_point_view = ec_point_view<g2_coordinate_size>;

   /**
    * Большое целое.
    *
    *  @ingroup crypto
    */
   using bigint = std::vector<char>;

   /**
    *  Сложение на эллиптической кривой `alt_bn128`
    *
    *  @ingroup crypto
    *  @param op1 - Операнд 1
    *  @param op2 - Операнд 2
    *  @return Результат сложения; при ошибке — исключение
    */
   template <typename T>
   inline g1_point alt_bn128_add( const T& op1, const T& op2 ) {
      auto op_1 = op1.serialized();
      auto op_2 = op2.serialized();
      std::vector<char> buf ( 2 * g1_coordinate_size ); // buffer storing x and y
      auto ret = internal_use_do_not_use::alt_bn128_add( op_1.data(), op_1.size(), op_2.data(), op_2.size(), buf.data(), buf.size());
      eosio::check ( ret == 0, "internal_use_do_not_use::alt_bn128_add failed" );
      return g1_point { buf };
   }

   /**
    *  Сложение на эллиптической кривой `alt_bn128`
    *
    *  @ingroup crypto
    *  @param op1 - Операнд 1
    *  @param op1_len - Размер операнда 1
    *  @param op2 - Операнд 2
    *  @param op2_len - Размер операнда 2
    *  @param result - Результат сложения
    *  @param result_len - Размер результата
    *  @return -1 при ошибке, иначе 0
    */
   inline int32_t alt_bn128_add( const char* op1, uint32_t op1_len, const char* op2, uint32_t op2_len, char* result, uint32_t result_len ) {
      return internal_use_do_not_use::alt_bn128_add( op1, op1_len, op2, op2_len, result, result_len);
   }

   /**
    *  Скалярное умножение на эллиптической кривой `alt_bn128`
    *
    *  @ingroup crypto
    *  @param g1 - Точка G1
    *  @param scalar - Скалярный множитель
    *  @return Результат умножения; при ошибке — исключение
    */
   template <typename T>
   inline g1_point alt_bn128_mul( const T& g1, const bigint& scalar) {
      auto g1_bin = g1.serialized();
      std::vector<char> buf( 2 * g1_coordinate_size ); // buffer storing x and y
      auto ret = internal_use_do_not_use::alt_bn128_mul( g1_bin.data(), g1_bin.size(), scalar.data(), scalar.size(), buf.data(), buf.size());
      eosio::check ( ret == 0, "internal_use_do_not_use::alt_bn128_mul failed");
      return g1_point { buf };
   }

   /**
    *  Скалярное умножение на эллиптической кривой `alt_bn128`
    *
    *  @ingroup crypto
    *  @param g1 - Точка G1
    *  @param g1_len - Размер точки G1
    *  @param scalar - Скалярный множитель
    *  @param scalar_len - Размер скаляра
    *  @param result - Результат скалярного умножения
    *  @param result_len - Размер результата
    *  @return -1 при ошибке, иначе 0
    */
   inline int32_t alt_bn128_mul( const char* g1, uint32_t g1_len, const char* scalar, uint32_t scalar_len, char* result, uint32_t result_len ) {
      return internal_use_do_not_use::alt_bn128_mul( g1, g1_len, scalar, scalar_len, result, result_len );
   }

   /**
    *  Проверка optimal-Ate pairing на кривой `alt_bn128`
    *
    *  @ingroup crypto
    *  @param pairs - Пары точек g1 и g2
    *  @return -1 при ошибке, 1 если ложь, 0 если истина и успех
    */
   template <typename G1_T, typename G2_T>
   inline int32_t alt_bn128_pair( const std::vector<std::pair<G1_T, G2_T>>& pairs ) {
      std::vector<char> g1_g2_pairs;
      for ( const auto& pair: pairs ) {
         auto g1_bin = pair.first.serialized();
         auto g2_bin = pair.second.serialized();
         g1_g2_pairs.insert( g1_g2_pairs.end(), g1_bin.begin(), g1_bin.end() );
         g1_g2_pairs.insert( g1_g2_pairs.end(), g2_bin.begin(), g2_bin.end() );
      }
      return internal_use_do_not_use::alt_bn128_pair( g1_g2_pairs.data(), g1_g2_pairs.size() );
   }

   /**
    *  Проверка optimal-Ate pairing на кривой `alt_bn128`
    *
    *  @ingroup crypto
    *  @param pairs - Пары g1 и g2
    *  @param pairs_len - Размер данных пар
    *  @return -1 при ошибке, 1 если ложь, 0 если истина и успех
    */
   inline int32_t alt_bn128_pair( const char* pairs, uint32_t pairs_len ) {
      return internal_use_do_not_use::alt_bn128_pair( pairs, pairs_len );
   }
   
   /**
    *  Модульное возведение в степень для больших целых
    *  Возвращает ( BASE^EXP ) % MOD
    *
    *  @ingroup crypto
    *  @param base - Основание (BASE)
    *  @param exp - Показатель степени (EXP)
    *  @param mod - Модуль (MOD)
    *  @param result - Результат модульного возведения в степень
    *  @return -1 при ошибке, иначе 0
    */

   inline int32_t mod_exp( const bigint& base, const bigint& exp, const bigint& mod, bigint& result) {
      eosio::check( result.size() >= mod.size(), "mod_exp result parameter's size must be >= mod's size" );
      auto ret = internal_use_do_not_use::mod_exp( base.data(), base.size(), exp.data(), exp.size(), mod.data(), mod.size(), result.data(), result.size());
      return ret;
   }

   /**
    *  Модульное возведение в степень для больших целых
    *  Возвращает ( BASE^EXP ) % MOD
    *
    *  @ingroup crypto
    *  @param base - Основание (BASE)
    *  @param base_len - Размер основания
    *  @param exp - Показатель степени (EXP)
    *  @param exp_len - Размер показателя
    *  @param mod - Модуль (MOD)
    *  @param mod_len - Размер модуля
    *  @param result - Результат модульного возведения в степень
    *  @param result_len - Размер результата
    *  @return -1 при ошибке, иначе 0
    */

   inline int32_t mod_exp( const char* base, uint32_t base_len, const char* exp, uint32_t exp_len, const char* mod, uint32_t mod_len, char* result, uint32_t result_len ) {
      return internal_use_do_not_use::mod_exp( base, base_len, exp, exp_len, mod, mod_len, result, result_len);
   }

   static constexpr size_t blake2f_result_size = 64;

   /**
    *  Функция сжатия BLAKE2 «F»
    *  https://eips.ethereum.org/EIPS/eip-152
    *
    *  @ingroup crypto
    *  @param rounds - Число раундов
    *  @param state - Вектор состояния
    *  @param msg - Вектор блока сообщения
    *  @param t0_offset - Смещения счётчиков
    *  @param t1_offset - Смещения счётчиков
    *  @param final - Флаг последнего блока
    *  @param result - Результат сжатия
    *  @return -1 при ошибке, иначе 0
    */
   inline int32_t blake2_f( uint32_t rounds, const std::vector<char>& state, const std::vector<char>& msg, const std::vector<char>& t0_offset, const std::vector<char>& t1_offset, bool final, std::vector<char>& result) {
      eosio::check( result.size() >= blake2f_result_size, "blake2_f result parameter's size must be >= 64" );
      return internal_use_do_not_use::blake2_f( rounds, state.data(), state.size(), msg.data(), msg.size(), t0_offset.data(), t0_offset.size(), t1_offset.data(), t1_offset.size(), final, result.data(), result.size());
   }

   /**
    *  Функция сжатия BLAKE2 «F»
    *  https://eips.ethereum.org/EIPS/eip-152
    *
    *  @ingroup crypto
    *  @param rounds - Число раундов
    *  @param state - Вектор состояния
    *  @param state_len - Размер вектора состояния
    *  @param msg - Вектор блока сообщения
    *  @param msg_len - Размер блока сообщения
    *  @param t0_offset - Смещения счётчиков
    *  @param t0_len - Размер t0_offset
    *  @param t1_offset - Смещения счётчиков
    *  @param t1_len - Размер t1_offset
    *  @param final - Флаг последнего блока
    *  @param result - Результат сжатия
    *  @param result_len - Размер результата
    *  @return -1 при ошибке, иначе 0
    */
   inline int32_t blake2_f( uint32_t rounds, const char* state, uint32_t state_len, const char* msg, uint32_t msg_len, 
                     const char* t0_offset, uint32_t t0_len, const char* t1_offset, uint32_t t1_len, int32_t final, char* result, uint32_t result_len) {
      return internal_use_do_not_use::blake2_f( rounds, state, state_len, msg, msg_len, t0_offset, t0_len, t1_offset, t1_len, final, result, result_len);
   }

   /**
    *  Вычисляет хеш `data` алгоритмом SHA3 (вариант NIST).
    *
    *  @ingroup crypto
    *  @param data - Данные для хеширования
    *  @param length - Длина данных
    *  @return eosio::checksum256 - Вычисленный дайджест
    */
   inline eosio::checksum256 sha3(const char* data, uint32_t length) {
      return internal_use_do_not_use::sha3_helper(data, length, false);
   }

   /**
    *  Проверяет, совпадает ли SHA3-хеш данных с переданным дайджестом.
    *
    *  @ingroup crypto
    *  @param data - Данные для хеширования
    *  @param length - Длина данных
    *  @param hash - Дайджест для сравнения
    *  @note Этот метод не вырезается при повторном воспроизведении
    */
   inline void assert_sha3(const char* data, uint32_t length, const eosio::checksum256& hash) {
      const auto& res = internal_use_do_not_use::sha3_helper(data, length, false);
      check( hash == res, "SHA3 hash of `data` does not match given `hash`");
   }

   /**
    *  Вычисляет хеш `data` алгоритмом SHA3 Keccak.
    *
    *  @ingroup crypto
    *  @param data - Данные для хеширования
    *  @param length - Длина данных
    *  @return eosio::checksum256 - Вычисленный дайджест
    */
   inline eosio::checksum256 keccak(const char* data, uint32_t length) {
      return internal_use_do_not_use::sha3_helper(data, length, true);
   }

   /**
    *  Проверяет, совпадает ли Keccak-хеш данных с переданным дайджестом.
    *
    *  @ingroup crypto
    *  @param data - Данные для хеширования
    *  @param length - Длина данных
    *  @param hash - Дайджест для сравнения
    *  @note Этот метод не вырезается при повторном воспроизведении
    */
   inline void assert_keccak(const char* data, uint32_t length, const eosio::checksum256& hash) {
      const auto& res = internal_use_do_not_use::sha3_helper(data, length, true);
      check( hash == res, "Keccak hash of `data` does not match given `hash`");
   }

   /**
    *  Вычисляет несжатый открытый ключ по подписи и дайджесту.
    *
    *  @ingroup crypto
    *  @param sig - Подпись
    *  @param sig_len - Размер подписи
    *  @param dig - Дайджест подписанного сообщения
    *  @param dig_len - Размер дайджеста
    *  @param pub - Буфер для открытого ключа
    *  @param pub_len - Размер буфера открытого ключа
    *
    *  @return -1 при ошибке, иначе 0
    */
   inline int32_t k1_recover( const char* sig, uint32_t sig_len, const char* dig, uint32_t dig_len, char* pub, uint32_t pub_len ) {
      return internal_use_do_not_use::k1_recover( sig, sig_len, dig, dig_len, pub, pub_len );
   }
}
