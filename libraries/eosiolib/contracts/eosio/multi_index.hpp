/**
 * @file
 * @copyright как определено в eos/LICENSE
 */
#pragma once

#include "../../contracts/eosio/action.hpp"
#include "../../core/eosio/name.hpp"
#include "../../core/eosio/serialize.hpp"
#include "../../core/eosio/fixed_bytes.hpp"

#include <bluegrass/meta/for_each.hpp>

#include <vector>
#include <tuple>
#include <functional>
#include <utility>
#include <type_traits>
#include <iterator>
#include <limits>
#include <algorithm>
#include <memory>

/**
 * @defgroup multiindex Многоиндексная таблица (Multi Index)
 * @ingroup contracts
 */

namespace eosio {
  namespace internal_use_do_not_use {
    extern "C" {
      __attribute__((eosio_wasm_import))
      int32_t db_store_i64(uint64_t, uint64_t, uint64_t, uint64_t,  const void*, uint32_t);

      __attribute__((eosio_wasm_import))
      void db_update_i64(int32_t, uint64_t, const void*, uint32_t);

      __attribute__((eosio_wasm_import))
      void db_remove_i64(int32_t);

      __attribute__((eosio_wasm_import))
      int32_t db_get_i64(int32_t, const void*, uint32_t);

      __attribute__((eosio_wasm_import))
      int32_t db_next_i64(int32_t, uint64_t*);

      __attribute__((eosio_wasm_import))
      int32_t db_previous_i64(int32_t, uint64_t*);

      __attribute__((eosio_wasm_import))
      int32_t db_find_i64(uint64_t, uint64_t, uint64_t, uint64_t);

      __attribute__((eosio_wasm_import))
      int32_t db_lowerbound_i64(uint64_t, uint64_t, uint64_t, uint64_t);

      __attribute__((eosio_wasm_import))
      int32_t db_upperbound_i64(uint64_t, uint64_t, uint64_t, uint64_t);

      __attribute__((eosio_wasm_import))
      int32_t db_end_i64(uint64_t, uint64_t, uint64_t);

      __attribute__((eosio_wasm_import))
      int32_t db_idx64_store(uint64_t, uint64_t, uint64_t, uint64_t, const uint64_t*);

      __attribute__((eosio_wasm_import))
      void db_idx64_update(int32_t, uint64_t, const uint64_t*);

      __attribute__((eosio_wasm_import))
      void db_idx64_remove(int32_t);

      __attribute__((eosio_wasm_import))
      int32_t db_idx64_next(int32_t, uint64_t*);

      __attribute__((eosio_wasm_import))
      int32_t db_idx64_previous(int32_t, uint64_t*);

      __attribute__((eosio_wasm_import))
      int32_t db_idx64_find_primary(uint64_t, uint64_t, uint64_t, uint64_t*, uint64_t);

      __attribute__((eosio_wasm_import))
      int32_t db_idx64_find_secondary(uint64_t, uint64_t, uint64_t, const uint64_t*, uint64_t*);

      __attribute__((eosio_wasm_import))
      int32_t db_idx64_lowerbound(uint64_t, uint64_t, uint64_t, uint64_t*, uint64_t*);

      __attribute__((eosio_wasm_import))
      int32_t db_idx64_upperbound(uint64_t, uint64_t, uint64_t, uint64_t*, uint64_t*);

      __attribute__((eosio_wasm_import))
      int32_t db_idx64_end(uint64_t, uint64_t, uint64_t);

      __attribute__((eosio_wasm_import))
      int32_t db_idx128_store(uint64_t, uint64_t, uint64_t, uint64_t, const uint128_t*);

      __attribute__((eosio_wasm_import))
      void db_idx128_update(int32_t, uint64_t, const uint128_t*);

      __attribute__((eosio_wasm_import))
      void db_idx128_remove(int32_t);

      __attribute__((eosio_wasm_import))
      int32_t db_idx128_next(int32_t, uint64_t*);

      __attribute__((eosio_wasm_import))
      int32_t db_idx128_previous(int32_t, uint64_t*);

      __attribute__((eosio_wasm_import))
      int32_t db_idx128_find_primary(uint64_t, uint64_t, uint64_t, uint128_t*, uint64_t);

      __attribute__((eosio_wasm_import))
      int32_t db_idx128_find_secondary(uint64_t, uint64_t, uint64_t, const uint128_t*, uint64_t*);

      __attribute__((eosio_wasm_import))
      int32_t db_idx128_lowerbound(uint64_t, uint64_t, uint64_t, uint128_t*, uint64_t*);

      __attribute__((eosio_wasm_import))
      int32_t db_idx128_upperbound(uint64_t, uint64_t, uint64_t, uint128_t*, uint64_t*);

      __attribute__((eosio_wasm_import))
      int32_t db_idx128_end(uint64_t, uint64_t, uint64_t);

      __attribute__((eosio_wasm_import))
      int32_t db_idx256_store(uint64_t, uint64_t, uint64_t, uint64_t, const uint128_t*, uint32_t);

      __attribute__((eosio_wasm_import))
      void db_idx256_update(int32_t, uint64_t, const uint128_t*, uint32_t);

      __attribute__((eosio_wasm_import))
      void db_idx256_remove(int32_t);

      __attribute__((eosio_wasm_import))
      int32_t db_idx256_next(int32_t, uint64_t*);

      __attribute__((eosio_wasm_import))
      int32_t db_idx256_previous(int32_t, uint64_t*);

      __attribute__((eosio_wasm_import))
      int32_t db_idx256_find_primary(uint64_t, uint64_t, uint64_t, uint128_t*, uint32_t, uint64_t);

      __attribute__((eosio_wasm_import))
      int32_t db_idx256_find_secondary(uint64_t, uint64_t, uint64_t, const uint128_t*, uint32_t, uint64_t*);

      __attribute__((eosio_wasm_import))
      int32_t db_idx256_lowerbound(uint64_t, uint64_t, uint64_t, uint128_t*, uint32_t, uint64_t*);

      __attribute__((eosio_wasm_import))
      int32_t db_idx256_upperbound(uint64_t, uint64_t, uint64_t, uint128_t*, uint32_t, uint64_t*);

      __attribute__((eosio_wasm_import))
      int32_t db_idx256_end(uint64_t, uint64_t, uint64_t);

      __attribute__((eosio_wasm_import))
      int32_t db_idx_double_store(uint64_t, uint64_t, uint64_t, uint64_t, const double*);

      __attribute__((eosio_wasm_import))
      void db_idx_double_update(int32_t, uint64_t, const double*);

      __attribute__((eosio_wasm_import))
      void db_idx_double_remove(int32_t);

      __attribute__((eosio_wasm_import))
      int32_t db_idx_double_next(int32_t, uint64_t*);

      __attribute__((eosio_wasm_import))
      int32_t db_idx_double_previous(int32_t, uint64_t*);

      __attribute__((eosio_wasm_import))
      int32_t db_idx_double_find_primary(uint64_t, uint64_t, uint64_t, double*, uint64_t);

      __attribute__((eosio_wasm_import))
      int32_t db_idx_double_find_secondary(uint64_t, uint64_t, uint64_t, const double*, uint64_t*);

      __attribute__((eosio_wasm_import))
      int32_t db_idx_double_lowerbound(uint64_t, uint64_t, uint64_t, double*, uint64_t*);

      __attribute__((eosio_wasm_import))
      int32_t db_idx_double_upperbound(uint64_t, uint64_t, uint64_t, double*, uint64_t*);

      __attribute__((eosio_wasm_import))
      int32_t db_idx_double_end(uint64_t, uint64_t, uint64_t);

      __attribute__((eosio_wasm_import))
      int32_t db_idx_long_double_store(uint64_t, uint64_t, uint64_t, uint64_t, const long double*);

      __attribute__((eosio_wasm_import))
      void db_idx_long_double_update(int32_t, uint64_t, const long double*);

      __attribute__((eosio_wasm_import))
      void db_idx_long_double_remove(int32_t);

      __attribute__((eosio_wasm_import))
      int32_t db_idx_long_double_next(int32_t, uint64_t*);

      __attribute__((eosio_wasm_import))
      int32_t db_idx_long_double_previous(int32_t, uint64_t*);

      __attribute__((eosio_wasm_import))
      int32_t db_idx_long_double_find_primary(uint64_t, uint64_t, uint64_t, long double*, uint64_t);

      __attribute__((eosio_wasm_import))
      int32_t db_idx_long_double_find_secondary(uint64_t, uint64_t, uint64_t, const long double*, uint64_t*);

      __attribute__((eosio_wasm_import))
      int32_t db_idx_long_double_lowerbound(uint64_t, uint64_t, uint64_t, long double*, uint64_t*);

      __attribute__((eosio_wasm_import))
      int32_t db_idx_long_double_upperbound(uint64_t, uint64_t, uint64_t, long double*, uint64_t*);

      __attribute__((eosio_wasm_import))
      int32_t db_idx_long_double_end(uint64_t, uint64_t, uint64_t);
    }
  };

constexpr static inline name same_payer{};

template<class Class,typename Type,Type (Class::*PtrToMemberFunction)()const>
struct const_mem_fun
{
  typedef typename std::remove_reference<Type>::type result_type;

  template<typename ChainedPtr>

  auto operator()(const ChainedPtr& x)const -> std::enable_if_t<!std::is_convertible<const ChainedPtr&, const Class&>::value, Type>
  {
    return operator()(*x);
  }

  Type operator()(const Class& x)const
  {
    return (x.*PtrToMemberFunction)();
  }

  Type operator()(const std::reference_wrapper<const Class>& x)const
  {
    return operator()(x.get());
  }

  Type operator()(const std::reference_wrapper<Class>& x)const
  {
    return operator()(x.get());
  }
};

#define WRAP_SECONDARY_SIMPLE_TYPE(IDX, TYPE)\
template<>\
struct secondary_index_db_functions<TYPE> {\
   static int32_t db_idx_next( int32_t iterator, uint64_t* primary )          { return internal_use_do_not_use::db_##IDX##_next( iterator, primary ); } \
   static int32_t db_idx_previous( int32_t iterator, uint64_t* primary )      { return internal_use_do_not_use::db_##IDX##_previous( iterator, primary ); } \
   static void    db_idx_remove( int32_t iterator  )                          { internal_use_do_not_use::db_##IDX##_remove( iterator ); } \
   static int32_t db_idx_end( uint64_t code, uint64_t scope, uint64_t table ) { return internal_use_do_not_use::db_##IDX##_end( code, scope, table ); } \
   static int32_t db_idx_store( uint64_t scope, uint64_t table, uint64_t payer, uint64_t id, const TYPE& secondary ) {\
     return internal_use_do_not_use::db_##IDX##_store( scope, table, payer, id, &secondary ); \
   }\
   static void    db_idx_update( int32_t iterator, uint64_t payer, const TYPE& secondary ) {\
     internal_use_do_not_use::db_##IDX##_update( iterator, payer, &secondary ); \
   }\
   static int32_t db_idx_find_primary( uint64_t code, uint64_t scope, uint64_t table, uint64_t primary, TYPE& secondary ) {\
     return internal_use_do_not_use::db_##IDX##_find_primary( code, scope, table, &secondary, primary ); \
   }\
   static int32_t db_idx_find_secondary( uint64_t code, uint64_t scope, uint64_t table, const TYPE& secondary, uint64_t& primary ) {\
     return internal_use_do_not_use::db_##IDX##_find_secondary( code, scope, table, &secondary, &primary ); \
   }\
   static int32_t db_idx_lowerbound( uint64_t code, uint64_t scope, uint64_t table, TYPE& secondary, uint64_t& primary ) {\
     return internal_use_do_not_use::db_##IDX##_lowerbound( code, scope, table, &secondary, &primary ); \
   }\
   static int32_t db_idx_upperbound( uint64_t code, uint64_t scope, uint64_t table, TYPE& secondary, uint64_t& primary ) {\
     return internal_use_do_not_use::db_##IDX##_upperbound( code, scope, table, &secondary, &primary ); \
   }\
};

#define WRAP_SECONDARY_ARRAY_TYPE(IDX, TYPE)\
template<>\
struct secondary_index_db_functions<TYPE> {\
   static int32_t db_idx_next( int32_t iterator, uint64_t* primary )          { return internal_use_do_not_use::db_##IDX##_next( iterator, primary ); } \
   static int32_t db_idx_previous( int32_t iterator, uint64_t* primary )      { return internal_use_do_not_use::db_##IDX##_previous( iterator, primary ); } \
   static void    db_idx_remove( int32_t iterator )                           { internal_use_do_not_use::db_##IDX##_remove( iterator ); } \
   static int32_t db_idx_end( uint64_t code, uint64_t scope, uint64_t table ) { return internal_use_do_not_use::db_##IDX##_end( code, scope, table ); } \
   static int32_t db_idx_store( uint64_t scope, uint64_t table, uint64_t payer, uint64_t id, const TYPE& secondary ) {\
     return internal_use_do_not_use::db_##IDX##_store( scope, table, payer, id, secondary.data(), TYPE::num_words() ); \
   }\
   static void    db_idx_update( int32_t iterator, uint64_t payer, const TYPE& secondary ) {\
     internal_use_do_not_use::db_##IDX##_update( iterator, payer, secondary.data(), TYPE::num_words() ); \
   }\
   static int32_t db_idx_find_primary( uint64_t code, uint64_t scope, uint64_t table, uint64_t primary, TYPE& secondary ) {\
     return internal_use_do_not_use::db_##IDX##_find_primary( code, scope, table, secondary.data(), TYPE::num_words(), primary ); \
   }\
   static int32_t db_idx_find_secondary( uint64_t code, uint64_t scope, uint64_t table, const TYPE& secondary, uint64_t& primary ) {\
     return internal_use_do_not_use::db_##IDX##_find_secondary( code, scope, table, secondary.data(), TYPE::num_words(), &primary ); \
   }\
   static int32_t db_idx_lowerbound( uint64_t code, uint64_t scope, uint64_t table, TYPE& secondary, uint64_t& primary ) {\
     return internal_use_do_not_use::db_##IDX##_lowerbound( code, scope, table, secondary.data(), TYPE::num_words(), &primary ); \
   }\
   static int32_t db_idx_upperbound( uint64_t code, uint64_t scope, uint64_t table, TYPE& secondary, uint64_t& primary ) {\
     return internal_use_do_not_use::db_##IDX##_upperbound( code, scope, table, secondary.data(), TYPE::num_words(), &primary ); \
   }\
};

#define MAKE_TRAITS_FOR_ARITHMETIC_SECONDARY_KEY(TYPE)\
template<>\
struct secondary_key_traits<TYPE> {\
   static_assert( std::numeric_limits<TYPE>::is_specialized, "TYPE does not have specialized numeric_limits" );\
   static constexpr TYPE true_lowest() { return std::numeric_limits<TYPE>::lowest(); }\
};

namespace _multi_index_detail {

   template<typename T>
   struct secondary_index_db_functions;

   template<typename T>
   struct secondary_key_traits;

   WRAP_SECONDARY_SIMPLE_TYPE(idx64,  uint64_t)
   MAKE_TRAITS_FOR_ARITHMETIC_SECONDARY_KEY(uint64_t)

   WRAP_SECONDARY_SIMPLE_TYPE(idx128, uint128_t)
   MAKE_TRAITS_FOR_ARITHMETIC_SECONDARY_KEY(uint128_t)

   WRAP_SECONDARY_SIMPLE_TYPE(idx_double, double)
   template<>
   struct secondary_key_traits<double> {
      static constexpr double true_lowest() { return -std::numeric_limits<double>::infinity(); }
   };

   WRAP_SECONDARY_SIMPLE_TYPE(idx_long_double, long double)
   template<>
   struct secondary_key_traits<long double> {
      static constexpr long double true_lowest() { return -std::numeric_limits<long double>::infinity(); }
   };

   WRAP_SECONDARY_ARRAY_TYPE(idx256, eosio::fixed_bytes<32>)
   template<>
   struct secondary_key_traits<eosio::fixed_bytes<32>> {
      static constexpr eosio::fixed_bytes<32> true_lowest() { return eosio::fixed_bytes<32>(); }
   };

   template<typename PK>
   inline uint64_t to_raw_key(PK pk) { return pk; }
   inline uint64_t to_raw_key(eosio::name pk) { return pk.value; }

}

/**
 * Структура indexed_by используется для задания индексов многоиндексной таблицы. В COOPOS можно задать до 16 вторичных индексов.
 *
 * @ingroup multiindex
 * @tparam IndexName — имя индекса. Имя задаётся как 64-битное целое в кодировке base32 COOPOS и должно соответствовать правилам имён COOPOS: не более 13 символов; первые двенадцать — строчные буквы a–z, цифры 1–5 и «.»; если есть 13-й символ, допускаются только строчные a–p и «.».
 * @tparam Extractor — функтор (оператор вызова функции), принимающий const-ссылку на тип объекта строки таблицы и возвращающий тип вторичного ключа или ссылку на него. Рекомендуется использовать шаблон `eosio::const_mem_fun`.
 *
 * Пример:
 *
 *
 * @code
 * #include <eosiolib/eosio.hpp>
 * using namespace eosio;
 * class mycontract: eosio::contract {
 *   struct record {
 *      uint64_t    primary;
 *      uint128_t   secondary;
 *      uint64_t primary_key() const { return primary; }
 *      uint128_t get_secondary() const { return secondary; }
 *    };
 *   public:
 *     mycontract(name receiver, name code, datastream<const char*> ds):contract(receiver, code, ds){}
 *     void myaction() {
 *       auto code = _self;
 *       auto scope = _self;
 *       multi_index<"mytable"_n, record,
 *                  indexed_by< "bysecondary"_n, const_mem_fun<record, uint128_t, &record::get_secondary> > > table( code, scope);
 *     }
 * }
 * EOSIO_DISPATCH( mycontract, (myaction) )
 * @endcode
 */
template<name::raw IndexName, typename Extractor>
struct indexed_by {
   enum constants { index_name   = static_cast<uint64_t>(IndexName) };
   typedef Extractor secondary_extractor_type;
};

/**
 * @ingroup multiindex
 *
 * @brief Определяет многоиндексную таблицу COOPOS (Multi Index Table)
 * @details API многоиндексных таблиц COOPOS даёт C++-интерфейс к базе данных COOPOS и устроен по образцу Boost Multi Index Container.
 * Многоиндексная таблица COOPOS требует ровно один первичный ключ типа uint64_t. Чтобы таблица могла получать первичный ключ,
 * у хранимого в ней объекта должна быть const-функция-член primary_key(), возвращающая uint64_t.
 * Многоиндексная таблица COOPOS также поддерживает до 16 вторичных индексов. Тип вторичного индекса может быть одним из:
 * - uint64_t
 * - uint128_t
 * - double
 * - long double
 * - eosio::checksum256
 *
 * @tparam TableName — имя таблицы
 * @tparam T — тип данных, хранимых в таблице
 * @tparam Indices — вторичные индексы таблицы; поддерживается до 16 индексов
 *
 * Пример:
 *
 * @code
 * #include <eosiolib/eosio.hpp>
 * using namespace eosio;
 * class mycontract: contract {
 *   struct record {
 *     uint64_t    primary;
 *     uint64_t    secondary_1;
 *     uint128_t   secondary_2;
 *     checksum256 secondary_3;
 *     double      secondary_4;
 *     long double secondary_5;
 *     uint64_t primary_key() const { return primary; }
 *     uint64_t get_secondary_1() const { return secondary_1; }
 *     uint128_t get_secondary_2() const { return secondary_2; }
 *     checksum256 get_secondary_3() const { return secondary_3; }
 *     double get_secondary_4() const { return secondary_4; }
 *     long double get_secondary_5() const { return secondary_5; }
 *   };
 *   public:
 *     mycontract(name receiver, name code, datastream<const char*> ds):contract(receiver, code, ds){}
 *     void myaction() {
 *       auto code = _self;
 *       auto scope = _self;
 *       multi_index<"mytable"_n, record,
 *         indexed_by< "bysecondary1"_n, const_mem_fun<record, uint64_t, &record::get_secondary_1> >,
 *         indexed_by< "bysecondary2"_n, const_mem_fun<record, uint128_t, &record::get_secondary_2> >,
 *         indexed_by< "bysecondary3"_n, const_mem_fun<record, checksum256, &record::get_secondary_3> >,
 *         indexed_by< "bysecondary4"_n, const_mem_fun<record, double, &record::get_secondary_4> >,
 *         indexed_by< "bysecondary5"_n, const_mem_fun<record, long double, &record::get_secondary_5> >
 *       > table( code, scope);
 *     }
 * }
 * EOSIO_DISPATCH( mycontract, (myaction) )
 * @endcode
 */

template<name::raw TableName, typename T, typename... Indices>
class multi_index
{
   private:

      static_assert( std::is_same_v<decltype(_multi_index_detail::to_raw_key(std::declval<T>().primary_key())), uint64_t>,
                     "Primary key must be uint64_t or name" );
      static_assert( sizeof...(Indices) <= 16, "multi_index only supports a maximum of 16 secondary indices" );

      constexpr static bool validate_table_name( name n ) {
         // Limit table names to 12 characters so that the last character (4 bits) can be used to distinguish between the secondary indices.
         return n.length() < 13; //(n & 0x000000000000000FULL) == 0;
      }

      constexpr static size_t max_stack_buffer_size = 512;

      static_assert( validate_table_name( name(TableName) ), "multi_index does not support table names with a length greater than 12");

      name     _code;
      uint64_t _scope;

      mutable uint64_t _next_primary_key;

      enum next_primary_key_tags : uint64_t {
         no_available_primary_key = static_cast<uint64_t>(-2), // Must be the smallest uint64_t value compared to all other tags
         unset_next_primary_key = static_cast<uint64_t>(-1)
      };

      struct item : public T
      {
         template<typename Constructor>
         item( const multi_index* idx, Constructor&& c )
         :__idx(idx){
            c(*this);
         }

         const multi_index* __idx;
         int32_t            __primary_itr;
         int32_t            __iters[sizeof...(Indices)+(sizeof...(Indices)==0)];
      };

      struct item_ptr
      {
         item_ptr(std::unique_ptr<item>&& i, uint64_t pk, int32_t pitr)
         : _item(std::move(i)), _primary_key(pk), _primary_itr(pitr) {}

         std::unique_ptr<item> _item;
         uint64_t              _primary_key;
         int32_t               _primary_itr;
      };

      mutable std::vector<item_ptr> _items_vector;

      template<name::raw IndexName, typename Extractor, uint64_t Number, bool IsConst>
      struct index {
         public:
            typedef Extractor  secondary_extractor_type;
            typedef typename std::decay<decltype( Extractor()(nullptr) )>::type secondary_key_type;

            constexpr static bool validate_index_name( eosio::name n ) {
               return n.value != 0 && n != eosio::name("primary"); // Primary is a reserve index name.
            }

            static_assert( validate_index_name( name(IndexName) ), "invalid index name used in multi_index" );

            enum constants {
               table_name   = static_cast<uint64_t>(TableName),
               index_name   = static_cast<uint64_t>(IndexName),
               index_number = Number,
               index_table_name = (static_cast<uint64_t>(TableName) & 0xFFFFFFFFFFFFFFF0ULL)
                                    | (Number & 0x000000000000000FULL) // Assuming no more than 16 secondary indices are allowed
            };

            constexpr static uint64_t name()   { return index_table_name; }
            constexpr static uint64_t number() { return Number; }

            struct const_iterator : public std::iterator<std::bidirectional_iterator_tag, const T> {
               public:
                  friend bool operator == ( const const_iterator& a, const const_iterator& b ) {
                     return a._item == b._item;
                  }
                  friend bool operator != ( const const_iterator& a, const const_iterator& b ) {
                     return a._item != b._item;
                  }

                  const T& operator*()const { return *static_cast<const T*>(_item); }
                  const T* operator->()const { return static_cast<const T*>(_item); }

                  const_iterator operator++(int){
                     const_iterator result(*this);
                     ++(*this);
                     return result;
                  }

                  const_iterator operator--(int){
                     const_iterator result(*this);
                     --(*this);
                     return result;
                  }

                  const_iterator& operator++() {
                     using namespace _multi_index_detail;

                     eosio::check( _item != nullptr, "cannot increment end iterator" );

                     if( _item->__iters[Number] == -1 ) {
                        secondary_key_type temp_secondary_key;
                        auto idxitr = secondary_index_db_functions<secondary_key_type>::db_idx_find_primary(_idx->get_code().value, _idx->get_scope(), _idx->name(), _item->primary_key(), temp_secondary_key);
                        auto& mi = const_cast<item&>( *_item );
                        mi.__iters[Number] = idxitr;
                     }

                     uint64_t next_pk = 0;
                     auto next_itr = secondary_index_db_functions<secondary_key_type>::db_idx_next( _item->__iters[Number], &next_pk );
                     if( next_itr < 0 ) {
                        _item = nullptr;
                        return *this;
                     }

                     const T& obj = *_idx->_multidx->find( next_pk );
                     auto& mi = const_cast<item&>( static_cast<const item&>(obj) );
                     mi.__iters[Number] = next_itr;
                     _item = &mi;

                     return *this;
                  }

                  const_iterator& operator--() {
                     using namespace _multi_index_detail;

                     uint64_t prev_pk = 0;
                     int32_t  prev_itr = -1;

                     if( !_item ) {
                        auto ei = secondary_index_db_functions<secondary_key_type>::db_idx_end(_idx->get_code().value, _idx->get_scope(), _idx->name());
                        eosio::check( ei != -1, "cannot decrement end iterator when the index is empty" );
                        prev_itr = secondary_index_db_functions<secondary_key_type>::db_idx_previous( ei , &prev_pk );
                        eosio::check( prev_itr >= 0, "cannot decrement end iterator when the index is empty" );
                     } else {
                        if( _item->__iters[Number] == -1 ) {
                           secondary_key_type temp_secondary_key;
                           auto idxitr = secondary_index_db_functions<secondary_key_type>::db_idx_find_primary(_idx->get_code().value, _idx->get_scope(), _idx->name(), _item->primary_key(), temp_secondary_key);
                           auto& mi = const_cast<item&>( *_item );
                           mi.__iters[Number] = idxitr;
                        }
                        prev_itr = secondary_index_db_functions<secondary_key_type>::db_idx_previous( _item->__iters[Number], &prev_pk );
                        eosio::check( prev_itr >= 0, "cannot decrement iterator at beginning of index" );
                     }

                     const T& obj = *_idx->_multidx->find( prev_pk );
                     auto& mi = const_cast<item&>( static_cast<const item&>(obj) );
                     mi.__iters[Number] = prev_itr;
                     _item = &mi;

                     return *this;
                  }

                  const_iterator():_item(nullptr){}
               private:
                  friend struct index;
                  const_iterator( const index* idx, const item* i = nullptr )
                  : _idx(idx), _item(i) {}

                  const index* _idx;
                  const item*  _item;
            }; /// struct multi_index::index::const_iterator

            typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

            const_iterator cbegin()const {
               using namespace _multi_index_detail;
               return lower_bound( secondary_key_traits<secondary_key_type>::true_lowest() );
            }
            const_iterator begin()const  { return cbegin(); }

            const_iterator cend()const   { return const_iterator( this ); }
            const_iterator end()const    { return cend(); }

            const_reverse_iterator crbegin()const { return std::make_reverse_iterator(cend()); }
            const_reverse_iterator rbegin()const  { return crbegin(); }

            const_reverse_iterator crend()const   { return std::make_reverse_iterator(cbegin()); }
            const_reverse_iterator rend()const    { return crend(); }

            const_iterator find( secondary_key_type&& secondary )const {
               return find( secondary );
            }

            const_iterator find( const secondary_key_type& secondary )const {
               auto lb = lower_bound( secondary );
               auto e = cend();
               if( lb == e ) return e;

               if( secondary != secondary_extractor_type()(*lb) )
                  return e;
               return lb;
            }

            const_iterator require_find( secondary_key_type&& secondary, const char* error_msg = "unable to find secondary key" )const {
               return require_find( secondary, error_msg );
            }

            const_iterator require_find( const secondary_key_type& secondary, const char* error_msg = "unable to find secondary key" )const {
               auto lb = lower_bound( secondary );
               eosio::check( lb != cend(), error_msg );
               eosio::check( secondary == secondary_extractor_type()(*lb), error_msg );
               return lb;
            }

            /**
             * Возвращает объект с наименьшим первичным ключом, если вторичный ключ не уникален.
             * 
             * Избегайте типичной ошибки: присвоить возвращённую ссылку T& локальной переменной на стеке
             * и передать её в modify многоиндексной таблицы.
             * Часто ошибка в том, что локальная переменная объявлена как auto без ссылки;
             * нужно использовать auto& или decltype(auto).
             */
            const T& get( secondary_key_type&& secondary, const char* error_msg = "unable to find secondary key" )const {
               return get( secondary, error_msg );
            }

            /**
             * Возвращает объект с наименьшим первичным ключом, если вторичный ключ не уникален.
             * 
             * Избегайте типичной ошибки: присвоить возвращённую ссылку T& локальной переменной на стеке
             * и передать её в modify многоиндексной таблицы.
             * Часто ошибка в том, что локальная переменная объявлена как auto без ссылки;
             * нужно использовать auto& или decltype(auto).
             */
            const T& get( const secondary_key_type& secondary, const char* error_msg = "unable to find secondary key" )const {
               auto result = find( secondary );
               eosio::check( result != cend(), error_msg );
               return *result;
            }

            const_iterator lower_bound( secondary_key_type&& secondary )const {
               return lower_bound( secondary );
            }
            const_iterator lower_bound( const secondary_key_type& secondary )const {
               using namespace _multi_index_detail;

               uint64_t primary = 0;
               secondary_key_type secondary_copy(secondary);
               auto itr = secondary_index_db_functions<secondary_key_type>::db_idx_lowerbound( get_code().value, get_scope(), name(), secondary_copy, primary );
               if( itr < 0 ) return cend();

               const T& obj = *_multidx->find( primary );
               auto& mi = const_cast<item&>( static_cast<const item&>(obj) );
               mi.__iters[Number] = itr;

               return {this, &mi};
            }

            const_iterator upper_bound( secondary_key_type&& secondary )const {
               return upper_bound( secondary );
            }
            const_iterator upper_bound( const secondary_key_type& secondary )const {
               using namespace _multi_index_detail;

               uint64_t primary = 0;
               secondary_key_type secondary_copy(secondary);
               auto itr = secondary_index_db_functions<secondary_key_type>::db_idx_upperbound( get_code().value, get_scope(), name(), secondary_copy, primary );
               if( itr < 0 ) return cend();

               const T& obj = *_multidx->find( primary );
               auto& mi = const_cast<item&>( static_cast<const item&>(obj) );
               mi.__iters[Number] = itr;

               return {this, &mi};
            }
            /**
             * Предупреждение: iterator_to может вести себя неопределённо, если вызывающий код
             * передаёт ссылку на объект на стеке вместо ссылки, возвращённой get
             * или полученной разыменованием const_iterator.
             */
            const_iterator iterator_to( const T& obj ) {
               using namespace _multi_index_detail;

               const auto& objitem = static_cast<const item&>(obj);
               eosio::check( objitem.__idx == _multidx, "object passed to iterator_to is not in multi_index" );

               if( objitem.__iters[Number] == -1 ) {
                  secondary_key_type temp_secondary_key;
                  auto idxitr = secondary_index_db_functions<secondary_key_type>::db_idx_find_primary(get_code().value, get_scope(), name(), objitem.primary_key(), temp_secondary_key);
                  auto& mi = const_cast<item&>( objitem );
                  mi.__iters[Number] = idxitr;
               }

               return {this, &objitem};
            }

            template<typename Lambda>
            void modify( const_iterator itr, eosio::name payer, Lambda&& updater ) {
               eosio::check( itr != cend(), "cannot pass end iterator to modify" );

               _multidx->modify( *itr, payer, std::forward<Lambda&&>(updater) );
            }

            template<typename Lambda>
            void modify( const T& obj, eosio::name payer, Lambda&& updater ) {
               _multidx->modify( obj, payer, std::forward<Lambda&&>(updater) );
            }

            const_iterator erase( const_iterator itr ) {
               eosio::check( itr != cend(), "cannot pass end iterator to erase" );

               const auto& obj = *itr;
               ++itr;

               _multidx->erase(obj);

               return itr;
            }

            eosio::name get_code()const  { return _multidx->get_code(); }
            uint64_t    get_scope()const { return _multidx->get_scope(); }

            static auto extract_secondary_key(const T& obj) { return secondary_extractor_type()(obj); }

            // used only for type deduction
            constexpr index() : _multidx(nullptr) { }
         private:
            friend class multi_index;

            index( typename std::conditional<IsConst, const multi_index*, multi_index*>::type midx )
            :_multidx(midx){}

            typename std::conditional<IsConst, const multi_index*, multi_index*>::type _multidx;
      }; /// struct multi_index::index

      template<uint64_t I>
      struct intc { enum e{ value = I }; operator uint64_t()const{ return I; }  };
      enum index_cv { const_index = 0, mutable_index = 1 };

      template<std::size_t Num, typename... Values>
      class make_index_tuple {

         template <std::size_t... Seq>
         static constexpr auto get_type(std::index_sequence<Seq...>) {
            return std::make_tuple(std::make_tuple(index<eosio::name::raw(static_cast<uint64_t>(Values::index_name)),
                                                         typename Values::secondary_extractor_type,
                                                         intc<Seq>::e::value, const_index>{},
                                                   index<eosio::name::raw(static_cast<uint64_t>(Values::index_name)),
                                                         typename Values::secondary_extractor_type,
                                                         intc<Seq>::e::value, mutable_index>{})...);
         }
      public:
         using type = decltype( get_type(std::make_index_sequence<Num>{}) );
      };

      using indices_type = typename make_index_tuple<sizeof... (Indices), Indices...>::type;
      
      class make_extractor_tuple {
         template <typename Obj, typename IndicesType, std::size_t... Seq>
         static constexpr auto extractor_tuple(IndicesType, const Obj& obj, std::index_sequence<Seq...>) {
            return std::make_tuple(std::tuple_element_t<const_index, std::tuple_element_t<Seq, IndicesType>>::extract_secondary_key(obj)...);
         }
      public:
         template <typename Obj, typename IndicesType>
         static constexpr auto get_extractor_tuple(IndicesType, const Obj& obj) {
            return extractor_tuple(IndicesType{}, obj, std::make_index_sequence<std::tuple_size_v<IndicesType>>{});
         }
      };

      const item& load_object_by_primary_iterator( int32_t itr )const {
         using namespace _multi_index_detail;

         auto itr2 = std::find_if(_items_vector.rbegin(), _items_vector.rend(), [&](const item_ptr& ptr) {
            return ptr._primary_itr == itr;
         });
         if( itr2 != _items_vector.rend() )
            return *itr2->_item;

         auto size = internal_use_do_not_use::db_get_i64( itr, nullptr, 0 );
         eosio::check( size >= 0, "error reading iterator" );

         //using malloc/free here potentially is not exception-safe, although WASM doesn't support exceptions
         void* buffer = max_stack_buffer_size < size_t(size) ? malloc(size_t(size)) : alloca(size_t(size));

         internal_use_do_not_use::db_get_i64( itr, buffer, uint32_t(size) );

         datastream<const char*> ds( (char*)buffer, uint32_t(size) );

         auto itm = std::make_unique<item>( this, [&]( auto& i ) {
            T& val = static_cast<T&>(i);
            ds >> val;

            i.__primary_itr = itr;
            bluegrass::meta::for_each(indices_type{}, [&](auto idx){
               typedef std::tuple_element_t<const_index, decltype(idx)> index_type;
               i.__iters[ index_type::number() ] = -1;
            });
         });

         const item* ptr = itm.get();
         auto pk   = _multi_index_detail::to_raw_key(itm->primary_key());
         auto pitr = itm->__primary_itr;

         _items_vector.emplace_back( std::move(itm), pk, pitr );

         if ( max_stack_buffer_size < size_t(size) ) {
            free(buffer);
         }

         return *ptr;
      } /// load_object_by_primary_iterator

   public:
      /**
       * Создаёт экземпляр многоиндексной таблицы.
       * @ingroup multiindex
       *
       * @param code — аккаунт-владелец таблицы
       * @param scope — идентификатор области (scope) в иерархии кода
       *
       * @pre свойства code и scope заданы
       * @post каждая таблица вторичного индекса инициализирована
       * @post Вторичные индексы обновлены для ссылки на новый объект; при отсутствии таблиц вторичных индексов они создаются.
       * @post С плательщика взимается хранение нового объекта и, при необходимости создания таблицы (и таблиц вторичных индексов), накладные расходы на создание.
       *
       * Замечания
       * Шаблон `eosio::multi_index` имеет параметры `<name::raw TableName, typename T, typename... Indices>`, где:
       * - `TableName` — имя таблицы, не более 12 символов из набора строчных букв, цифр 1–5 и «.»; преобразуется в eosio::raw (обёртка над uint64_t);
       * - `T` — тип объекта (определение строки);
       * - `Indices` — список до 16 вторичных индексов.
       * - Каждый элемент должен быть классом или структурой с конструктором по умолчанию
       * - У каждого должен быть оператор вызова, принимающий const-ссылку на тип объекта строки и возвращающий вторичный ключ или ссылку на него
       * - Рекомендуется шаблон eosio::const_mem_fun
       *
       * Пример:
       *
       * @code
       * #include <eosiolib/eosio.hpp>
       * using namespace eosio;
       * using namespace std;
       * class addressbook: contract {
       *   struct address {
       *      uint64_t account_name;
       *      string first_name;
       *      string last_name;
       *      string street;
       *      string city;
       *      string state;
       *      uint64_t primary_key() const { return account_name; }
       *   };
       *   public:
       *     addressbook(name self):contract(self) {}
       *     typedef eosio::multi_index< "address"_n, address > address_index;
       *     void myaction() {
       *       address_index addresses(_self, _self.value); // code, scope
       *     }
       * }
       * EOSIO_DISPATCH( addressbook, (myaction) )
       * @endcode
       */
      multi_index( name code, uint64_t scope )
      :_code(code),_scope(scope),_next_primary_key(unset_next_primary_key)
      {}

      /**
       * Возвращает свойство-член `code`.
       * @ingroup multiindex
       *
       * @return Имя аккаунта кода, которому принадлежит первичная таблица.
       *
       * Пример:
       *
       * @code
       * // This assumes the code from the constructor example. Replace myaction() {...}
       *
       *     void myaction() {
       *       address_index addresses("dan"_n, "dan"_n.value); // code, scope
       *       eosio::check(addresses.get_code() == "dan"_n, "Codes don't match.");
       *     }
       * }
       * EOSIO_DISPATCH( addressbook, (myaction) )
       * @endcode
       */
      name get_code()const      { return _code; }

      /**
       * Возвращает свойство-член `scope`.
       * @ingroup multiindex
       *
       * @return Идентификатор области (scope) в коде текущего получателя, под которым находится нужный экземпляр первичной таблицы.
       *
       * Пример:
       *
       * @code
       * // This assumes the code from the constructor example. Replace myaction() {...}
       *
       *     void myaction() {
       *       address_index addresses("dan"_n, "dan"_n.value); // code, scope
       *       eosio::check(addresses.get_scope() == "dan"_n.value, "Scopes don't match");
       *     }
       * }
       * EOSIO_DISPATCH( addressbook, (myaction) )
       * @endcode
       */
      uint64_t get_scope()const { return _scope; }

      struct const_iterator : public std::iterator<std::bidirectional_iterator_tag, const T> {
         friend bool operator == ( const const_iterator& a, const const_iterator& b ) {
            return a._item == b._item;
         }
         friend bool operator != ( const const_iterator& a, const const_iterator& b ) {
            return a._item != b._item;
         }

         const T& operator*()const { return *static_cast<const T*>(_item); }
         const T* operator->()const { return static_cast<const T*>(_item); }

         const_iterator operator++(int) {
            const_iterator result(*this);
            ++(*this);
            return result;
         }

         const_iterator operator--(int) {
            const_iterator result(*this);
            --(*this);
            return result;
         }

         const_iterator& operator++() {
            eosio::check( _item != nullptr, "cannot increment end iterator" );

            uint64_t next_pk;
            auto next_itr = internal_use_do_not_use::db_next_i64( _item->__primary_itr, &next_pk );
            if( next_itr < 0 )
               _item = nullptr;
            else
               _item = &_multidx->load_object_by_primary_iterator( next_itr );
            return *this;
         }
         const_iterator& operator--() {
            uint64_t prev_pk;
            int32_t  prev_itr = -1;

            if( !_item ) {
               auto ei = internal_use_do_not_use::db_end_i64(_multidx->get_code().value, _multidx->get_scope(), static_cast<uint64_t>(TableName));
               eosio::check( ei != -1, "cannot decrement end iterator when the table is empty" );
               prev_itr = internal_use_do_not_use::db_previous_i64( ei , &prev_pk );
               eosio::check( prev_itr >= 0, "cannot decrement end iterator when the table is empty" );
            } else {
               prev_itr = internal_use_do_not_use::db_previous_i64( _item->__primary_itr, &prev_pk );
               eosio::check( prev_itr >= 0, "cannot decrement iterator at beginning of table" );
            }

            _item = &_multidx->load_object_by_primary_iterator( prev_itr );
            return *this;
         }

         private:
            const_iterator( const multi_index* mi, const item* i = nullptr )
            :_multidx(mi),_item(i){}

            const multi_index* _multidx;
            const item*        _item;
            friend class multi_index;
      }; /// struct multi_index::const_iterator

      typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

      /**
       * Возвращает итератор на объект с наименьшим значением первичного ключа в многоиндексной таблице.
       * @ingroup multiindex
       *
       * @return Итератор на объект с наименьшим первичным ключом в многоиндексной таблице.
       *
       * Пример:
       *
       * @code
       * // This assumes the code from the constructor example. Replace myaction() {...}
       *
       *     void myaction() {
       *       // create reference to address_index  - see emplace example below
       *       // add dan account to table           - see emplace example below
       *
       *       auto itr = addresses.find("dan"_n);
       *       eosio::check(itr == addresses.cbegin(), "Only address is not at front.");
       *     }
       * }
       * EOSIO_DISPATCH( addressbook, (myaction) )
       * @endcode
       */
      const_iterator cbegin()const {
         return lower_bound(std::numeric_limits<uint64_t>::lowest());
      }

      /**
       * Возвращает итератор на объект с наименьшим значением первичного ключа в многоиндексной таблице.
       * @ingroup multiindex
       *
       * @return Итератор на объект с наименьшим первичным ключом в многоиндексной таблице.
       *
       * Пример:
       *
       * @code
       * // This assumes the code from the constructor example. Replace myaction() {...}
       *
       *     void myaction() {
       *       // create reference to address_index  - see emplace example below
       *       // add dan account to table           - see emplace example below
       *
       *       auto itr = addresses.find("dan"_n);
       *       eosio::check(itr == addresses.begin(), "Only address is not at front.");
       *     }
       * }
       * EOSIO_ABI( addressbook, (myaction) )
       * @endcode
       */
      const_iterator begin()const  { return cbegin(); }

      /**
       * Возвращает итератор на элемент «после последнего» в контейнере multi_index. Такой элемент — теоретический следующий за последним; на реальный объект не указывает и разыменовывать его нельзя.
       * @ingroup multiindex
       *
       * @return Итератор на позицию «после последнего» в контейнере multi_index.
       *
       * Пример:
       *
       * @code
       * // This assumes the code from the constructor example. Replace myaction() {...}
       *
       *     void myaction() {
       *       // create reference to address_index  - see emplace example below
       *       // add dan account to table           - see emplace example below
       *
       *       auto itr = addresses.find("dan"_n);
       *       eosio::check(itr != addresses.cend(), "Address for account doesn't exist");
       *     }
       * }
       * EOSIO_DISPATCH( addressbook, (myaction) )
       * @endcode
       */
      const_iterator cend()const   { return const_iterator( this ); }

      /**
       * Возвращает итератор на элемент «после последнего» в контейнере multi_index. Такой элемент — теоретический следующий за последним; на реальный объект не указывает и разыменовывать его нельзя.
       * @ingroup multiindex
       *
       * @return Итератор на позицию «после последнего» в контейнере multi_index.
       *
       * Пример:
       *
       * @code
       * // This assumes the code from the constructor example. Replace myaction() {...}
       *
       *     void myaction() {
       *       // create reference to address_index  - see emplace example below
       *       // add dan account to table           - see emplace example below
       *
       *       auto itr = addresses.find("dan"_n);
       *       eosio::check(itr != addresses.end(), "Address for account doesn't exist");
       *     }
       * }
       * EOSIO_DISPATCH( addressbook, (myaction) )
       * @endcode
       */
      const_iterator end()const    { return cend(); }

      /**
       * Возвращает обратный итератор на объект с наибольшим первичным ключом в многоиндексной таблице.
       * @ingroup multiindex
       *
       * @return Обратный итератор на объект с наибольшим первичным ключом в многоиндексной таблице.
       *
       * Пример:
       *
       * @code
       * // This assumes the code from the constructor example. Replace myaction() {...}
       *
       *     void myaction() {
       *       // create reference to address_index  - see emplace example below
       *       // add dan account to table           - see emplace example below
       *       // add additional account - brendan
       *
       *       addresses.emplace(payer, [&](auto& address) {
       *         address.account_name = "brendan"_n;
       *         address.first_name = "Brendan";
       *         address.last_name = "Blumer";
       *         address.street = "1 EOS Way";
       *         address.city = "Hong Kong";
       *         address.state = "HK";
       *       });
       *       auto itr = addresses.crbegin();
       *       eosio::check(itr->account_name == name("dan"), "Lock arf, Incorrect Last Record ");
       *       itr++;
       *       eosio::check(itr->account_name == name("brendan"), "Lock arf, Incorrect Second Last Record");
       *     }
       * }
       * EOSIO_DISPATCH( addressbook, (myaction) )
       * @endcode
       */
      const_reverse_iterator crbegin()const { return std::make_reverse_iterator(cend()); }

      /**
       * Возвращает обратный итератор на объект с наибольшим первичным ключом в многоиндексной таблице.
       * @ingroup multiindex
       *
       * @return Обратный итератор на объект с наибольшим первичным ключом в многоиндексной таблице.
       *
       * Пример:
       *
       * @code
       * // This assumes the code from the constructor example. Replace myaction() {...}
       *
       *     void myaction() {
       *       // create reference to address_index  - see emplace example below
       *       // add dan account to table           - see emplace example below
       *       // add additional account - brendan
       *
       *       addresses.emplace(payer, [&](auto& address) {
       *         address.account_name = "brendan"_n;
       *         address.first_name = "Brendan";
       *         address.last_name = "Blumer";
       *         address.street = "1 EOS Way";
       *         address.city = "Hong Kong";
       *         address.state = "HK";
       *       });
       *       auto itr = addresses.rbegin();
       *       eosio::check(itr->account_name == name("dan"), "Lock arf, Incorrect Last Record ");
       *       itr++;
       *       eosio::check(itr->account_name == name("brendan"), "Lock arf, Incorrect Second Last Record");
       *     }
       * }
       * EOSIO_DISPATCH( addressbook, (myaction) )
       * @endcode
       */
      const_reverse_iterator rbegin()const  { return crbegin(); }

      /**
       * Возвращает итератор на объект с наименьшим первичным ключом в многоиндексной таблице.
       * @ingroup multiindex
       *
       * @return Итератор на объект с наименьшим первичным ключом в многоиндексной таблице.
       *
       * Пример:
       *
       * @code
       * // This assumes the code from the constructor example. Replace myaction() {...}
       *
       *     void myaction() {
       *       // create reference to address_index  - see emplace example below
       *       // add dan account to table           - see emplace example below
       *       // add additional account - brendan
       *
       *       addresses.emplace(payer, [&](auto& address) {
       *         address.account_name = "brendan"_n;
       *         address.first_name = "Brendan";
       *         address.last_name = "Blumer";
       *         address.street = "1 EOS Way";
       *         address.city = "Hong Kong";
       *         address.state = "HK";
       *       });
       *       auto itr = addresses.crend();
       *       itr--;
       *       eosio::check(itr->account_name == name("brendan"), "Lock arf, Incorrect First Record ");
       *       itr--;
       *       eosio::check(itr->account_name == name("dan"), "Lock arf, Incorrect Second Record");
       *     }
       * }
       * EOSIO_DISPATCH( addressbook, (myaction) )
       * @endcode
       */
      const_reverse_iterator crend()const   { return std::make_reverse_iterator(cbegin()); }

      /**
       * Возвращает итератор на объект с наименьшим первичным ключом в многоиндексной таблице.
       * @ingroup multiindex
       *
       * @return Итератор на объект с наименьшим первичным ключом в многоиндексной таблице.
       *
       * Пример:
       *
       * @code
       * // This assumes the code from the constructor example. Replace myaction() {...}
       *
       *     void myaction() {
       *       // create reference to address_index  - see emplace example below
       *       // add dan account to table           - see emplace example below
       *       // add additional account - brendan
       *
       *       addresses.emplace(payer, [&](auto& address) {
       *         address.account_name = "brendan"_n;
       *         address.first_name = "Brendan";
       *         address.last_name = "Blumer";
       *         address.street = "1 EOS Way";
       *         address.city = "Hong Kong";
       *         address.state = "HK";
       *       });
       *       auto itr = addresses.rend();
       *       itr--;
       *       eosio::check(itr->account_name == name("brendan"), "Lock arf, Incorrect First Record ");
       *       itr--;
       *       eosio::check(itr->account_name == name("dan"), "Lock arf, Incorrect Second Record");
       *     }
       * }
       * EOSIO_DISPATCH( addressbook, (myaction) )
       * @endcode
       */
      const_reverse_iterator rend()const    { return crend(); }

      /**
       * Ищет объект с наименьшим первичным ключом, который больше или равен заданному первичному ключу.
       * @ingroup multiindex
       *
       * @param primary — первичный ключ, задающий целевое значение для поиска нижней границы
       * @return Итератор на объект с наименьшим первичным ключом ≥ `primary`. Если объект не найден или таблицы нет**, возвращается итератор `end`.
       *
       * Пример:
       *
       * @code
       * // This assumes the code from the get_index() example below. Replace myaction() {...}
       *
       *     void myaction() {
       *       // create reference to address_index  - see emplace example below
       *       // add dan account to table           - see emplace example below
       *       // add additional account - brendan
       *
       *       addresses.emplace(payer, [&](auto& address) {
       *         address.account_name = "brendan"_n;
       *         address.first_name = "Brendan";
       *         address.last_name = "Blumer";
       *         address.street = "1 EOS Way";
       *         address.city = "Hong Kong";
       *         address.state = "HK";
       *         address.zip = 93445;
       *       });
       *       uint32_t zipnumb = 93445;
       *       auto zip_index = addresses.get_index<name("zip")>();
       *       auto itr = zip_index.lower_bound(zipnumb);
       *       eosio::check(itr->account_name == name("brendan"), "Lock arf, Incorrect First Lower Bound Record ");
       *       itr++;
       *       eosio::check(itr->account_name == name("dan"), "Lock arf, Incorrect Second Lower Bound Record");
       *       itr++;
       *       eosio::check(itr == zip_index.end(), "Lock arf, Incorrect End of Iterator");
       *     }
       * }
       * EOSIO_DISPATCH( addressbook, (myaction) )
       * @endcode
       */
      template<typename PK>
      const_iterator lower_bound( PK primary )const {
         uint64_t primary_int = _multi_index_detail::to_raw_key(primary);
         auto itr = internal_use_do_not_use::db_lowerbound_i64( _code.value, _scope, static_cast<uint64_t>(TableName), primary_int );
         if( itr < 0 ) return end();
         const auto& obj = load_object_by_primary_iterator( itr );
         return {this, &obj};
      }

      /**
       * Ищет объект с наименьшим первичным ключом, строго большим заданного первичного ключа.
       * @ingroup multiindex
       *
       * @param primary — первичный ключ, задающий целевое значение для поиска верхней границы
       * @return Итератор на объект с наименьшим первичным ключом, большим `primary`. Если объект не найден или таблицы нет**, возвращается итератор `end`.
       *
       * Пример:
       *
       * @code
       * // This assumes the code from the get_index() example below. Replace myaction() {...}
       *
       *     void myaction() {
       *       // create reference to address_index  - see emplace example below
       *       // add dan account to table           - see emplace example below
       *       // add additional account - brendan
       *
       *       addresses.emplace(payer, [&](auto& address) {
       *         address.account_name = "brendan"_n;
       *         address.first_name = "Brendan";
       *         address.last_name = "Blumer";
       *         address.street = "1 EOS Way";
       *         address.city = "Hong Kong";
       *         address.state = "HK";
       *         address.zip = 93445;
       *       });
       *       uint32_t zipnumb = 93445;
       *       auto zip_index = addresses.get_index<name("zip")>();
       *       auto itr = zip_index.upper_bound(zipnumb);
       *       eosio::check(itr->account_name == name("dan"), "Lock arf, Incorrect First Upper Bound Record ");
       *       itr++;
       *       eosio::check(itr == zip_index.end(), "Lock arf, Incorrect End of Iterator");
       *     }
       * }
       * EOSIO_DISPATCH( addressbook, (myaction) )
       * @endcode
       */
      template<typename PK>
      const_iterator upper_bound( PK primary )const {
         uint64_t primary_int = _multi_index_detail::to_raw_key(primary);
         auto itr = internal_use_do_not_use::db_upperbound_i64( _code.value, _scope, static_cast<uint64_t>(TableName), primary_int );
         if( itr < 0 ) return end();
         const auto& obj = load_object_by_primary_iterator( itr );
         return {this, &obj};
      }

      /**
       * Возвращает свободный первичный ключ.
       * @ingroup multiindex
       *
       * @return Доступное (неиспользованное) значение первичного ключа.
       *
       * Замечания:
       * Предназначено для таблиц, где первичные ключи строго автоинкрементируются и контракт никогда не задаёт произвольные значения. Нарушение этого ожидания может привести к тому, что таблица будет казаться заполненной из‑за невозможности выделить свободный первичный ключ.
       * В идеале метод используют только для выбора первичного ключа новых строк в таблицах, изначально рассчитанных на автоинкремент без произвольных значений со стороны контракта. Нарушение этого соглашения может дать ложное ощущение переполнения при наличии свободного места.
       *
       * Пример:
       *
       * @code
       * // This assumes the code from the constructor example. Replace myaction() {...}
       *
       *     void myaction() {
       *       address_index addresses(_self, _self.value);  // code, scope
       *       // add to table, first argument is account to bill for storage
       *       addresses.emplace(payer, [&](auto& address) {
       *         address.key = addresses.available_primary_key();
       *         address.first_name = "Daniel";
       *         address.last_name = "Larimer";
       *         address.street = "1 EOS Way";
       *         address.city = "Blacksburg";
       *         address.state = "VA";
       *       });
       *     }
       * }
       * EOSIO_DISPATCH( addressbook, (myaction) )
       * @endcode
       */
      uint64_t available_primary_key()const {
         if( _next_primary_key == unset_next_primary_key ) {
            // This is the first time available_primary_key() is called for this multi_index instance.
            if( begin() == end() ) { // empty table
               _next_primary_key = 0;
            } else {
               auto itr = --end(); // last row of table sorted by primary key
               auto pk = itr->primary_key(); // largest primary key currently in table
               if( pk >= no_available_primary_key ) // Reserve the tags
                  _next_primary_key = no_available_primary_key;
               else
                  _next_primary_key = pk + 1;
            }
         }

         eosio::check( _next_primary_key < no_available_primary_key, "next primary key in table is at autoincrement limit");
         return _next_primary_key;
      }

      /**
       * Возвращает вторичный индекс нужного типа.
       * @ingroup multiindex
       *
       * @tparam IndexName — идентификатор нужного вторичного индекса
       *
       * @return Индекс соответствующего типа: 64-битный беззнаковый целочисленный ключ (idx64), 128-битный беззнаковый целочисленный ключ (idx128), 128-битный ключ фиксированной длины в лексикографическом порядке (idx128), 256-битный ключ фиксированной длины (idx256), ключ с плавающей точкой, ключ double, ключ long double (четверная точность)
       *
       * Пример:
       *
       * @code
       * #include <eosiolib/eosio.hpp>
       * using namespace eosio;
       * using namespace std;
       * class addressbook: contract {
       *   struct address {
       *      uint64_t account_name;
       *      string first_name;
       *      string last_name;
       *      string street;
       *      string city;
       *      string state;
       *      uint32_t zip = 0;
       *      uint64_t primary_key() const { return account_name; }
       *      uint64_t by_zip() const { return zip; }
       *   };
       *   public:
       *     addressbook(name receiver, name code, datastream<const char*> ds):contract(receiver, code, ds) {}
       *     typedef eosio::multi_index< name("address"), address, indexed_by< name("zip"), const_mem_fun<address, uint64_t, &address::by_zip> > address_index;
       *     void myaction() {
       *       // create reference to address_index  - see emplace example below
       *       // add dan account to table           - see emplace example below
       *       uint32_t zipnumb = 93446;
       *       auto zip_index = addresses.get_index<name("zip")>();
       *       auto itr = zip_index.find(zipnumb);
       *       eosio::check(itr->account_name == name("dan"), "Lock arf, Incorrect Record ");
       *     }
       * }
       * EOSIO_DISPATCH( addressbook, (myaction) )
       * @endcode
       */
      template<name::raw IndexName>
      auto get_index() {
         using namespace _multi_index_detail;

         constexpr uint64_t index_num = bluegrass::meta::for_each(indices_type{}, [](auto idx){
            return std::tuple_element_t<const_index, decltype(idx)>::index_name == static_cast<uint64_t>(IndexName);
         });

         static_assert( index_num < sizeof...(Indices), "name provided is not the name of any secondary index within multi_index" );

         return std::tuple_element_t<const_index, std::tuple_element_t<index_num, indices_type>>(this);
      }

      /**
       * Возвращает вторичный индекс нужного типа.
       * @ingroup multiindex
       *
       * @tparam IndexName — идентификатор нужного вторичного индекса
       *
       * @return Индекс соответствующего типа: 64-битный беззнаковый целочисленный ключ (idx64), 128-битный беззнаковый целочисленный ключ (idx128), 128-битный ключ фиксированной длины в лексикографическом порядке (idx128), 256-битный ключ фиксированной длины (idx256), ключ с плавающей точкой, ключ double, ключ long double (четверная точность)
       *
       * Пример:
       *
       * @code
       * // This assumes the code from the get_index() example. Replace myaction() {...}
       *
       *     void myaction() {
       *       // create reference to address_index  - see emplace example below
       *       // add dan account to table           - see emplace example below
       *       // add additional account - brendan
       *
       *       addresses.emplace(payer, [&](auto& address) {
       *         address.account_name = "brendan"_n;
       *         address.first_name = "Brendan";
       *         address.last_name = "Blumer";
       *         address.street = "1 EOS Way";
       *         address.city = "Hong Kong";
       *         address.state = "HK";
       *         address.zip = 93445;
       *       });
       *       uint32_t zipnumb = 93445;
       *       auto zip_index = addresses.get_index<name("zip")>();
       *       auto itr = zip_index.upper_bound(zipnumb);
       *       eosio::check(itr->account_name == name("dan"), "Lock arf, Incorrect First Upper Bound Record ");
       *       itr++;
       *       eosio::check(itr == zip_index.end(), "Lock arf, Incorrect End of Iterator");
       *     }
       * }
       * EOSIO_DISPATCH( addressbook, (myaction) )
       * @endcode
       */
      template<name::raw IndexName>
      auto get_index()const {
         using namespace _multi_index_detail;

         constexpr uint64_t index_num = bluegrass::meta::for_each(indices_type{}, [](auto idx){
            return std::tuple_element_t<mutable_index, decltype(idx)>::index_name == static_cast<uint64_t>(IndexName);
         });

         static_assert( index_num < sizeof...(Indices), "name provided is not the name of any secondary index within multi_index" );

         return std::tuple_element_t<mutable_index, std::tuple_element_t<index_num, indices_type>>(this);
      }

      /**
       * Возвращает итератор на заданный объект в многоиндексной таблице.
       * @ingroup multiindex
       *
       * @param obj — ссылка на нужный объект
       *
       * @return Итератор на этот объект
       *
       * Пример:
       *
       * @code
       * // This assumes the code from the get_index() example. Replace myaction() {...}
       *
       *     void myaction() {
       *       // create reference to address_index  - see emplace example below
       *       // add dan account to table           - see emplace example below
       *       // add additional account - brendan
       *
       *       addresses.emplace(payer, [&](auto& address) {
       *         address.account_name = "brendan"_n;
       *         address.first_name = "Brendan";
       *         address.last_name = "Blumer";
       *         address.street = "1 EOS Way";
       *         address.city = "Hong Kong";
       *         address.state = "HK";
       *         address.zip = 93445;
       *       });
       *       auto user = addresses.get("dan"_n);
       *       auto itr = address.find("dan"_n);
       *       eosio::check(iterator_to(user) == itr, "Invalid iterator");
       *     }
       * }
       * EOSIO_DISPATCH( addressbook, (myaction) )
       * @endcode
       * 
       * Предупреждение: iterator_to может вести себя неопределённо, если вызывающий код
       * передаёт ссылку на объект на стеке вместо ссылки, возвращённой get
       * или полученной разыменованием const_iterator.
       */
      const_iterator iterator_to( const T& obj )const {
         const auto& objitem = static_cast<const item&>(obj);
         eosio::check( objitem.__idx == this, "object passed to iterator_to is not in multi_index" );
         return {this, &objitem};
      }
      /**
       * Добавляет в таблицу новый объект (строку).
       * @ingroup multiindex
       *
       * @param payer — имя аккаунта-плательщика за хранение нового объекта
       * @param constructor — лямбда для инициализации создаваемого объекта на месте
       *
       * @pre создан экземпляр многоиндексной таблицы
       * @post В многоиндексной таблице создан новый объект с уникальным первичным ключом (как в объекте). Объект сериализуется и записывается; при отсутствии таблицы она создаётся.
       * @post Вторичные индексы обновлены для нового объекта; при отсутствии таблиц вторичных индексов они создаются.
       * @post С плательщика взимается хранение нового объекта и при необходимости создания таблицы (и вторичных индексов) — накладные расходы.
       *
       * @return Итератор по первичному ключу на созданный объект
       *
       * Исключение — аккаунт не уполномочен записывать в таблицу.
       *
       * Пример:
       *
       * @code
       * // This assumes the code from the constructor example. Replace myaction() {...}
       *
       *     void myaction() {
       *       address_index addresses(_self, _self.value); // code, scope
       *       // add to table, first argument is account to bill for storage
       *       addresses.emplace(_self, [&](auto& address) {
       *         address.account_name = "dan"_n;
       *         address.first_name = "Daniel";
       *         address.last_name = "Larimer";
       *         address.street = "1 EOS Way";
       *         address.city = "Blacksburg";
       *         address.state = "VA";
       *       });
       *     }
       * }
       * EOSIO_DISPATCH( addressbook, (myaction) )
       * @endcode
       */
      template<typename Lambda>
      const_iterator emplace( name payer, Lambda&& constructor ) {
         using namespace _multi_index_detail;

         eosio::check( _code == current_receiver(), "cannot create objects in table of another contract" ); // Quick fix for mutating db using multi_index that shouldn't allow mutation. Real fix can come in RC2.

         auto itm = std::make_unique<item>( this, [&]( auto& i ){
            T& obj = static_cast<T&>(i);
            constructor( obj );

            size_t size = pack_size( obj );

            //using malloc/free here potentially is not exception-safe, although WASM doesn't support exceptions
            void* buffer = max_stack_buffer_size < size ? malloc(size) : alloca(size);

            datastream<char*> ds( (char*)buffer, size );
            ds << obj;

            uint64_t pk = _multi_index_detail::to_raw_key(obj.primary_key());

            i.__primary_itr = internal_use_do_not_use::db_store_i64( _scope, static_cast<uint64_t>(TableName), payer.value, pk, buffer, size );

            if ( max_stack_buffer_size < size ) {
               free(buffer);
            }

            if( pk >= _next_primary_key )
               _next_primary_key = (pk >= no_available_primary_key) ? no_available_primary_key : (pk + 1);

            bluegrass::meta::for_each(indices_type{}, [&](auto idx){
                  typedef std::tuple_element_t<const_index, decltype(idx)> index_type;

                  i.__iters[index_type::number()] = secondary_index_db_functions<typename index_type::secondary_key_type>::db_idx_store( _scope, index_type::name(), payer.value, obj.primary_key(), index_type::extract_secondary_key(obj) );
            });
         });

         const item* ptr = itm.get();
         auto pk   = _multi_index_detail::to_raw_key(itm->primary_key());
         auto pitr = itm->__primary_itr;

         _items_vector.emplace_back( std::move(itm), pk, pitr );

         return {this, ptr};
      }

      /**
       * Изменяет существующий объект в таблице.
       * @ingroup multiindex
       *
       * @param itr — итератор на обновляемый объект
       * @param payer — имя аккаунта-плательщика за хранение обновлённой строки
       * @param updater — лямбда, обновляющая целевой объект
       *
       * @pre itr указывает на существующий элемент
       * @pre payer — действительный аккаунт, уполномоченный выполнять действие и оплачивать хранение
       *
       * @post Изменённый объект сериализуется и заменяет прежний в таблице.
       * @post Вторичные индексы обновлены; первичный ключ объекта не меняется.
       * @post С плательщика взимается хранение обновлённого объекта.
       * @post Если payer совпадает с прежним плательщиком, списывается только разница объёма хранения (при отрицательной разнице — возврат).
       * @post Если payer другой, прежнему плательщику возвращается стоимость хранения старого объекта.
       *
       * Исключения:
       * При нарушении предусловий выполнение прерывается.
       *
       * Пример:
       *
       * @code
       * // This assumes the code from the constructor example. Replace myaction() {...}
       *
       *     void myaction() {
       *       // create reference to address_index  - see emplace example
       *       // add dan account to table           - see emplace example
       *
       *       auto itr = addresses.find("dan"_n);
       *       eosio::check(itr != addresses.end(), "Address for account not found");
       *       addresses.modify( itr, account payer, [&]( auto& address ) {
       *         address.city = "San Luis Obispo";
       *         address.state = "CA";
       *       });
       *     }
       * }
       * EOSIO_DISPATCH( addressbook, (myaction) )
       * @endcode
       */
      template<typename Lambda>
      void modify( const_iterator itr, name payer, Lambda&& updater ) {
         eosio::check( itr != end(), "cannot pass end iterator to modify" );

         modify( *itr, payer, std::forward<Lambda&&>(updater) );
      }

      /**
       * Изменяет существующий объект в таблице.
       * @ingroup multiindex
       *
       * @param obj — ссылка на обновляемый объект
       * @param payer — имя аккаунта-плательщика за хранение обновлённой строки
       * @param updater — лямбда, обновляющая целевой объект
       *
       * @pre obj — существующий объект в таблице
       * @pre payer — действительный аккаунт, уполномоченный выполнять действие и оплачивать хранение
       *
       * @post Изменённый объект сериализуется и заменяет прежний в таблице.
       * @post Вторичные индексы обновлены; первичный ключ объекта не меняется.
       * @post С плательщика взимается хранение обновлённого объекта.
       * @post Если payer совпадает с прежним плательщиком, списывается только разница объёма хранения (при отрицательной разнице — возврат).
       * @post Если payer другой, прежнему плательщику возвращается стоимость хранения старого объекта.
       *
       * Исключения:
       * При нарушении предусловий выполнение прерывается.
       *
       * Пример:
       *
       * @code
       * // This assumes the code from the constructor example. Replace myaction() {...}
       *
       *     void myaction() {
       *       // create reference to address_index  - see emplace example
       *       // add dan account to table           - see emplace example
       *
       *       auto itr = addresses.find("dan"_n);
       *       eosio::check(itr != addresses.end(), "Address for account not found");
       *       addresses.modify( *itr, payer, [&]( auto& address ) {
       *         address.city = "San Luis Obispo";
       *         address.state = "CA";
       *       });
       *       eosio::check(itr->city == "San Luis Obispo", "Lock arf, Address not modified");
       *     }
       * }
       * EOSIO_DISPATCH( addressbook, (myaction) )
       * @endcode
       */
      template<typename Lambda>
      void modify( const T& obj, name payer, Lambda&& updater ) {
         using namespace _multi_index_detail;

         const auto& objitem = static_cast<const item&>(obj);
         eosio::check( objitem.__idx == this, "object passed to modify is not in multi_index" );
         auto& mutableitem = const_cast<item&>(objitem);
         eosio::check( _code == current_receiver(), "cannot modify objects in table of another contract" ); // Quick fix for mutating db using multi_index that shouldn't allow mutation. Real fix can come in RC2.

         auto secondary_keys = make_extractor_tuple::get_extractor_tuple(indices_type{}, obj);

         uint64_t pk = _multi_index_detail::to_raw_key(obj.primary_key());

         auto& mutableobj = const_cast<T&>(obj); // Do not forget the auto& otherwise it would make a copy and thus not update at all.
         updater( mutableobj );

         eosio::check( pk == _multi_index_detail::to_raw_key(obj.primary_key()), "updater cannot change primary key when modifying an object" );

         size_t size = pack_size( obj );
         //using malloc/free here potentially is not exception-safe, although WASM doesn't support exceptions
         void* buffer = max_stack_buffer_size < size ? malloc(size) : alloca(size);

         datastream<char*> ds( (char*)buffer, size );
         ds << obj;

         internal_use_do_not_use::db_update_i64( objitem.__primary_itr, payer.value, buffer, size );

         if ( max_stack_buffer_size < size ) {
            free( buffer );
         }

         if( pk >= _next_primary_key )
            _next_primary_key = (pk >= no_available_primary_key) ? no_available_primary_key : (pk + 1);

         bluegrass::meta::for_each(indices_type{}, [&](auto idx){
            typedef std::tuple_element_t<const_index, decltype(idx)> index_type;
            auto secondary = index_type::extract_secondary_key( obj );
            if( memcmp( &std::get<index_type::index_number>(secondary_keys), &secondary, sizeof(secondary) ) != 0 ) {
               auto indexitr = mutableitem.__iters[index_type::number()];

               if( indexitr < 0 ) {
                  typename index_type::secondary_key_type temp_secondary_key;
                  indexitr = mutableitem.__iters[index_type::number()]
                           = secondary_index_db_functions<typename index_type::secondary_key_type>::db_idx_find_primary( _code.value, _scope, index_type::name(), pk,  temp_secondary_key );
               }

               secondary_index_db_functions<typename index_type::secondary_key_type>::db_idx_update( indexitr, payer.value, secondary );
            }
         } );
      }

      /**
       * Извлекает существующий объект из таблицы по первичному ключу.
       * @ingroup multiindex
       *
       * @param primary — значение первичного ключа объекта
       * @return Константная ссылка на объект с указанным первичным ключом
       *
       * Исключение — нет объекта с данным ключом
       *
       * Пример:
       *
       * @code
       * // This assumes the code from the constructor example. Replace myaction() {...}
       *
       *     void myaction() {
       *       // create reference to address_index  - see emplace example
       *       // add dan account to table           - see emplace example
       *
       *       auto& user = addresses.get("dan"_n);
       *       eosio::check(user.first_name == "Daniel", "Couldn't get him.");
       *     }
       * }
       * EOSIO_DISPATCH( addressbook, (myaction) )
       * @endcode
       * 
       * Предупреждение:
       * 
       * Избегайте присвоения возвращённой ссылки T& локальной переменной на стеке
       * и передачи её в modify многоиндексной таблицы.
       * Частая ошибка — объявить локальную переменную как auto без ссылки;
       * нужно auto& или decltype(auto).
       */
      template<typename PK>
      const T& get( PK primary, const char* error_msg = "unable to find key" )const {
         auto result = find( primary );
         eosio::check( result != cend(), error_msg );
         return *result;
      }

      /**
       * Ищет существующий объект в таблице по первичному ключу.
       * @ingroup multiindex
       *
       * @param primary — значение первичного ключа объекта
       * @return Итератор на найденный объект с первичным ключом `primary` либо итератор `end`, если объекта с ключом `primary` нет.
       *
       * Пример:
       *
       * @code
       * // This assumes the code from the constructor example. Replace myaction() {...}
       *
       *     void myaction() {
       *       // create reference to address_index  - see emplace example
       *       // add dan account to table           - see emplace example
       *
       *       auto itr = addresses.find("dan"_n);
       *       eosio::check(itr != addresses.end(), "Couldn't get him.");
       *     }
       * }
       * EOSIO_DISPATCH( addressbook, (myaction) )
       * @endcode
       */
      template<typename PK>
      const_iterator find( PK primary )const {
         auto itr2 = std::find_if(_items_vector.rbegin(), _items_vector.rend(), [&](const item_ptr& ptr) {
            return ptr._item->primary_key() == primary;
         });
         if( itr2 != _items_vector.rend() )
            return iterator_to(*(itr2->_item));

         uint64_t primary_int = _multi_index_detail::to_raw_key(primary);
         auto itr = internal_use_do_not_use::db_find_i64( _code.value, _scope, static_cast<uint64_t>(TableName), primary_int );
         if( itr < 0 ) return end();

         const item& i = load_object_by_primary_iterator( itr );
         return iterator_to(static_cast<const T&>(i));
      }

      /**
       * Ищет существующий объект в таблице по первичному ключу.
       * @ingroup multiindex
       *
       * @param primary — значение первичного ключа объекта
       * @param error_msg — сообщение об ошибке, если объекта с ключом `primary` нет
       * @return Итератор на объект с первичным ключом `primary` либо выброс исключения, если объекта нет
       */

      template<typename PK>
      const_iterator require_find( PK primary, const char* error_msg = "unable to find key" )const {
         auto itr2 = std::find_if(_items_vector.rbegin(), _items_vector.rend(), [&](const item_ptr& ptr) {
               return ptr._item->primary_key() == primary;
            });
         if( itr2 != _items_vector.rend() )
            return iterator_to(*(itr2->_item));

         uint64_t primary_int = _multi_index_detail::to_raw_key(primary);
         auto itr = internal_use_do_not_use::db_find_i64( _code.value, _scope, static_cast<uint64_t>(TableName), primary_int );
         eosio::check( itr >= 0,  error_msg );

         const item& i = load_object_by_primary_iterator( itr );
         return iterator_to(static_cast<const T&>(i));
      }

      /**
       * Удаляет существующий объект из таблицы (по итератору).
       * @ingroup multiindex
       *
       * @param itr — итератор на удаляемый объект
       *
       * @pre itr указывает на существующий элемент
       * @post Объект удалён из таблицы, связанное хранение освобождено.
       * @post Обновлены вторичные индексы таблицы.
       * @post Прежнему плательщику за хранение возвращается стоимость таблицы и вторичных индексов для удалённого объекта; при удалении таблицы и индексов — также накладные расходы.
       *
       * @return Для сигнатуры с `const_iterator` — итератор на объект, следующий за удалённым.
       *
       * Исключения:
       * Удаляемого объекта нет в таблице.
       * Действие не уполномочено изменять таблицу.
       * Недопустимый итератор.
       *
       * Пример:
       *
       * @code
       * // This assumes the code from the constructor example. Replace myaction() {...}
       *
       *     void myaction() {
       *       // create reference to address_index  - see emplace example
       *       // add dan account to table           - see emplace example
       *
       *       auto itr = addresses.find("dan"_n);
       *       eosio::check(itr != addresses.end(), "Address for account not found");
       *       addresses.erase( itr );
       *       eosio::check(itr != addresses.end(), "Everting lock arf, Address not erased properly");
       *     }
       * }
       * EOSIO_ABI( addressbook, (myaction) )
       * @endcode
       */
      const_iterator erase( const_iterator itr ) {
         eosio::check( itr != end(), "cannot pass end iterator to erase" );

         const auto& obj = *itr;
         ++itr;

         erase(obj);

         return itr;
      }

      /**
       * Удаляет существующий объект из таблицы.
       * @ingroup multiindex
       *
       * @param obj — удаляемый объект
       *
       * @pre obj — существующий объект в таблице
       * @post Объект удалён из таблицы, связанное хранение освобождено.
       * @post Обновлены вторичные индексы таблицы.
       * @post Прежнему плательщику за хранение возвращается стоимость таблицы и вторичных индексов для удалённого объекта; при удалении таблицы и индексов — также накладные расходы.
       *
       * Исключения:
       * Удаляемого объекта нет в таблице.
       * Действие не уполномочено изменять таблицу.
       * Недопустимый итератор.
       *
       * Пример:
       *
       * @code
       * // This assumes the code from the constructor example. Replace myaction() {...}
       *
       *     void myaction() {
       *       auto itr = addresses.find("dan"_n);
       *       eosio::check(itr != addresses.end(), "Record is not found");
       *       addresses.erase(*itr);
       *       itr = addresses.find("dan"_n);
       *       eosio::check(itr == addresses.end(), "Record is not deleted");
       *     }
       * }
       * EOSIO_DISPATCH( addressbook, (myaction) )
       * @endcode
       */
      void erase( const T& obj ) {
         using namespace _multi_index_detail;

         const auto& objitem = static_cast<const item&>(obj);
         eosio::check( objitem.__idx == this, "object passed to erase is not in multi_index" );
         eosio::check( _code == current_receiver(), "cannot erase objects in table of another contract" ); // Quick fix for mutating db using multi_index that shouldn't allow mutation. Real fix can come in RC2.

         auto pk = objitem.primary_key();
         auto itr2 = std::find_if(_items_vector.rbegin(), _items_vector.rend(), [&](const item_ptr& ptr) {
            return ptr._item->primary_key() == pk;
         });

         eosio::check( itr2 != _items_vector.rend(), "attempt to remove object that was not in multi_index" );

         internal_use_do_not_use::db_remove_i64( objitem.__primary_itr );

         bluegrass::meta::for_each(indices_type{}, [&](auto idx){
            typedef std::tuple_element_t<const_index, decltype(idx)> index_type;

            auto i = objitem.__iters[index_type::number()];
            if( i < 0 ) {
              typename index_type::secondary_key_type secondary;
              i = secondary_index_db_functions<typename index_type::secondary_key_type>::db_idx_find_primary( _code.value, _scope, index_type::name(), objitem.primary_key(),  secondary );
            }
            if( i >= 0 )
               secondary_index_db_functions<typename index_type::secondary_key_type>::db_idx_remove( i );
         });

         _items_vector.erase(--(itr2.base()));
      }

};
}  /// eosio
