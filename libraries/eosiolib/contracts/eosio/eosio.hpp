/**
 *  @file
 *  @copyright как определено в eos/LICENSE
 */
#pragma once
#include "action.hpp"
#include "../../core/eosio/print.hpp"
#include "multi_index.hpp"
#include "dispatcher.hpp"
#include "contract.hpp"

#ifndef EOSIO_NATIVE
static_assert( sizeof(long) == sizeof(int), "unexpected size difference" );
#endif

/**
 * @defgroup core API ядра
 * @brief C++ API ядра для функциональности смарт-контрактов, не зависящей от конкретной сети
 */

 /**
  * @defgroup contracts API контрактов
  * @brief C++ API контрактов для функциональности смарт-контрактов, зависящей от сети COOPOS
  */

/**
 * @defgroup types Типы
 * @brief C++ API типов для компоновки данных структур, доступных на платформе COOPOS
 */
