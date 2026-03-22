/**
 *  @file
 *  @copyright определён в eos/LICENSE
 */
#pragma once
#include <cstdlib>
#include <type_traits>

#include "../../core/eosio/serialize.hpp"
#include "../../core/eosio/datastream.hpp"
#include "../../core/eosio/name.hpp"
#include "../../core/eosio/fixed_bytes.hpp"
#include "../../core/eosio/ignore.hpp"
#include "../../core/eosio/time.hpp"

namespace eosio {

   namespace internal_use_do_not_use {
      extern "C" {
         __attribute__((eosio_wasm_import))
         uint32_t read_action_data( void* msg, uint32_t len );

         __attribute__((eosio_wasm_import))
         uint32_t action_data_size();

         __attribute__((eosio_wasm_import))
         void require_recipient( uint64_t name );

         __attribute__((eosio_wasm_import))
         void require_auth( uint64_t name );

         __attribute__((eosio_wasm_import))
         bool has_auth( uint64_t name );

         __attribute__((eosio_wasm_import))
         void require_auth2( uint64_t name, uint64_t permission );

         __attribute__((eosio_wasm_import))
         bool is_account( uint64_t name );

         __attribute__((eosio_wasm_import))
         void send_inline(char *serialized_action, size_t size);

         __attribute__((eosio_wasm_import))
         void send_context_free_inline(char *serialized_action, size_t size);

         __attribute__((eosio_wasm_import))
         uint64_t  publication_time();

         __attribute__((eosio_wasm_import))
         uint64_t current_receiver();

         __attribute__((eosio_wasm_import))
         uint32_t get_code_hash( uint64_t account, uint32_t struct_version, char* result_buffer, size_t buffer_size );
      }
   };

   struct code_hash_result {
       unsigned_int struct_version;
       uint64_t code_sequence;
       checksum256 code_hash;
       uint8_t vm_type;
       uint8_t vm_version;

       CDT_REFLECT(struct_version, code_sequence, code_hash, vm_type, vm_version);
       EOSLIB_SERIALIZE(code_hash_result, (struct_version)(code_sequence)(code_hash)(vm_type)(vm_version));
   };

   /**
    *  @defgroup action Действие
    *  @ingroup contracts
    *  @brief Типобезопасные C++-обёртки для чтения данных действия и отправки действия
    *  @note Некоторые методы из @ref action можно вызывать напрямую из C++
    */

   /**
    *  @ingroup action
    *  @return Распакованные данные действия, приведённые к типу T.
    *
    *  Пример:
    *
    *  @code
    *  struct dummy_action {
    *    char a; //1
    *    unsigned long long b; //8
    *    int  c; //4
    *
    *    EOSLIB_SERIALIZE( dummy_action, (a)(b)(c) )
    *  };
    *  dummy_action msg = unpack_action_data<dummy_action>();
    *  @endcode
    */
   template<typename T>
   T unpack_action_data() {
      constexpr size_t max_stack_buffer_size = 512;
      size_t size = internal_use_do_not_use::action_data_size();
      char* buffer = (char*)( max_stack_buffer_size < size ? malloc(size) : alloca(size) );
      internal_use_do_not_use::read_action_data( buffer, size );
      return unpack<T>( buffer, size );
   }

   /**
    *  Добавляет указанный аккаунт в множество аккаунтов для уведомления
    *
    *  @ingroup action
    *  @brief Добавляет указанный аккаунт в множество аккаунтов для уведомления
    *  @param notify_account — имя аккаунта для проверки
    */
   inline void require_recipient( name notify_account ){
      internal_use_do_not_use::require_recipient( notify_account.value );
   }

   /**
    *  Все перечисленные аккаунты будут добавлены в множество аккаунтов для уведомления
    *
    *  Вспомогательный метод позволяет добавить несколько аккаунтов в список уведомляемых одним
    *  вызовом вместо многократного вызова аналогичного C API.
    *
    *  @ingroup action
    *  @param notify_account аккаунт для уведомления
    *  @param remaining_accounts аккаунты для уведомления
    *  @note action.code также считается частью множества уведомляемых аккаунтов
    *
    *  Пример:
    *
    *  @code
    *  require_recipient("Account1"_n, "Account2"_n, "Account3"_n); // throws exception if any of them not in set.
    *  @endcode
    */
   template<typename... accounts>
   void require_recipient( name notify_account, accounts... remaining_accounts ){
      internal_use_do_not_use::require_recipient( notify_account.value );
      require_recipient( remaining_accounts... );
   }

   /**
    *  Проверяет, что @ref name входит в множество предоставленных полномочий (auths) для действия. Завершается с ошибкой, если не найдено.
    *
    *  @ingroup action
    *  @param name — имя проверяемого аккаунта
    */
   inline void require_auth( name n ) {
      internal_use_do_not_use::require_auth( n.value );
   }

   /**
   *  Возвращает время в микросекундах с 1970 года для publication_time
   *
   *  @ingroup action
   *  @return время в микросекундах с 1970 года для publication_time
   */
   inline time_point  publication_time() {
     return time_point( microseconds ( internal_use_do_not_use::publication_time() ) );
   }

   /**
   *  Возвращает текущего получателя действия
   *  @return аккаунт — текущий получатель действия
   */
   inline name current_receiver() {
     return name{internal_use_do_not_use::current_receiver()};
   }

   /**
   *  Возвращает хэш кода, опубликованного на указанном аккаунте
   *  @param account имя аккаунта, код которого хэшируется
   *  @param full_result необязательно: при необходимости полной структуры результата — указатель на заполняемую структуру
   *  @return SHA256-хэш кода указанного аккаунта
   */
   inline checksum256 get_code_hash( name account, code_hash_result* full_result = nullptr ) {
       if (full_result == nullptr)
           full_result = (code_hash_result*)alloca(sizeof(code_hash_result));
       constexpr size_t max_stack_buffer_size = 50;

       // Packed size of this struct will virtually always be less than the struct size; always less after padding
       auto struct_buffer_size = sizeof(code_hash_result);
       char* struct_buffer = (char*)alloca(struct_buffer_size);

       using VersionType = decltype(code_hash_result::struct_version);
       const VersionType STRUCT_VERSION = 0;
       auto response_size =
           internal_use_do_not_use::get_code_hash(account.value, STRUCT_VERSION, struct_buffer, struct_buffer_size);
       // Safety check: in this case, response size should never exceed our buffer, but just in case...
       bool buffer_on_heap = false;
       if (response_size > struct_buffer_size) {
           // Slow path: allocate an adequate buffer and try again
           // No need to deallocate struct_buffer since it was alloca'd
           if (response_size > max_stack_buffer_size) {
               struct_buffer = (char*)malloc(response_size);
               buffer_on_heap = true;
           } else {
               struct_buffer = (char*)alloca(response_size);
           }
           internal_use_do_not_use::get_code_hash(account.value, STRUCT_VERSION, struct_buffer, struct_buffer_size);
       }

       check(unpack<VersionType>(struct_buffer, struct_buffer_size) == STRUCT_VERSION,
             "Hypervisor returned unexpected code hash struct version");
       unpack(*full_result, struct_buffer, struct_buffer_size);

       // If struct_buffer is heap allocated, we must free it
       if (buffer_on_heap)
           free(struct_buffer);

       return full_result->code_hash;
   }

   /**
    *  Копирует до len байт данных текущего действия в указанное место
    *
    *  @ingroup action
    *  @param msg — указатель, куда будут скопированы до len байт данных текущего действия
    *  @param len — число байт данных текущего действия для копирования; 0 — вернуть требуемый размер
    *  @return число скопированных в msg байт либо число байт, которые можно скопировать, если передан len==0
    *  @pre `msg` — корректный указатель на область памяти длиной не менее `len` байт
    *  @post `msg` заполнен упакованными данными действия
    */
   inline uint32_t read_action_data( void* msg, uint32_t len ) {
     return internal_use_do_not_use::read_action_data(msg, len);
   }

   /**
    * Возвращает длину поля данных текущего действия. Полезно для действий с динамическим размером
    *
    * @return длина поля данных текущего действия
    */
   inline uint32_t action_data_size() {
     return internal_use_do_not_use::action_data_size();
   }
   /**
    * Упакованное представление уровня разрешения (авторизация)
    *
    * @ingroup action
    */
   struct permission_level {
      /**
       * Создаёт объект уровня разрешения с именем актора и именем разрешения
       *
       * @param a — имя аккаунта, которому принадлежит эта авторизация
       * @param p — имя разрешения
       */
      permission_level( name a, name p ):actor(a),permission(p){}

      /**
       * Конструктор по умолчанию
       *
       */
      permission_level(){}

      /**
       * Имя аккаунта, которому принадлежит это разрешение
       */
      name    actor;
      /**
       * Имя разрешения
       */
      name    permission;

      /**
       * Проверка равенства двух разрешений
       *
       * @param a — первое сравниваемое разрешение
       * @param b — второе сравниваемое разрешение
       * @return true, если равны
       * @return false, если не равны
       */
      friend constexpr bool operator == ( const permission_level& a, const permission_level& b ) {
         return std::tie( a.actor, a.permission ) == std::tie( b.actor, b.permission );
      }

      /**
       * Лексикографическое сравнение двух разрешений
       *
       * @param a — первое сравниваемое разрешение
       * @param b — второе сравниваемое разрешение
       * @return true, если a < b
       * @return false, если a >= b
       */
      friend constexpr bool operator < ( const permission_level& a, const permission_level& b ) {
         return std::tie( a.actor, a.permission ) < std::tie( b.actor, b.permission );
      }

      EOSLIB_SERIALIZE( permission_level, (actor)(permission) )
   };

   /**
    *  Требует указанную авторизацию для этого действия. Если действие не содержит указанной авторизации, выполнение завершится с ошибкой.
    *
    *  @ingroup action
    *  @param level — требуемая авторизация
    */
   inline void require_auth( const permission_level& level ) {
      internal_use_do_not_use::require_auth2( level.actor.value, level.permission.value );
   }

   /**
    *  Проверяет, что у @ref n есть авторизация в текущем действии.
    *
    *  @ingroup action
    *  @param n — имя проверяемого аккаунта
    */
   inline bool has_auth( name n ) {
      return internal_use_do_not_use::has_auth( n.value );
   }

   /**
    *  Проверяет, что @ref n — существующий аккаунт.
    *
    *  @ingroup action
    *  @param n — имя проверяемого аккаунта
    */
   inline bool is_account( name n ) {
      return internal_use_do_not_use::is_account( n.value );
   }

   /**
    *  Упакованное представление действия вместе с
    *  метаданными об уровнях авторизации.
    *
    *  @ingroup action
    */
   struct action {
      /**
       *  Имя аккаунта, для которого предназначено действие
       */
      name                       account;

      /**
       *  Имя действия
       */
      name                       name;

      /**
       *  Список разрешений, авторизующих это действие
       */
      std::vector<permission_level>   authorization;

      /**
       *  Полезная нагрузка (данные)
       */
      std::vector<char>               data;

      /**
       *  Конструктор по умолчанию
       */
      action() = default;

      /**
       * Создаёт объект действия с заданным разрешением, получателем действия, именем действия и структурой действия
       *
       * @tparam T  — тип структуры действия, должен сериализоваться через `pack(...)`
       * @param auth — разрешение, авторизующее это действие
       * @param a — имя аккаунта-получателя действия
       * @param n — имя действия
       * @param value — структура действия, сериализуемая через pack в data
       */
      template<typename T>
      action( const permission_level& auth, struct name a, struct name n, T&& value )
      :account(a), name(n), authorization(1,auth), data(pack(std::forward<T>(value))) {}

      /**
       * Создаёт объект действия с заданным списком разрешений, получателем, именем действия и структурой действия
       *
       * @tparam T  — тип структуры действия, должен сериализоваться через `pack(...)`
       * @param auths — список разрешений, авторизующих это действие
       * @param a — имя аккаунта-получателя действия
       * @param n — имя действия
       * @param value — структура действия, сериализуемая через pack в data
       */
      template<typename T>
      action( std::vector<permission_level> auths, struct name a, struct name n, T&& value )
      :account(a), name(n), authorization(std::move(auths)), data(pack(std::forward<T>(value))) {}

      /// @cond INTERNAL

      EOSLIB_SERIALIZE( action, (account)(name)(authorization)(data) )

      /// @endcond

      /**
       * Отправляет действие как встроенное (inline)
       */
      void send() const {
         auto serialize = pack(*this);
         internal_use_do_not_use::send_inline(serialize.data(), serialize.size());
      }

      /**
       * Отправляет действие как встроенное контекстно-свободное (context free)
       *
       * @pre это действие не должно содержать авторизаций
       */
      void send_context_free() const {
         eosio::check( authorization.size() == 0, "context free actions cannot have authorizations");
         auto serialize = pack(*this);
         internal_use_do_not_use::send_context_free_inline(serialize.data(), serialize.size());
      }

      /**
       * Возвращает распакованные данные как T
       *
       * @tparam T ожидаемый тип данных
       * @return данные действия
       */
      template<typename T>
      T data_as() {
         return unpack<T>( &data[0], data.size() );
      }

   };



   namespace detail {

      /// @cond INTERNAL

      template <typename T>
      struct unwrap { typedef T type; };

      template <typename T>
      struct unwrap<ignore<T>> { typedef T type; };

      template <typename R, typename Act, typename... Args>
      auto get_args(R(Act::*p)(Args...)) {
         return std::tuple<std::decay_t<typename unwrap<Args>::type>...>{};
      }

      template <typename R, typename Act, typename... Args>
      auto get_args_nounwrap(R(Act::*p)(Args...)) {
         return std::tuple<std::decay_t<Args>...>{};
      }

      template <auto Action>
      using deduced = decltype(get_args(Action));

      template <auto Action>
      using deduced_nounwrap = decltype(get_args_nounwrap(Action));

      template <typename T>
      struct convert { typedef T type; };

      template <>
      struct convert<const char*> { typedef std::string type; };

      template <>
      struct convert<char*> { typedef std::string type; };

      template <typename T, typename U>
      struct is_same { static constexpr bool value = std::is_convertible<T,U>::value; };

      template <typename U>
      struct is_same<bool,U> { static constexpr bool value = std::is_integral<U>::value; };

      template <typename T>
      struct is_same<T,bool> { static constexpr bool value = std::is_integral<T>::value; };

      template <size_t N, size_t I, auto Arg, auto... Args>
      struct get_nth_impl { static constexpr auto value  = get_nth_impl<N,I+1,Args...>::value; };

      template <size_t N, auto Arg, auto... Args>
      struct get_nth_impl<N, N, Arg, Args...> { static constexpr auto value = Arg; };

      template <size_t N, auto... Args>
      struct get_nth { static constexpr auto value  = get_nth_impl<N,0,Args...>::value; };

      template <auto Action, size_t I, typename T, typename... Rest>
      struct check_types {
         static_assert(detail::is_same<typename convert<T>::type, typename convert<typename std::tuple_element<I, deduced<Action>>::type>::type>::value);
         using type = check_types<Action, I+1, Rest...>;
         static constexpr bool value = true;
      };
      template <auto Action, size_t I, typename T>
      struct check_types<Action, I, T> {
         static_assert(detail::is_same<typename convert<T>::type, typename convert<typename std::tuple_element<I, deduced<Action>>::type>::type>::value);
         static constexpr bool value = true;
      };

      template <auto Action, typename... Ts>
      constexpr bool type_check() {
         static_assert(sizeof...(Ts) == std::tuple_size<deduced<Action>>::value);
         if constexpr (sizeof...(Ts) != 0)
            return check_types<Action, 0, Ts...>::value;
         return true;
      }

      /// @endcond
   }

   /**
    * Обёртка над объектом действия.
    *
    * @brief Обёртка для конкретного действия, упрощающая отправку встроенных (inline) действий к этому действию из других контрактов.
    * Пример:
    * @code
    * // defined by contract writer of the actions
    * using transfer_act = action_wrapper<"transfer"_n, &token::transfer>;
    * // usage by different contract writer
    * transfer_act{"eosio.token"_n, {st.issuer, "active"_n}}.send(st.issuer, to, quantity, memo);
    * // or
    * transfer_act trans_action{ "eosio.token"_n, {st.issuer, "active"_n}};
    * trans_action.send(st.issuer, to, quantity, memo);
    * @endcode
    */
   template <eosio::name::raw Name, auto Action>
   struct action_wrapper {
      template <typename Code>
      constexpr action_wrapper(Code&& code, std::vector<eosio::permission_level>&& perms)
         : code_name(std::forward<Code>(code)), permissions(std::move(perms)) {}

      template <typename Code>
      constexpr action_wrapper(Code&& code, const std::vector<eosio::permission_level>& perms)
         : code_name(std::forward<Code>(code)), permissions(perms) {}

      template <typename Code>
      constexpr action_wrapper(Code&& code, eosio::permission_level&& perm)
         : code_name(std::forward<Code>(code)), permissions({1, std::move(perm)}) {}

      template <typename Code>
      constexpr action_wrapper(Code&& code, const eosio::permission_level& perm)
         : code_name(std::forward<Code>(code)), permissions({1, perm}) {}

      template <typename Code>
      constexpr action_wrapper(Code&& code)
         : code_name(std::forward<Code>(code)) {}

      static constexpr eosio::name action_name = eosio::name(Name);
      eosio::name code_name;
      std::vector<eosio::permission_level> permissions;

      static constexpr auto get_mem_ptr() {
         return Action;
      }

      template <typename... Args>
      action to_action(Args&&... args)const {
         static_assert(detail::type_check<Action, Args...>());
         return action(permissions, code_name, action_name, detail::deduced<Action>{std::forward<Args>(args)...});
      }
      template <typename... Args>
      void send(Args&&... args)const {
         to_action(std::forward<Args>(args)...).send();
      }

      template <typename... Args>
      void send_context_free(Args&&... args)const {
         to_action(std::forward<Args>(args)...).send_context_free();
      }

   };

   template <eosio::name::raw Name, auto... Actions>
   struct variant_action_wrapper {
      template <typename Code>
      constexpr variant_action_wrapper(Code&& code, std::vector<eosio::permission_level>&& perms)
         : code_name(std::forward<Code>(code)), permissions(std::move(perms)) {}

      template <typename Code>
      constexpr variant_action_wrapper(Code&& code, const std::vector<eosio::permission_level>& perms)
         : code_name(std::forward<Code>(code)), permissions(perms) {}

      template <typename Code>
      constexpr variant_action_wrapper(Code&& code, eosio::permission_level&& perm)
         : code_name(std::forward<Code>(code)), permissions({1, std::move(perm)}) {}

      template <typename Code>
      constexpr variant_action_wrapper(Code&& code, const eosio::permission_level& perm)
         : code_name(std::forward<Code>(code)), permissions({1, perm}) {}

      static constexpr eosio::name action_name = eosio::name(Name);
      eosio::name code_name;
      std::vector<eosio::permission_level> permissions;

      template <size_t Variant>
      static constexpr auto get_mem_ptr() {
         return detail::get_nth<Variant, Actions...>::value;
      }

      template <size_t Variant, typename... Args>
      action to_action(Args&&... args)const {
         static_assert(detail::type_check<detail::get_nth<Variant, Actions...>::value, Args...>());
         unsigned_int var = Variant;
         return action(permissions, code_name, action_name, std::tuple_cat(std::make_tuple(var), detail::deduced<detail::get_nth<Variant, Actions...>::value>{std::forward<Args>(args)...}));
      }


      template <size_t Variant, typename... Args>
      void send(Args&&... args)const {
         to_action<Variant>(std::forward<Args>(args)...).send();
      }

      template <size_t Variant, typename... Args>
      void send_context_free(Args&&... args) const {
         to_action<Variant>(std::forward<Args>(args)...).send_context_free();
      }

   };

   template<typename... Args>
   void dispatch_inline( name code, name act,
                         std::vector<permission_level> perms,
                         std::tuple<Args...> args ) {
      action( perms, code, act, std::move(args) ).send();
   }

   template<typename, name::raw>
   struct inline_dispatcher;


   template<typename T, name::raw Name, typename... Args>
   struct inline_dispatcher<void(T::*)(Args...), Name> {
      static void call(name code, const permission_level& perm, std::tuple<Args...> args) {
         dispatch_inline(code, name(Name), std::vector<permission_level>(1, perm), std::move(args));
      }
      static void call(name code, std::vector<permission_level> perms, std::tuple<Args...> args) {
         dispatch_inline(code, name(Name), std::move(perms), std::move(args));
      }
   };

} // namespace eosio

#define INLINE_ACTION_SENDER3( CONTRACT_CLASS, FUNCTION_NAME, ACTION_NAME  )\
::eosio::inline_dispatcher<decltype(&CONTRACT_CLASS::FUNCTION_NAME), ACTION_NAME>::call

#define INLINE_ACTION_SENDER2( CONTRACT_CLASS, NAME )\
INLINE_ACTION_SENDER3( CONTRACT_CLASS, NAME, ::eosio::name(#NAME) )

#define INLINE_ACTION_SENDER(...) BLUEGRASS_META_OVERLOAD(INLINE_ACTION_SENDER,__VA_ARGS__)(__VA_ARGS__)

/**
 * Отправка встроенного (inline) действия из контракта.
 *
 * @brief Макрос для упрощения вызова встроенных (inline) действий
 * @details Макрос отправки встроенного действия упрощает вызов встроенных действий. При вызове новых действий из уже выполняемых
 * COOPOS поддерживает две модели обмена: встроенные (inline) и отложенные (deferred). Встроенные действия выполняются как часть текущей транзакции. Этот макрос
 * создаёт @ref action с переданными параметрами и автоматически вызывает action.send() для этого действия.
 *
 * Пример:
 * @code
 * SEND_INLINE_ACTION( *this, transfer, {st.issuer,N(active)}, {st.issuer, to, quantity, memo} );
 * @endcode
 *
 * Приведённый выше пример взят из контракта токена COOPOS (аналог стандартного токена).
 * В примере:
 *       используется переданный разыменованный указатель `this` для вызова this.get_self(), т.е. контракта токена COOPOS;
 *       вызывается действие transfer контракта токена COOPOS;
 *       используется разрешение active аккаунта «issuer»;
 *       передаются параметры st.issuer, to, quantity и memo.
 * Макрос создаёт структуру действия для вызова send() — вызова transfer(account_name from, account_name to, asset quantity, string memo)
 *
 * @param CONTRACT — вызываемый контракт, содержащий отправляемое действие; соответствует @ref account
 * @param NAME — имя вызываемого действия; соответствует @ref name
 * @param ... — авторизующее разрешение; соответствует @ref authorization, затем параметры действия; соответствуют @ref data.
 */

#define SEND_INLINE_ACTION( CONTRACT, NAME, ... )\
INLINE_ACTION_SENDER(std::decay_t<decltype(CONTRACT)>, NAME)( (CONTRACT).get_self(),__VA_ARGS__)
