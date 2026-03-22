#pragma once
#include <vector>
#include "../../core/eosio/name.hpp"
#include "../../core/eosio/crypto.hpp"

namespace eosio {

  /**
   *  @defgroup producer_key Ключ производителя
   *  @ingroup contracts
   *  @ingroup types
   *  @brief Сопоставляет производителя с его ключом подписи блока; используется в расписании производителей
   */

  /**
   *  Сопоставляет производителя с ключом подписи блока; используется в расписании производителей
   *
   *  @ingroup producer_key
   */
  struct producer_key {

    /**
     *  Имя производителя
     *
     *  @ingroup producer_key
     */
    name             producer_name;

    /**
     *  Ключ подписи блока, используемый этим производителем
     *
     *  @ingroup producer_key
     */
    public_key       block_signing_key;

    /// @cond OPERATORS

    friend constexpr bool operator < ( const producer_key& a, const producer_key& b ) {
      return a.producer_name < b.producer_name;
    }

    /// @endcond

    CDT_REFLECT(producer_name, block_signing_key);
  };

   /**
    *  @defgroup producer_schedule Расписание производителей
    *  @ingroup contracts
    *  @ingroup types
    *  @brief Задаёт порядок, имена аккаунтов и ключи подписи активного набора производителей.
    */

   /**
    *  Задаёт порядок, имена аккаунтов и ключи подписи активного набора производителей
    *
    * @ingroup producer_schedule
    */
   struct producer_schedule {
      /**
       * Номер версии расписания (монотонно возрастающий)
       */
      uint32_t                     version;

      /**
       * Список производителей расписания с ключами подписи
       */
      std::vector<producer_key>    producers;

      CDT_REFLECT(version, producers);
   };

   /**
    *  @defgroup producer_authority Полномочие производителя
    *  @ingroup contracts
    *  @ingroup types
    *  @brief Сопоставляет производителя с гибкой структурой полномочий; используется в расписании производителей
    */

   /**
    *  Сопоставляет открытый ключ с целочисленным весом
    *
    * @ingroup producer_authority
    */
   struct key_weight {
      /**
       * Открытый ключ во взвешенном пороговом полномочии с несколькими подписями (multi-sig)
       *
       * @brief открытый ключ во взвешенном пороговом полномочии с несколькими подписями (multi-sig)
       */
      public_key key;

      /**
       * Вес подписи закрытым ключом, соответствующим этому открытому ключу
       *
       * @brief вес открытого ключа
       */
      uint16_t   weight;

      CDT_REFLECT(key, weight);
   };

   /**
    *  Полномочие подписи блока, версия 0: взвешенный пороговый multi-sig на производителя
    *
    * @ingroup producer_authority
    *
    * @brief взвешенное пороговое полномочие с несколькими подписями (multi-sig)
    */
   struct block_signing_authority_v0 {
      /**
       * Минимальный порог суммы весов компонентных ключей, при котором полномочие считается удовлетворённым
       *
       * @brief минимальный порог суммы весов компонентных ключей, при котором полномочие считается удовлетворённым
       */
      uint32_t                    threshold;

      /**
       * Компонентные ключи и связанные с ними веса
       *
       * @brief компонентные ключи и связанные с ними веса
       */
      std::vector<key_weight>     keys;

      bool is_valid()const;

      CDT_REFLECT(threshold, keys);
   };

   /**
    *  Вариант (std::variant) всех допустимых полномочий подписи блока
    *
    * @ingroup producer_authority
    */
   using block_signing_authority = std::variant<block_signing_authority_v0>;

   /**
    *  Сопоставляет производителя с полномочием подписи блока; используется в расписании производителей
    *
    * @ingroup producer_authority
    *
    * @brief Сопоставляет производителя с его ключом подписи
    */
   struct producer_authority {

      /**
       * Имя производителя
       *
       * @brief имя производителя
       */
      name             producer_name;

      /**
       * Полномочие подписи блока для этого производителя
       */
      block_signing_authority       authority;

      friend constexpr bool operator < ( const producer_authority& a, const producer_authority& b ) {
         return a.producer_name < b.producer_name;
      }

      CDT_REFLECT(producer_name, authority);
   };

   /**
    *  Возвращает список имён активных производителей
    *
    *  @ingroup producer_schedule
    */
   std::vector<name> get_active_producers();

} /// пространство имён eosio
