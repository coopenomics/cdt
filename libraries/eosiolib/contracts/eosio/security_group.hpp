#pragma once
#include <set>
#include "../../core/eosio/name.hpp"
#include "../../core/eosio/serialize.hpp"

namespace eosio {

namespace internal_use_do_not_use {
extern "C" {
__attribute__((eosio_wasm_import)) int64_t add_security_group_participants(const char* data, uint32_t datalen);

__attribute__((eosio_wasm_import)) int64_t remove_security_group_participants(const char* data, uint32_t datalen);

__attribute__((eosio_wasm_import)) bool in_active_security_group(const char* data, uint32_t datalen);

__attribute__((eosio_wasm_import)) uint32_t get_active_security_group(char* data, uint32_t datalen);
}
} // namespace internal_use_do_not_use

/**
 *  @defgroup security_group Группа безопасности
 *  @ingroup contracts
 *  @brief Определяет C++ API группы безопасности
 */

struct security_group {
   uint32_t version;
   std::set<name> participants;
   CDT_REFLECT(version, participants);
};

/**
 * Предложить новых участников группы безопасности.
 *
 * @ingroup security_group
 * @param participants — участники.
 *
 * @return -1, если предложение новой группы безопасности не удалось, иначе 0.
 */
inline int64_t add_security_group_participants(const std::set<name>& participants) {
   auto packed_participants = eosio::pack( participants );
   return internal_use_do_not_use::add_security_group_participants( packed_participants.data(), packed_participants.size() );
}

/**
 * Предложить исключить участников из группы безопасности.
 *
 * @ingroup security_group
 * @param participants — участники.
 *
 * @return -1, если предложение новой группы безопасности не удалось, иначе 0.
 */
inline int64_t remove_security_group_participants(const std::set<name>& participants){
   auto packed_participants = eosio::pack( participants );
   return internal_use_do_not_use::remove_security_group_participants( packed_participants.data(), packed_participants.size() );
}

/**
 * Проверить, что все указанные аккаунты входят в активную группу безопасности.
 *
 * @ingroup security_group
 * @param participants — участники.
 *
 * @return true, если все указанные аккаунты входят в активную группу безопасности.
 */
inline bool in_active_security_group(const std::set<name>& participants){
   auto packed_participants = eosio::pack( participants );
   return internal_use_do_not_use::in_active_security_group( packed_participants.data(), packed_participants.size() );
}

/**
 * Возвращает активную группу безопасности (распакованную структуру).
 *
 * @ingroup security_group
 * @return объект security_group с данными активной группы
 */
inline security_group get_active_security_group() {
   size_t buffer_size = internal_use_do_not_use::get_active_security_group(0, 0);
   std::vector<char> buffer(buffer_size);
   internal_use_do_not_use::get_active_security_group(buffer.data(), buffer_size);
   return eosio::unpack<security_group>(buffer);
}
} // namespace eosio