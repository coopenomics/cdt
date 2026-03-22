/**
 *  @file
 *  @copyright см. eos/LICENSE
 */
#pragma once

#include "fixed_bytes.hpp"
#include "varint.hpp"
#include "serialize.hpp"
#include "name.hpp"

#include <array>

namespace eosio {

   /**
    *  @defgroup public_key Тип открытого ключа
    *  @ingroup core
    *  @ingroup types
    *  @brief Задаёт тип открытого ключа
    */

   /**
    *  Данные открытого ключа ECC в COOPOS
    *
    *  Представление фиксированного размера: сжатый открытый ключ K1 или R1

    *  @ingroup public_key
    */
   using ecc_public_key = std::array<char, 33>;

   /**
    *  Открытый ключ WebAuthN в COOPOS
    *
    *  @ingroup public_key
    */
   struct webauthn_public_key {
      /**
       * Перечисление возможных результатов проверки присутствия пользователя
       * @see https://w3c.github.io/webauthn/#test-of-user-presence
       */
      enum class user_presence_t : uint8_t {
         USER_PRESENCE_NONE,
         USER_PRESENCE_PRESENT,
         USER_PRESENCE_VERIFIED
      };

      /**
       * Материал ключа ECC
       */
      ecc_public_key     key;

      /**
       * Ожидаемый результат проверки присутствия пользователя для действительной подписи
       * @see https://w3c.github.io/webauthn/#test-of-user-presence
       */
      user_presence_t    user_presence;

      /**
       * Идентификатор доверенной стороны (Relying Party) для WebAuthN
       * @see https://w3c.github.io/webauthn/#relying-party-identifier
       */
      std::string        rpid;

      /// @cond OPERATORS

      friend bool operator == ( const webauthn_public_key& a, const webauthn_public_key& b ) {
         return std::tie(a.key,a.user_presence,a.rpid) == std::tie(b.key,b.user_presence,b.rpid);
      }
      friend bool operator != ( const webauthn_public_key& a, const webauthn_public_key& b ) {
         return std::tie(a.key,a.user_presence,a.rpid) != std::tie(b.key,b.user_presence,b.rpid);
      }
      friend bool operator < ( const webauthn_public_key& a, const webauthn_public_key& b ) {
         return std::tie(a.key,a.user_presence,a.rpid) < std::tie(b.key,b.user_presence,b.rpid);
      }
      friend bool operator <= ( const webauthn_public_key& a, const webauthn_public_key& b ) {
         return std::tie(a.key,a.user_presence,a.rpid) <= std::tie(b.key,b.user_presence,b.rpid);
      }
      friend bool operator > ( const webauthn_public_key& a, const webauthn_public_key& b ) {
         return std::tie(a.key,a.user_presence,a.rpid) > std::tie(b.key,b.user_presence,b.rpid);
      }
      friend bool operator >= ( const webauthn_public_key& a, const webauthn_public_key& b ) {
         return std::tie(a.key,a.user_presence,a.rpid) >= std::tie(b.key,b.user_presence,b.rpid);
      }

      /// @cond
   };

   /**
    *  Открытый ключ COOPOS
    *
    *  Открытый ключ — вариант из:
    *   0 : открытый ключ ECC K1
    *   1 : открытый ключ ECC R1
    *   2 : открытый ключ WebAuthN (требуется активация на цепи-хосте COOPOS консенсусного обновления WEBAUTHN_KEY)
    *
    *  @ingroup public_key
    */
   using public_key = std::variant<ecc_public_key, ecc_public_key, webauthn_public_key>;


   /// @cond IMPLEMENTATIONS

   /**
    *  Сериализует eosio::webauthn_public_key в поток
    *
    *  @ingroup public_key
    *  @param ds - Поток для записи
    *  @param pubkey - Значение для сериализации
    *  @tparam DataStream - Тип буфера потока данных
    *  @return DataStream& - Ссылка на поток данных
    */
   template<typename DataStream>
   inline DataStream& operator<<(DataStream& ds, const eosio::webauthn_public_key& pubkey) {
      ds << pubkey.key << pubkey.user_presence << pubkey.rpid;
      return ds;
   }

   /**
    *  Десериализует eosio::webauthn_public_key из потока
    *
    *  @ingroup public_key
    *  @param ds - Поток для чтения
    *  @param pubkey - Назначение для десериализованного значения
    *  @tparam DataStream - Тип буфера потока данных
    *  @return DataStream& - Ссылка на поток данных
    */
   template<typename DataStream>
   inline DataStream& operator>>(DataStream& ds, eosio::webauthn_public_key& pubkey) {
      ds >> pubkey.key >> pubkey.user_presence >> pubkey.rpid;
      return ds;
   }

   /// @endcond

   /**
    *  @defgroup signature Подпись
    *  @ingroup core
    *  @ingroup types
    *  @brief Задаёт тип подписи
    */

   /**
    *  Данные подписи ECC в COOPOS
    *
    *  Представление фиксированного размера: компактная подпись ECC K1 или R1

    *  @ingroup signature
    */
   using ecc_signature = std::array<char, 65>;

   /**
    *  Подпись WebAuthN (формат COOPOS / совместимый с протоколом)
    *
    *  @ingroup signature
    */
   struct webauthn_signature {
      /**
       * Данные подписи ECC
       */
      ecc_signature                     compact_signature;

      /**
       * Закодированные данные аутентификатора, возвращённые церемонией WebAuthN
       * @see https://w3c.github.io/webauthn/#sctn-authenticator-data
       */
      std::vector<uint8_t>              auth_data;

      /**
       * Данные клиента в JSON из церемонии WebAuthN
       * @see https://w3c.github.io/webauthn/#dictdef-collectedclientdata
       */
      std::string                       client_json;

      /// @cond OPERATORS

      friend bool operator == ( const webauthn_signature& a, const webauthn_signature& b ) {
         return std::tie(a.compact_signature,a.auth_data,a.client_json) == std::tie(b.compact_signature,b.auth_data,b.client_json);
      }
      friend bool operator != ( const webauthn_signature& a, const webauthn_signature& b ) {
         return std::tie(a.compact_signature,a.auth_data,a.client_json) != std::tie(b.compact_signature,b.auth_data,b.client_json);
      }

      /// @cond
   };

   /**
    *  Подпись COOPOS
    *
    *  Подпись — вариант из:
    *   0 : подпись ECC K1
    *   1 : подпись ECC R1
    *   2 : подпись WebAuthN (требуется активация на цепи-хосте COOPOS консенсусного обновления WEBAUTHN_KEY)
    *
    *  @ingroup signature
    */
   using signature = std::variant<ecc_signature, ecc_signature, webauthn_signature>;

   /// @cond IMPLEMENTATIONS

   /**
    *  Сериализует eosio::webauthn_signature в поток
    *
    *  @param ds - Поток для записи
    *  @param sig - Значение для сериализации
    *  @tparam DataStream - Тип буфера потока данных
    *  @return DataStream& - Ссылка на поток данных
    */
   template<typename DataStream>
   inline DataStream& operator<<(DataStream& ds, const eosio::webauthn_signature& sig) {
      ds << sig.compact_signature << sig.auth_data << sig.client_json;
      return ds;
   }

   /**
    *  Десериализует eosio::webauthn_signature из потока
    *
    *  @param ds - Поток для чтения
    *  @param sig - Назначение для десериализованного значения
    *  @tparam DataStream - Тип буфера потока данных
    *  @return DataStream& - Ссылка на поток данных
    */
   template<typename DataStream>
   inline DataStream& operator>>(DataStream& ds, eosio::webauthn_signature& sig) {
      ds >> sig.compact_signature >> sig.auth_data >> sig.client_json;
      return ds;
   }

   /// @endcond

   /**
    *  @defgroup crypto Криптография
    *  @ingroup core
    *  @brief Определяет API вычисления и проверки хешей
    */

   /**
    *  Проверяет, совпадает ли SHA256-хеш данных с переданным дайджестом.
    *
    *  @ingroup crypto
    *  @param data - Данные для хеширования
    *  @param length - Длина данных
    *  @param hash - Дайджест для сравнения
    *  @note В режиме быстрой оценки этот метод сводится к NO-OP.
    */
   void assert_sha256( const char* data, uint32_t length, const eosio::checksum256& hash );

   /**
    *  Проверяет, совпадает ли SHA1-хеш данных с переданным дайджестом.
    *
    *  @ingroup crypto
    *  @param data - Данные для хеширования
    *  @param length - Длина данных
    *  @param hash - Дайджест для сравнения
    *  @note В режиме быстрой оценки этот метод сводится к NO-OP.
    */
   void assert_sha1( const char* data, uint32_t length, const eosio::checksum160& hash );

   /**
    *  Проверяет, совпадает ли SHA512-хеш данных с переданным дайджестом.
    *
    *  @ingroup crypto
    *  @param data - Данные для хеширования
    *  @param length - Длина данных
    *  @param hash - Дайджест для сравнения
    *  @note В режиме быстрой оценки этот метод сводится к NO-OP.
    */
   void assert_sha512( const char* data, uint32_t length, const eosio::checksum512& hash );

   /**
    *  Проверяет, совпадает ли RIPEMD160-хеш данных с переданным дайджестом.
    *
    *  @ingroup crypto
    *  @param data - Данные для хеширования
    *  @param length - Длина данных
    *  @param hash - Дайджест для сравнения
    */
   void assert_ripemd160( const char* data, uint32_t length, const eosio::checksum160& hash );

   /**
    *  Вычисляет хеш `data` алгоритмом SHA256.
    *
    *  @ingroup crypto
    *  @param data - Данные для хеширования
    *  @param length - Длина данных
    *  @return eosio::checksum256 - Вычисленный дайджест
    */
   eosio::checksum256 sha256( const char* data, uint32_t length );

   /**
    *  Вычисляет хеш `data` алгоритмом SHA1.
    *
    *  @ingroup crypto
    *
    *  @param data - Данные для хеширования
    *  @param length - Длина данных
    *  @return eosio::checksum160 - Вычисленный дайджест
    */
   eosio::checksum160 sha1( const char* data, uint32_t length );

   /**
    *  Вычисляет хеш `data` алгоритмом SHA512.
    *
    *  @ingroup crypto
    *  @param data - Данные для хеширования
    *  @param length - Длина данных
    *  @return eosio::checksum512 - Вычисленный дайджест
    */
   eosio::checksum512 sha512( const char* data, uint32_t length );

   /**
    *  Вычисляет хеш `data` алгоритмом RIPEMD160.
    *
    *  @ingroup crypto
    *  @param data - Данные для хеширования
    *  @param length - Длина данных
    *  @return eosio::checksum160 - Вычисленный дайджест
    */
   eosio::checksum160 ripemd160( const char* data, uint32_t length );

   /**
    *  Вычисляет открытый ключ по дайджесту и подписи.
    *
    *  @ingroup crypto
    *  @param digest - Дайджест подписанного сообщения
    *  @param sig - Подпись
    *  @return eosio::public_key - Восстановленный открытый ключ
    */
   eosio::public_key recover_key( const eosio::checksum256& digest, const eosio::signature& sig );

   /**
    *  Сравнивает заданный открытый ключ с восстановленным по дайджесту и подписи.
    *
    *  @ingroup crypto
    *  @param digest - Дайджест подписанного сообщения
    *  @param sig - Подпись
    *  @param pubkey - Открытый ключ
    */
   void assert_recover_key( const eosio::checksum256& digest, const eosio::signature& sig, const eosio::public_key& pubkey );

   /**
    *  Проверяет подпись по хешу, сверяет восстановленный открытый ключ с ожидаемым
    *  и удостоверяется, что ключ привязан к указанному разрешению аккаунта.
    *
    *  @ingroup crypto
    *  @param digest - Дайджест подписанного сообщения
    *  @param sig - Подпись
    *  @param pubkey - Ожидаемый открытый ключ
    *  @param account - Имя аккаунта для проверки владения ключом
    *  @param permission - Имя разрешения (например, "active"_n, "owner"_n)
    *
    *  @throw eosio::check завершится ошибкой, если:
    *    - подпись недействительна;
    *    - восстановленный ключ не совпадает с ожидаемым открытым ключом;
    *    - аккаунт не существует;
    *    - разрешение не существует для аккаунта;
    *    - открытый ключ не принадлежит указанному разрешению аккаунта.
    */
   void assert_recover_key_account( const eosio::checksum256& digest, const eosio::signature& sig, const eosio::public_key& pubkey, eosio::name account, eosio::name permission );
}
