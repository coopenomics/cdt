/**
 *  @file datastream.hpp
 *  @copyright см. eos/LICENSE
 */
#pragma once
#include "check.hpp"
#include "varint.hpp"
#include <bluegrass/meta/for_each.hpp>

#include <list>
#include <queue>
#include <vector>
#include <array>
#include <set>
#include <map>
#include <string>
#include <optional>
#include <variant>

#include <string.h>

namespace eosio {

/**
 * @defgroup datastream Поток данных
 * @ingroup core
 * @brief Определяет поток данных для чтения и записи в виде байтов
 */

/**
 *  Поток данных для чтения и записи в виде байтов
 *
 *  @tparam T - Тип буфера потока данных
 */
template<typename T>
class datastream {
   public:
      /**
       * Создаёт новый объект потока данных
       *
       * @details Создаёт новый объект потока данных по размеру буфера и начальной позиции буфера
       * @param start - Начальная позиция буфера
       * @param s - Размер буфера
       */
      datastream( T start, size_t s )
      :_start(start),_pos(start),_end(start+s){}

     /**
      *  Пропускает заданное число байт в этом потоке
      *
      *  @param s - Число байт для пропуска
      */
      inline void skip( size_t s ){ _pos += s; }

     /**
      *  Читает заданное число байт из потока в буфер
      *
      *  @param d - Указатель на буфер назначения
      *  @param s - Число байт для чтения
      *  @return true
      */
      inline bool read( void* d, size_t s ) {
        eosio::check( size_t(_end - _pos) >= (size_t)s, "datastream attempted to read past the end" );
        memcpy( d, _pos, s );
        _pos += s;
        return true;
      }

     /**
      *  Записывает заданное число байт в поток из буфера
      *
      *  @param d - Указатель на исходный буфер
      *  @param s - Число байт для записи
      *  @return true
      */
      inline bool write( const char* d, size_t s ) {
        eosio::check( _end - _pos >= (int32_t)s, "datastream attempted to write past the end" );
        memcpy( (void*)_pos, d, s );
        _pos += s;
        return true;
      }

     /**
      *  Записывает один байт в поток из буфера
      *
      *  @param d - Байт для записи
      *  @return true
      */
      inline bool write( char d ) {
        eosio::check( _end - _pos >= 1, "datastream attempted to write past the end" );
        *_pos++ = d;
        return true;
      }

     /**
      *  Записывает заданное число байт в поток из буфера
      *
      *  @param d - Указатель на исходный буфер
      *  @param s - Число байт для записи
      *  @return true
      */
      inline bool write( const void* d, size_t s ) {
        eosio::check( _end - _pos >= (int32_t)s, "datastream attempted to write past the end" );
        memcpy( (void*)_pos, d, s );
        _pos += s;
        return true;
      }

     /**
      *  Записывает байт в поток
      *
      *  @param c Байт для записи
      *  @return true
      */
      inline bool put(char c) {
        eosio::check( _pos < _end, "put" );
        *_pos = c;
        ++_pos;
        return true;
      }

     /**
      *  Читает байт из потока
      *
      *  @param c - Ссылка на байт назначения
      *  @return true
      */
      inline bool get( unsigned char& c ) { return get( *(char*)&c ); }

     /**
      *  Читает байт из потока
      *
      *  @param c - Ссылка на байт назначения
      *  @return true
      */
      inline bool get( char& c )
      {
        eosio::check( _pos < _end, "get" );
        c = *_pos;
        ++_pos;
        return true;
      }

     /**
      *  Возвращает текущую позицию в потоке
      *
      *  @return T - Текущая позиция в потоке
      */
      T pos()const { return _pos; }
      inline bool valid()const { return _pos <= _end && _pos >= _start;  }

     /**
      *  Устанавливает позицию в текущем потоке
      *
      *  @param p - Смещение относительно начала
      *  @return true, если p в пределах диапазона
      *  @return false, если p вне диапазона
      */
      inline bool seekp(size_t p) { _pos = _start + p; return _pos <= _end; }

     /**
      *  Возвращает позицию в текущем потоке
      *
      *  @return p - Позиция в текущем потоке
      */
      inline size_t tellp()const      { return size_t(_pos - _start); }

     /**
      *  Возвращает число оставшихся байт, доступных для чтения/пропуска
      *
      *  @return size_t - Число оставшихся байт
      */
      inline size_t remaining()const  { return _end - _pos; }
    private:
      /**
       * Начальная позиция буфера
       */
      T _start;
      /**
       * Текущая позиция буфера
       */
      T _pos;
      /**
       * Конечная позиция буфера
       */
      T _end;
};

/**
 * Специализация datastream для определения итогового размера сериализованного значения
 */
template<>
class datastream<size_t> {
   public:
      /**
       * Создаёт новый специализированный объект потока данных с заданным начальным размером
       *
       * @param init_size - Начальный размер
       */
     datastream( size_t init_size = 0):_size(init_size){}

     /**
      *  Увеличивает размер на s. Ведёт себя так же, как write( const char* ,size_t s ).
      *
      *  @param s - Величина увеличения размера
      *  @return true
      */
     inline bool     skip( size_t s )                 { _size += s; return true;  }

     /**
      *  Увеличивает размер на s. Ведёт себя так же, как skip( size_t s )
      *
      *  @param s - Величина увеличения размера
      *  @return true
      */
     inline bool     write( const char* ,size_t s )  { _size += s; return true;  }

     /**
      *  Увеличивает размер на s. Ведёт себя так же, как skip( size_t s )
      *
      *  @param s - Величина увеличения размера
      *  @return true
      */
     inline bool     write( char )  { _size++; return true;  }

     /**
      *  Увеличивает размер на s. Ведёт себя так же, как skip( size_t s )
      *
      *  @param s - Величина увеличения размера
      *  @return true
      */
     inline bool     write( const void* ,size_t s )  { _size += s; return true;  }

     /**
      *  Увеличивает размер на единицу
      *
      *  @return true
      */
     inline bool     put(char )                      { ++_size; return  true;    }

     /**
      *  Проверка допустимости. Всегда допустимо
      *
      *  @return true
      */
     inline bool     valid()const                     { return true;              }

     /**
      * Устанавливает новый размер
      *
      * @param p - Новый размер
      * @return true
      */
     inline bool     seekp(size_t p)                  { _size = p;  return true;  }

     /**
      * Возвращает размер
      *
      * @return size_t - Размер
      */
     inline size_t   tellp()const                     { return _size;             }

     /**
      * Всегда возвращает 0
      *
      * @return size_t - 0
      */
     inline size_t   remaining()const                 { return 0;                 }
  private:
     /**
      * Размер, используемый для определения итогового размера сериализованного значения.
      */
     size_t _size;
};

/**
 *  Сериализует std::list в поток
 *
 *  @param ds - Поток для записи
 *  @param opt - Значение для сериализации
 *  @tparam Stream - Тип буфера потока данных
 *  @return datastream<Stream>& - Ссылка на поток данных
 */
template<typename Stream, typename T>
inline datastream<Stream>& operator<<(datastream<Stream>& ds, const std::list<T>& l) {
   ds << unsigned_int( l.size() );
   for ( const auto& elem : l )
      ds << elem;
  return ds;
}

/**
 *  Десериализует std::list из потока
 *
 *  @param ds - Поток для чтения
 *  @param opt - Назначение для десериализованного значения
 *  @tparam Stream - Тип буфера потока данных
 *  @return datastream<Stream>& - Ссылка на поток данных
 */
template<typename Stream, typename T>
inline datastream<Stream>& operator>>(datastream<Stream>& ds, std::list<T>& l) {
   unsigned_int s;
   ds >> s;
   l.resize(s.value);
   for( auto& i : l )
      ds >> i;
   return ds;
}

/**
 *  Сериализует std::deque в поток
 *
 *  @param ds - Поток для записи
 *  @param opt - Значение для сериализации
 *  @tparam Stream - Тип буфера потока данных
 *  @return datastream<Stream>& - Ссылка на поток данных
 */
template<typename Stream, typename T>
inline datastream<Stream>& operator<<(datastream<Stream>& ds, const std::deque<T>& d) {
   ds << unsigned_int( d.size() );
   for ( const auto& elem : d )
      ds << elem;
  return ds;
}

/**
 *  Десериализует std::deque из потока
 *
 *  @param ds - Поток для чтения
 *  @param opt - Назначение для десериализованного значения
 *  @tparam Stream - Тип буфера потока данных
 *  @return datastream<Stream>& - Ссылка на поток данных
 */
template<typename Stream, typename T>
inline datastream<Stream>& operator>>(datastream<Stream>& ds, std::deque<T>& d) {
   unsigned_int s;
   ds >> s;
   d.resize(s.value);
   for( auto& i : d )
      ds >> i;
   return ds;
}

/**
 *  Сериализует std::variant в поток
 *
 *  @param ds - Поток для записи
 *  @param opt - Значение для сериализации
 *  @tparam Stream - Тип буфера потока данных
 *  @return datastream<Stream>& - Ссылка на поток данных
 */
template<typename Stream, typename... Ts>
inline datastream<Stream>& operator<<(datastream<Stream>& ds, const std::variant<Ts...>& var) {
  unsigned_int index = var.index();
  ds << index;
  std::visit([&ds](auto& val){ ds << val; }, var);
  return ds;
}

template<int I, typename Stream, typename... Ts>
void deserialize(datastream<Stream>& ds, std::variant<Ts...>& var, int i) {
   if constexpr (I < std::variant_size_v<std::variant<Ts...>>) {
      if (i == I) {
         std::variant_alternative_t<I, std::variant<Ts...>> tmp;
         ds >> tmp;
         var.template emplace<I>(std::move(tmp));
      } else {
         deserialize<I+1>(ds,var,i);
      }
   } else {
      eosio::check(false, "invalid variant index");
   }
}

/**
 *  Десериализует std::variant из потока
 *
 *  @param ds - Поток для чтения
 *  @param opt - Назначение для десериализованного значения
 *  @tparam Stream - Тип буфера потока данных
 *  @return datastream<Stream>& - Ссылка на поток данных
 */
template<typename Stream, typename... Ts>
inline datastream<Stream>& operator>>(datastream<Stream>& ds, std::variant<Ts...>& var) {
  unsigned_int index;
  ds >> index;
  deserialize<0>(ds,var,index);
  return ds;
}

/**
 *  Сериализует std::pair
 *
 *  @param ds - Поток для записи
 *  @param t - Значение для сериализации
 *  @tparam Stream - Тип буфера потока данных
 *  @tparam Args - Типы элементов кортежа
 *  @return datastream<Stream>& - Ссылка на поток данных
 */
template<typename Stream, typename T1, typename T2>
datastream<Stream>& operator<<( datastream<Stream>& ds, const std::pair<T1, T2>& t ) {
   ds << std::get<0>(t);
   ds << std::get<1>(t);
   return ds;
}

/**
 *  Десериализует std::pair
 *
 *  @param ds - Поток для чтения
 *  @param t - Назначение для десериализованного значения
 *  @tparam Stream - Тип буфера потока данных
 *  @tparam Args - Типы элементов кортежа
 *  @return datastream<Stream>& - Ссылка на поток данных
 */
template<typename Stream, typename T1, typename T2>
datastream<Stream>& operator>>( datastream<Stream>& ds, std::pair<T1, T2>& t ) {
   T1 t1;
   T2 t2;
   ds >> t1;
   ds >> t2;
   t = std::pair<T1, T2>{t1, t2};
   return ds;
}

/**
 *  Сериализует optional в поток
 *
 *  @param ds - Поток для записи
 *  @param opt - Значение для сериализации
 *  @tparam Stream - Тип буфера потока данных
 *  @return datastream<Stream>& - Ссылка на поток данных
 */
template<typename Stream, typename T>
inline datastream<Stream>& operator<<(datastream<Stream>& ds, const std::optional<T>& opt) {
  char valid = opt.has_value();
  ds << valid;
  if (valid)
     ds << *opt;
  return ds;
}

/**
 *  Десериализует optional из потока
 *
 *  @param ds - Поток для чтения
 *  @param opt - Назначение для десериализованного значения
 *  @tparam Stream - Тип буфера потока данных
 *  @return datastream<Stream>& - Ссылка на поток данных
 */
template<typename Stream, typename T>
inline datastream<Stream>& operator>>(datastream<Stream>& ds, std::optional<T>& opt) {
  char valid = 0;
  ds >> valid;
  if (valid) {
     T val;
     ds >> val;
     opt = val;
  }
  return ds;
}


/**
 *  Сериализует bool в поток
 *
 *  @param ds - Поток для чтения
 *  @param d - Значение для сериализации
 *  @tparam Stream - Тип буфера потока данных
 *  @return datastream<Stream>& - Ссылка на поток данных
 */
template<typename Stream>
inline datastream<Stream>& operator<<(datastream<Stream>& ds, const bool& d) {
  return ds << uint8_t(d);
}

/**
 *  Десериализует bool из потока
 *
 *  @param ds - Поток для чтения
 *  @param d - Назначение для десериализованного значения
 *  @tparam Stream - Тип буфера потока данных
 *  @return datastream<Stream>& - Ссылка на поток данных
 */
template<typename Stream>
inline datastream<Stream>& operator>>(datastream<Stream>& ds, bool& d) {
  uint8_t t;
  ds >> t;
  d = t;
  return ds;
}

/**
 *  Сериализует строку в поток
 *
 *  @param ds - Поток для записи
 *  @param v - Значение для сериализации
 *  @tparam Stream - Тип буфера потока данных
 *  @return datastream<Stream>& - Ссылка на поток данных
 */
template<typename Stream>
datastream<Stream>& operator << ( datastream<Stream>& ds, const std::string& v ) {
   ds << unsigned_int( v.size() );
   if (v.size())
      ds.write(v.data(), v.size());
   return ds;
}

/**
 *  Десериализует строку из потока
 *
 *  @param ds - Поток для чтения
 *  @param v - Назначение для десериализованного значения
 *  @tparam Stream - Тип буфера потока данных
 *  @return datastream<Stream>& - Ссылка на поток данных
 */
template<typename Stream>
datastream<Stream>& operator >> ( datastream<Stream>& ds, std::string& v ) {
   std::vector<char> tmp;
   ds >> tmp;
   if( tmp.size() )
      v = std::string(tmp.data(),tmp.data()+tmp.size());
   else
      v = std::string();
   return ds;
}

/**
 *  Сериализует std::array фиксированного размера
 *
 *  @param ds - Поток для записи
 *  @param v - Значение для сериализации
 *  @tparam Stream - Тип буфера потока данных
 *  @tparam T - Тип элементов массива
 *  @tparam N - Размер массива
 *  @return datastream<Stream>& - Ссылка на поток данных
 */
template<typename Stream, typename T, std::size_t N>
datastream<Stream>& operator << ( datastream<Stream>& ds, const std::array<T,N>& v ) {
   for( const auto& i : v )
      ds << i;
   return ds;
}


/**
 *  Десериализует std::array фиксированного размера
 *
 *  @param ds - Поток для чтения
 *  @param v - Назначение для десериализованного значения
 *  @tparam Stream - Тип буфера потока данных
 *  @tparam T - Тип элементов массива
 *  @tparam N - Размер массива
 *  @return datastream<Stream>& - Ссылка на поток данных
 */
template<typename Stream, typename T, std::size_t N>
datastream<Stream>& operator >> ( datastream<Stream>& ds, std::array<T,N>& v ) {
   for( auto& i : v )
      ds >> i;
   return ds;
}

namespace _datastream_detail {
   /**
    * Проверяет, является ли тип T указателем
    *
    * @tparam T - Проверяемый тип
    * @return true, если T — указатель
    * @return false в противном случае
    */
   template<typename T>
   constexpr bool is_pointer() {
      return std::is_pointer<T>::value ||
             std::is_null_pointer<T>::value ||
             std::is_member_pointer<T>::value;
   }

   /**
    * Проверяет, является ли тип T примитивным
    *
    * @tparam T - Проверяемый тип
    * @return true, если T — примитивный тип
    * @return false в противном случае
    */
   template<typename T>
   constexpr bool is_primitive() {
      return std::is_arithmetic<T>::value ||
             std::is_enum<T>::value;
   }

   /*
    * Проверяет, является ли T специализацией datastream
    *
    * @tparam T - Проверяемый тип
    */
   template<typename T>
   struct is_datastream { static constexpr bool value = false; };
   template<typename T>
   struct is_datastream<datastream<T>> { static constexpr bool value = true; };
}

/**
 *  Десериализует указатель
 *
 *  @brief Указатель не должен сериализоваться; эта перегрузка всегда приводит к ошибке
 *  @param ds - Поток для чтения
 *  @tparam Stream - Тип буфера потока данных
 *  @tparam T - Тип указателя
 *  @return datastream<Stream>& - Ссылка на поток данных
 *  @post Выбрасывает исключение, если это указатель
 */
template<typename Stream, typename T, std::enable_if_t<_datastream_detail::is_pointer<T>()>* = nullptr>
datastream<Stream>& operator >> ( datastream<Stream>& ds, T ) {
   static_assert(!_datastream_detail::is_pointer<T>(), "Pointers should not be serialized" );
   return ds;
}

/**
 *  Сериализует C-массив фиксированного размера из непримитивного и не указательного типа
 *
 *  @param ds - Поток для записи
 *  @param v - Значение для сериализации
 *  @tparam Stream - Тип буфера потока данных
 *  @tparam T - Тип элемента массива
 *  @return datastream<Stream>& - Ссылка на поток данных
 */
template<typename Stream, typename T, std::size_t N,
         std::enable_if_t<!_datastream_detail::is_primitive<T>() &&
                          !_datastream_detail::is_pointer<T>()>* = nullptr>
datastream<Stream>& operator << ( datastream<Stream>& ds, const T (&v)[N] ) {
   ds << unsigned_int( N );
   for( uint32_t i = 0; i < N; ++i )
      ds << v[i];
   return ds;
}

/**
 *  Сериализует C-массив фиксированного размера примитивного типа
 *
 *  @param ds - Поток для записи
 *  @param v - Значение для сериализации
 *  @tparam Stream - Тип буфера потока данных
 *  @tparam T - Тип элемента массива
 *  @return datastream<Stream>& - Ссылка на поток данных
 */
template<typename Stream, typename T, std::size_t N,
         std::enable_if_t<_datastream_detail::is_primitive<T>()>* = nullptr>
datastream<Stream>& operator << ( datastream<Stream>& ds, const T (&v)[N] ) {
   ds << unsigned_int( N );
   ds.write((char*)&v[0], sizeof(v));
   return ds;
}

/**
 *  Десериализует C-массив фиксированного размера из непримитивного и не указательного типа
 *
 *  @param ds - Поток для чтения
 *  @param v - Назначение для десериализованного значения
 *  @tparam T - Тип элементов массива
 *  @tparam N - Размер массива
 *  @tparam Stream - Тип буфера потока данных
 *  @return datastream<Stream>& - Ссылка на поток данных
 */
template<typename Stream, typename T, std::size_t N,
         std::enable_if_t<!_datastream_detail::is_primitive<T>() &&
                          !_datastream_detail::is_pointer<T>()>* = nullptr>
datastream<Stream>& operator >> ( datastream<Stream>& ds, T (&v)[N] ) {
   unsigned_int s;
   ds >> s;
   eosio::check( N == s.value, "T[] size and unpacked size don't match");
   for( uint32_t i = 0; i < N; ++i )
      ds >> v[i];
   return ds;
}

/**
 *  Десериализует C-массив фиксированного размера примитивного типа
 *
 *  @param ds - Поток для чтения
 *  @param v - Назначение для десериализованного значения
 *  @tparam T - Тип элементов массива
 *  @tparam N - Размер массива
 *  @tparam Stream - Тип буфера потока данных
 *  @return datastream<Stream>& - Ссылка на поток данных
 */
template<typename Stream, typename T, std::size_t N,
         std::enable_if_t<_datastream_detail::is_primitive<T>()>* = nullptr>
datastream<Stream>& operator >> ( datastream<Stream>& ds, T (&v)[N] ) {
   unsigned_int s;
   ds >> s;
   eosio::check( N == s.value, "T[] size and unpacked size don't match");
   ds.read((char*)&v[0], sizeof(v));
   return ds;
}

/**
 *  Сериализует vector из T, где T — примитивный тип данных
 *
 *  @param ds - Поток для записи
 *  @param v - Значение для сериализации
 *  @tparam Stream - Тип буфера потока данных
 *  @return datastream<Stream>& - Ссылка на поток данных
 */
template<typename Stream, typename T, 
	std::enable_if_t<_datastream_detail::is_primitive<T>()>* = nullptr>
datastream<Stream>& operator << ( datastream<Stream>& ds, const std::vector<T>& v ) {
   ds << unsigned_int( v.size() );
   ds.write( (const void*)v.data(), v.size()*sizeof(T) );
   return ds;
}

/**
 *  Сериализует vector
 *
 *  @param ds - Поток для записи
 *  @param v - Значение для сериализации
 *  @tparam Stream - Тип буфера потока данных
 *  @tparam T - Тип элементов vector
 *  @return datastream<Stream>& - Ссылка на поток данных
 */
template<typename Stream, typename T,
	std::enable_if_t<!_datastream_detail::is_primitive<T>()>* = nullptr>
datastream<Stream>& operator << ( datastream<Stream>& ds, const std::vector<T>& v ) {
   ds << unsigned_int( v.size() );
   for( const auto& i : v )
      ds << i;
   return ds;
}

/**
 *  Десериализует vector из T, где T — примитивный тип данных
 *
 *  @param ds - Поток для чтения
 *  @param v - Назначение для десериализованного значения
 *  @tparam Stream - Тип буфера потока данных
 *  @return datastream<Stream>& - Ссылка на поток данных
 */
template<typename Stream, typename T,
	std::enable_if_t<_datastream_detail::is_primitive<T>()>* = nullptr>
datastream<Stream>& operator >> ( datastream<Stream>& ds, std::vector<T>& v ) {
   unsigned_int s;
   ds >> s;
   v.resize( s.value );
   ds.read( (char*)v.data(), v.size()*sizeof(T) );
   return ds;
}

/**
 *  Десериализует vector
 *
 *  @param ds - Поток для чтения
 *  @param v - Назначение для десериализованного значения
 *  @tparam Stream - Тип буфера потока данных
 *  @tparam T - Тип элементов vector
 *  @return datastream<Stream>& - Ссылка на поток данных
 */
template<typename Stream, typename T,
	std::enable_if_t<!_datastream_detail::is_primitive<T>()>* = nullptr>
datastream<Stream>& operator >> ( datastream<Stream>& ds, std::vector<T>& v ) {
   unsigned_int s;
   ds >> s;
   v.resize(s.value);
   for( auto& i : v )
      ds >> i;
   return ds;
}

/**
 *  Сериализует basic_string<T>
 *
 *  @param ds - Поток для записи
 *  @param s - Значение для сериализации
 *  @tparam Stream - Тип буфера потока данных
 *  @tparam T - Тип элементов basic_string
 *  @return datastream<Stream>& - Ссылка на поток данных
 */
template<typename Stream, typename T>
datastream<Stream>& operator << ( datastream<Stream>& ds, const std::basic_string<T>& s ) {
   ds << unsigned_int(s.size());
   if (s.size())
      ds.write(s.data(), s.size()*sizeof(T));
   return ds;
}

/**
 *  Десериализует basic_string<T>
 *
 *  @param ds - Поток для чтения
 *  @param s - Назначение для десериализованного значения
 *  @tparam Stream - Тип буфера потока данных
 *  @tparam T - Тип элементов basic_string
 *  @return datastream<Stream>& - Ссылка на поток данных
 */
template<typename Stream, typename T>
datastream<Stream>& operator >> ( datastream<Stream>& ds, std::basic_string<T>& s ) {
   unsigned_int v;
   ds >> v;
   s.resize(v.value);
   ds.read(s.data(), s.size()*sizeof(T));
   return ds;
}

/**
 *  Сериализует basic_string<uint8_t>
 *
 *  @param ds - Поток для записи
 *  @param s - Значение для сериализации
 *  @tparam Stream - Тип буфера потока данных
 *  @tparam T - Тип элементов basic_string
 *  @return datastream<Stream>& - Ссылка на поток данных
 */
template<typename Stream>
datastream<Stream>& operator << ( datastream<Stream>& ds, const std::basic_string<uint8_t>& s ) {
   ds << unsigned_int(s.size());
   if (s.size())
      ds.write(s.data(), s.size());
   return ds;
}

/**
 *  Десериализует basic_string<uint8_t>
 *
 *  @param ds - Поток для чтения
 *  @param s - Назначение для десериализованного значения
 *  @tparam Stream - Тип буфера потока данных
 *  @tparam T - Тип элементов basic_string
 *  @return datastream<Stream>& - Ссылка на поток данных
 */
template<typename Stream>
datastream<Stream>& operator >> ( datastream<Stream>& ds, std::basic_string<uint8_t>& s ) {
   unsigned_int v;
   ds >> v;
   s.resize(v.value);
   ds.read(s.data(), s.size());
   return ds;
}


/**
 *  Сериализует std::set
 *
 *  @param ds - Поток для записи
 *  @param s - Значение для сериализации
 *  @tparam Stream - Тип буфера потока данных
 *  @tparam T - Тип элементов множества
 *  @return datastream<Stream>& - Ссылка на поток данных
 */
template<typename Stream, typename T>
datastream<Stream>& operator << ( datastream<Stream>& ds, const std::set<T>& s ) {
   ds << unsigned_int( s.size() );
   for( const auto& i : s ) {
      ds << i;
   }
   return ds;
}


/**
 *  Десериализует std::set
 *
 *  @param ds - Поток для чтения
 *  @param s - Назначение для десериализованного значения
 *  @tparam Stream - Тип буфера потока данных
 *  @tparam T - Тип элементов множества
 *  @return datastream<Stream>& - Ссылка на поток данных
 */
template<typename Stream, typename T>
datastream<Stream>& operator >> ( datastream<Stream>& ds, std::set<T>& s ) {
   s.clear();
   unsigned_int sz; ds >> sz;

   for( uint32_t i = 0; i < sz.value; ++i ) {
      T v;
      ds >> v;
      s.emplace( std::move(v) );
   }
   return ds;
}

/**
 *  Сериализует std::map
 *
 *  @param ds - Поток для записи
 *  @param m - Значение для сериализации
 *  @tparam Stream - Тип буфера потока данных
 *  @tparam K - Тип ключа в map
 *  @tparam V - Тип значения в map
 *  @return datastream<Stream>& - Ссылка на поток данных
 */
template<typename Stream, typename K, typename V>
datastream<Stream>& operator << ( datastream<Stream>& ds, const std::map<K,V>& m ) {
   ds << unsigned_int( m.size() );
   for( const auto& i : m ) {
      ds << i.first << i.second;
   }
   return ds;
}

/**
 *  Десериализует std::map
 *
 *  @param ds - Поток для чтения
 *  @param m - Назначение для десериализованного значения
 *  @tparam Stream - Тип буфера потока данных
 *  @tparam K - Тип ключа в map
 *  @tparam V - Тип значения в map
 *  @return datastream<Stream>& - Ссылка на поток данных
 */
template<typename Stream, typename K, typename V>
datastream<Stream>& operator >> ( datastream<Stream>& ds, std::map<K,V>& m ) {
   m.clear();
   unsigned_int s; ds >> s;

   for (uint32_t i = 0; i < s.value; ++i) {
      K k; V v;
      ds >> k >> v;
      m.emplace( std::move(k), std::move(v) );
   }
   return ds;
}

/**
 *  Сериализует std::tuple
 *
 *  @param ds - Поток для записи
 *  @param t - Значение для сериализации
 *  @tparam Stream - Тип буфера потока данных
 *  @tparam Args - Типы элементов кортежа
 *  @return datastream<Stream>& - Ссылка на поток данных
 */
template<typename Stream, typename... Args>
datastream<Stream>& operator<<( datastream<Stream>& ds, const std::tuple<Args...>& t ) {
   bluegrass::meta::for_each( t, [&]( const auto& i ) {
       ds << i;
   });
   return ds;
}

/**
 *  Десериализует std::tuple
 *
 *  @param ds - Поток для чтения
 *  @param t - Назначение для десериализованного значения
 *  @tparam Stream - Тип буфера потока данных
 *  @tparam Args - Типы элементов кортежа
 *  @return datastream<Stream>& - Ссылка на поток данных
 */
template<typename Stream, typename... Args>
datastream<Stream>& operator>>( datastream<Stream>& ds, std::tuple<Args...>& t ) {
   bluegrass::meta::for_each( t, [&]( auto& i ) {
       ds >> i;
   });
   return ds;
}

/**
 *  Сериализует класс
 *
 *  @param ds - Поток для записи
 *  @param v - Значение для сериализации
 *  @tparam DataStream - Тип потока данных
 *  @tparam T - Тип класса
 *  @return DataStream& - Ссылка на поток данных
 */
template<typename DataStream, typename T, std::enable_if_t<std::is_class<T>::value && _datastream_detail::is_datastream<DataStream>::value>* = nullptr>
DataStream& operator<<( DataStream& ds, const T& v ) {
   bluegrass::meta::for_each_field(v, [&](const auto& field) {
      ds << field;
   });
   return ds;
}

/**
 *  Десериализует класс
 *
 *  @param ds - Поток для чтения
 *  @param v - Назначение для десериализованного значения
 *  @tparam DataStream - Тип потока данных
 *  @tparam T - Тип класса
 *  @return DataStream& - Ссылка на поток данных
 */
template<typename DataStream, typename T, std::enable_if_t<std::is_class<T>::value && _datastream_detail::is_datastream<DataStream>::value>* = nullptr>
DataStream& operator>>( DataStream& ds, T& v ) {
   bluegrass::meta::for_each_field(v, [&](auto& field) {
      ds >> field;
   });
   return ds;
}

/**
 *  Сериализует примитивный тип
 *
 *  @param ds - Поток для записи
 *  @param v - Значение для сериализации
 *  @tparam Stream - Тип буфера потока данных
 *  @tparam T - Примитивный тип
 *  @return datastream<Stream>& - Ссылка на поток данных
 */
template<typename Stream, typename T, std::enable_if_t<_datastream_detail::is_primitive<T>()>* = nullptr>
datastream<Stream>& operator<<( datastream<Stream>& ds, const T& v ) {
   ds.write( (const char*)&v, sizeof(T) );
   return ds;
}

/**
 *  Десериализует примитивный тип
 *
 *  @param ds - Поток для чтения
 *  @param v - Назначение для десериализованного значения
 *  @tparam Stream - Тип буфера потока данных
 *  @tparam T - Примитивный тип
 *  @return datastream<Stream>& - Ссылка на поток данных
 */
template<typename Stream, typename T, std::enable_if_t<_datastream_detail::is_primitive<T>()>* = nullptr>
datastream<Stream>& operator>>( datastream<Stream>& ds, T& v ) {
   ds.read( (char*)&v, sizeof(T) );
   return ds;
}

/**
 * Распаковывает данные из буфера фиксированного размера как T
 *
 * @ingroup datastream
 * @tparam T - Тип распакованных данных
 * @param buffer - Указатель на буфер
 * @param len - Длина буфера
 * @return T - Распакованные данные
 */
template<typename T>
T unpack( const char* buffer, size_t len ) {
   T result;
   datastream<const char*> ds(buffer,len);
   ds >> result;
   return result;
}

/**
 * Распаковывает данные из буфера фиксированного размера как T
 *
 * @ingroup datastream
 * @tparam T - Тип распакованных данных
 * @param res - Переменная для заполнения результатом распаковки
 * @param buffer - Указатель на буфер
 * @param len - Длина буфера
 */
template<typename T>
void unpack( T& res, const char* buffer, size_t len ) {
   datastream<const char*> ds(buffer,len);
   ds >> res;
}

/**
 * Распаковывает данные из буфера переменного размера как T
 *
 * @ingroup datastream
 * @tparam T - Тип распакованных данных
 * @param bytes - Буфер
 * @return T - Распакованные данные
 */
template<typename T>
T unpack( const std::vector<char>& bytes ) {
   return unpack<T>( bytes.data(), bytes.size() );
}

/**
 * Возвращает размер упакованных данных
 *
 * @ingroup datastream
 * @tparam T - Тип упаковываемых данных
 * @param value - Данные для упаковки
 * @return size_t - Размер упакованных данных
 */
template<typename T>
size_t pack_size( const T& value ) {
  datastream<size_t> ps;
  ps << value;
  return ps.tellp();
}

/**
 * Возвращает упакованные данные
 *
 * @ingroup datastream
 * @tparam T - Тип упаковываемых данных
 * @param value - Данные для упаковки
 * @return Упакованные данные (байты)
 */
template<typename T>
std::vector<char> pack( const T& value ) {
  std::vector<char> result;
  result.resize(pack_size(value));

  datastream<char*> ds( result.data(), result.size() );
  ds << value;
  return result;
}
}
