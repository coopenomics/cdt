#include <bluegrass/meta/preprocessor.hpp>

#define EOSLIB_REFLECT_MEMBER_OP( OP, elem ) \
  OP t.elem

/**
 *  @defgroup serialize Сериализация
 *  @ingroup core
 *  @brief Определяет C++ API сериализации и десериализации объектов
 */

/**
 *  Задаёт сериализацию и десериализацию для класса
 *
 *  @ingroup serialize
 *  @param TYPE - Класс, для которого задаются сериализация и десериализация
 *  @param MEMBERS - Последовательность имён полей: (field1)(field2)(field3)
 */
#define EOSLIB_SERIALIZE( TYPE,  MEMBERS ) \
 template<typename DataStream> \
 friend DataStream& operator << ( DataStream& ds, const TYPE& t ){ \
    return ds BLUEGRASS_META_FOREACH_SEQ( EOSLIB_REFLECT_MEMBER_OP, <<, MEMBERS );\
 }\
 template<typename DataStream> \
 friend DataStream& operator >> ( DataStream& ds, TYPE& t ){ \
    return ds BLUEGRASS_META_FOREACH_SEQ( EOSLIB_REFLECT_MEMBER_OP, >>, MEMBERS );\
 }

/**
 *  Задаёт сериализацию и десериализацию для класса, наследующего базовые классы,
 *  у которых сериализация и десериализация уже определены
 *
 *  @ingroup serialize
 *  @param TYPE - Класс, для которого задаются сериализация и десериализация
 *  @param BASE - Последовательность имён базовых классов: (basea)(baseb)(basec)
 *  @param MEMBERS - Последовательность имён полей: (field1)(field2)(field3)
 */
#define EOSLIB_SERIALIZE_DERIVED( TYPE, BASE, MEMBERS ) \
 template<typename DataStream> \
 friend DataStream& operator << ( DataStream& ds, const TYPE& t ){ \
    ds << static_cast<const BASE&>(t); \
    return ds BLUEGRASS_META_FOREACH_SEQ( EOSLIB_REFLECT_MEMBER_OP, <<, MEMBERS );\
 }\
 template<typename DataStream> \
 friend DataStream& operator >> ( DataStream& ds, TYPE& t ){ \
    ds >> static_cast<BASE&>(t); \
    return ds BLUEGRASS_META_FOREACH_SEQ( EOSLIB_REFLECT_MEMBER_OP, >>, MEMBERS );\
 }
