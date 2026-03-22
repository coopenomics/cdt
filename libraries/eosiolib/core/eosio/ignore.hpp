#pragma once

#include "datastream.hpp"

namespace eosio {
   /**
    * @defgroup ignore Игнорирование типа
    * @ingroup core
    * @brief Указывает datastream игнорировать этот тип, сохраняя корректный тип для генератора ABI.
    */

   /**
    * Указывает datastream игнорировать этот тип при упаковке, при этом генератор ABI может вывести корректный тип.
    *
    * @ingroup ignore
    * @details Сейчас после параметра типа ignore в сигнатуре метода не могут следовать «обычные» типы: допустимо void foo(float, ignore<int>), недопустимо void foo(float, ignore<int>, int).
    * @note Ограничение может быть ослаблено в будущих версиях. Формулировка та же: после ignore<int> в списке параметров нежелательны следующие не-ignore типы.
    */
   template <typename T>
   struct [[eosio::ignore]] ignore {};

    /**
    * Обёртка для корректной передачи полезной нагрузки при inline-действиях в COOPOS
    */
   template <typename T>
   struct ignore_wrapper {
      constexpr ignore_wrapper() {}
      constexpr ignore_wrapper(T val) : value(val) {}
      constexpr ignore_wrapper(ignore<T> val) {}
      constexpr inline T get() { return value; }
      constexpr operator T() { return value; }
      constexpr operator ignore<T>() { return {}; }
      T value;
   };

   /**
    *  Сериализация ignore_wrapper<T> в поток (пишется val.value)
    *
    *  @brief Сериализация значения T из ignored_wrapper<T>
    *  @param ds — поток записи
    *  @param val — обёртка с полем value
    *  @tparam DataStream — тип буфера потока данных
    *  @return DataStream& — ссылка на поток
    */
   template<typename DataStream, typename T>
   inline DataStream& operator<<(DataStream& ds, const ::eosio::ignore_wrapper<T>& val) {
     ds << val.value;
     return ds;
   }

   /**
    *  Сериализация ignore<T> — ничего не записывает
    *
    *  @brief Сериализация игнорируемого типа
    *  @param ds — поток записи
    *  @param val — игнорируемое значение
    *  @tparam DataStream — тип буфера потока данных
    *  @return DataStream& — ссылка на поток
    */
   template<typename DataStream, typename T>
   inline DataStream& operator<<(DataStream& ds, const ::eosio::ignore<T>& val) {
     return ds;
   }

   /**
    *  Десериализация ignore<T> — ничего не читает
    *
    *  @brief Десериализация игнорируемого типа
    *  @param ds — поток чтения
    *  @tparam DataStream — тип буфера потока данных
    *  @return DataStream& — ссылка на поток
    */
   template<typename DataStream, typename T>
   inline DataStream& operator>>(DataStream& ds, ::eosio::ignore<T>&) {
     return ds;
   }
}
