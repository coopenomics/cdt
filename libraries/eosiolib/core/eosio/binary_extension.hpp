#pragma once

#include "check.hpp"

namespace eosio {
    /**
    *  @defgroup binary_extension Двоичное расширение
    *  @ingroup core
    *  @ingroup types
    *  @brief Контейнер для двоичной полезной нагрузки расширения
    */

    /**
    *  Опциональное значение типа T для расширений действий/таблиц в COOPOS (аналог optional с размещением в буфере)
    *
    *  @ingroup binary_extension
    *  @tparam T — тип хранимого значения
    */
   template <typename T>
   class binary_extension {
      public:
         using value_type = T;

         constexpr binary_extension() {}
         constexpr binary_extension( const T& ext )
         :_has_value(true)
         {
            ::new (&_data) T(ext);
         }
         constexpr binary_extension( T&& ext )
         :_has_value(true)
         {
            ::new (&_data) T(std::move(ext));
         }
          /** размещает значение T по месту (in-place) */
         template <typename... Args>
         constexpr binary_extension( std::in_place_t, Args&&... args )
         :_has_value(true)
         {
            ::new (&_data) T(std::forward<Args>(args)...);
         }

         constexpr binary_extension( const binary_extension& other )
         :_has_value(other._has_value)
         {
            if( other._has_value ) ::new (&_data) T( *other );
         }

         constexpr binary_extension( binary_extension&& other )
         :_has_value(other._has_value)
         {
            if( other._has_value ) {
               ::new (&_data) T( *std::move(other) );
               other._has_value = false;
            }
         }

         /// @cond INTERNAL
         ~binary_extension() { reset(); }

         /// @cond INTERNAL
         constexpr binary_extension& operator = (const binary_extension& other) {
            if (has_value())
               reset();

            if (other.has_value()) {
               ::new (&_data) T(*other);
               _has_value = true;
            }
            return *this;
         }

         /// @cond INTERNAL
         constexpr binary_extension& operator = (binary_extension&& other) {
            if (has_value())
               reset();

            if (other.has_value()) {
               ::new (&_data) T(*other);
               _has_value = true;
               other._has_value = false;
            }
            return *this;
         }
         /** true, если значение присутствует */
         constexpr explicit operator bool()const { return _has_value; }
         /** true, если значение присутствует */
         constexpr bool has_value()const { return _has_value; }

          /** ссылка на хранимое значение */
         constexpr T& value()& {
            if (!_has_value) {
               check(false, "cannot get value of empty binary_extension");
            }
            return _get();
         }

         /// @cond INTERNAL

          /** константная ссылка на хранимое значение */
         constexpr const T& value()const & {
            if (!_has_value) {
               check(false, "cannot get value of empty binary_extension");
            }
            return _get();
         }

          /** значение или переданный по ссылке запасной объект
          * @pre def должен быть приводим к T
          * */
         template <typename U>
         constexpr auto value_or( U&& def ) -> std::enable_if_t<std::is_convertible<U, T>::value, T&>& {
            if (_has_value)
               return _get();
            return def;
         }
         constexpr T&& value_or()&& {
            if (!_has_value)
               return std::move(T());
            _has_value = false;
            return std::move(_get());
         }
         constexpr const T&& value_or()const&& {
            if (!_has_value)
               return std::move(T());
            _has_value = false;
            return std::move(_get());
         }
         constexpr T value_or()& {
            if (!_has_value)
               return T();
            return _get();
         }
         constexpr T value_or()const& {
            if (!_has_value)
               return T();
            return _get();
         }

         constexpr T* operator->() {
            return &_get();
         }
         constexpr const T* operator->()const {
            return &_get();
         }

         constexpr T& operator*()& {
            return _get();
         }
         constexpr const T& operator*()const& {
            return _get();
         }
         constexpr const T&& operator*()const&& {
            return std::move(_get());
         }
         constexpr T&& operator*()&& {
            return std::move(_get());
         }

         template<typename ...Args>
         T& emplace(Args&& ... args)& {
            if (_has_value) {
               reset();
            }

            ::new (&_data) T( std::forward<Args>(args)... );
            _has_value = true;

            return _get();
         }

         void reset() {
            if( _has_value ) {
               _get().~value_type();
               _has_value = false;
            }
         }

         /// @endcond

       private:
         bool _has_value = false;
         typename std::aligned_storage<sizeof(T), alignof(T)>::type _data;

         constexpr T& _get() {
            return *reinterpret_cast<T*>(&_data);
         }

         constexpr const T& _get()const {
            return *reinterpret_cast<const T*>(&_data);
         }
   };

   /// @cond IMPLEMENTATIONS

   /**
    *  Сериализация binary_extension в поток (через value_or())
    *
    *  @ingroup binary_extension
    *  @brief Сериализация binary_extension
    *  @param ds — поток записи
    *  @param be — значение для сериализации
    *  @tparam DataStream — тип буфера потока данных
    *  @return DataStream& — ссылка на поток
    */
   template<typename DataStream, typename T>
   inline DataStream& operator<<(DataStream& ds, const eosio::binary_extension<T>& be) {
     ds << be.value_or();
     return ds;
   }

   /**
    *  Десериализация binary_extension из потока (при наличии остатка в потоке)
    *
    *  @ingroup binary_extension
    *  @brief Десериализация binary_extension
    *  @param ds — поток чтения
    *  @param be — приёмник значения
    *  @tparam DataStream — тип буфера потока данных
    *  @return DataStream& — ссылка на поток
    */
   template<typename DataStream, typename T>
   inline DataStream& operator>>(DataStream& ds, eosio::binary_extension<T>& be) {
     if( ds.remaining() ) {
        T val;
        ds >> val;
        be.emplace(val);
     }
     return ds;
   }

   /// @endcond

} // namespace eosio
