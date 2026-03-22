#pragma once

#include "../../core/eosio/name.hpp"
#include "../../core/eosio/datastream.hpp"


/**
 * @defgroup contract Контракт (Contract)
 * @ingroup contracts
 * @ingroup types
 * @brief Определяет тип контракта — %базовый класс для каждого контракта COOPOS
 */

/**
 * Вспомогательные макросы для сокращения шаблонного кода в типовых контрактах
 * @ingroup contract
 */
#define CONTRACT class [[eosio::contract]]
#define ACTION   [[eosio::action]] void
#define TABLE struct [[eosio::table]]

namespace eosio {

/**
 * %Базовый класс контракта COOPOS.
 *
 * @ingroup contract
 * @details Новый контракт должен наследовать этот класс, чтобы можно было использовать макрос EOSIO_ABI.
 */
class contract {
   public:
      /**
       * Создаёт контракт с заданными параметрами
       *
       * @param self — имя аккаунта, на котором развёрнут контракт
       * @param first_receiver — аккаунт, на который действие впервые поступило
       * @param ds — используемый поток данных (datastream)
       */
      contract( name self, name first_receiver, datastream<const char*> ds ):_self(self),_first_receiver(first_receiver),_ds(ds) {}

      /**
       *
       * Имя этого контракта
       *
       * @return name — имя контракта
       */
      inline name get_self()const { return _self; }

      /**
       * Имя first_receiver для обрабатываемого действия (устаревший метод).
       *
       * @return name — первый получатель текущего действия (параметр конструктора `first_receiver`)
       */
      [[deprecated]]
      inline name get_code()const { return _first_receiver; }

      /**
       * Аккаунт, на который входящее действие впервые поступило.
       *
       * @return name — первый получатель текущего действия (параметр конструктора `first_receiver`)
       */
      inline name get_first_receiver()const { return _first_receiver; }

      /**
       * Поток данных (datastream) этого контракта
       *
       * @return datastream<const char*> — поток данных контракта
       */
      inline datastream<const char*>& get_datastream() { return _ds; }

      /**
       * Поток данных (datastream) этого контракта (константная версия)
       *
       * @return datastream<const char*> — поток данных контракта
       */
      inline const datastream<const char*>& get_datastream()const { return _ds; }

   protected:
      /**
       * Имя аккаунта, на котором развёрнут контракт.
       */
      name _self;

      /**
       * Аккаунт, на который входящее действие впервые поступило.
       */
      name _first_receiver;

      /**
       * Поток данных (datastream) контракта
       */
      datastream<const char*> _ds = datastream<const char*>(nullptr, 0);
};
}
