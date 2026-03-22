# CDT (Contract Development Toolkit)

**Contract Development Toolkit (CDT)** — это цепочка инструментов C/C++ для сборки WebAssembly (WASM) и набор утилит для разработки смарт-контрактов на C/C++, предназначенных для развёртывания в блокчейне **COOPOS** (совместим с протоколом [Antelope](https://github.com/AntelopeIO/)).

Помимо универсальной сборки WebAssembly, CDT добавляет возможности и оптимизации именно под смарт-контракты. Инструментарий опирается на [Clang 9](https://github.com/coopenomics/cdt-llvm) и LLVM этого поколения; целевой WASM по-прежнему считается экспериментальным, поэтому часть оптимизаций может быть неполной или недоступной.

## Структура репозитория

Ветка `main` — ветка разработки: **не используйте её в продакшене**. Актуальные релизы, пре-релизы и устаревшие версии смотрите на [странице релизов](https://github.com/coopenomics/cdt/releases) и по соответствующим тегам.

## Бинарные пакеты

Поддерживаются пакеты Debian для Linux x86_64. Скачать сборку под нужную версию CDT можно на [странице релизов](https://github.com/coopenomics/cdt/releases) — это самый быстрый способ начать работу.

### Установка пакета Debian

Скачайте подходящий `.deb` и установите. Пример для актуального релиза **v4.2.0** (другие версии — на странице релизов):

```sh
wget https://github.com/coopenomics/cdt/releases/download/v4.2.0/cdt_4.2.0-1_amd64.deb
sudo apt install ./cdt_4.2.0-1_amd64.deb
```

### Удаление пакета Debian

```sh
sudo apt remove cdt
```

## Сборка из исходников

Полностью поддерживаются недавние LTS-релизы Ubuntu. Другие дистрибутивы Linux и POSIX-системы (включая macOS) — по возможности, без гарантии полного набора функций.

Ниже предполагается **Ubuntu 20.04**.

### Зависимости

```sh
apt-get update && apt-get install   \
        build-essential             \
        clang                       \
        cmake                       \
        git                         \
        libxml2-dev                 \
        opam ocaml-interp           \
        python3                     \
        python3-pip                 \
        time
```

```sh
python3 -m pip install pygments
```

### Интеграционные тесты

Для них нужна сборка [Leap](https://github.com/AntelopeIO/leap) (реализация протокола Antelope) из исходников; одного бинарного пакета Leap обычно недостаточно.

Если Leap собирать не планируете, CDT можно собрать без интеграционных тестов. Иначе перед `cmake` в той же сессии терминала задайте путь к сборке Leap, например:

```sh
export leap_DIR=/path/to/leap/build/lib/cmake/leap
```

Далее при `cmake` не должно появляться сообщения вроде `leap package not found`; иначе интеграционные тесты не соберутся.

### ccache

При проблемах со сборкой через ccache можно отключить его:

```sh
export CCACHE_DISABLE=1
```

### Сборка CDT

**Параллельная сборка (`-j`)**: часть единиц трансляции в CDT очень тяжёлые по памяти. Если компилятор падает или нехватает RAM, уменьшите параллелизм, например `make -j2` вместо `make -j $(nproc)`.

```sh
git clone --recursive https://github.com/coopenomics/cdt
cd cdt
mkdir build
cd build
cmake ..
make -j $(nproc)
```

Исполняемые файлы — в `build/bin`. Можно добавить каталог в `PATH`, либо использовать CMake toolchain `build/lib/cmake/CDTWasmToolchain.cmake` в своём проекте без глобальной установки CDT.

Глобальная установка описана в разделе [Установка CDT](#установка-cdt).

#### Отладочная сборка

```sh
cmake -DCMAKE_BUILD_TYPE="Debug" -DTOOLS_BUILD_TYPE="Debug" -DLIBS_BUILD_TYPE="Debug" ..
```

### Тесты

#### Модульные

```sh
cd build
ctest
```

#### Интеграционные (если собирались)

```sh
cd build/tests/integration
ctest
```

### Установка CDT

Глобальная установка добавляет в `PATH`, среди прочего:

* cdt-abidiff  
* cdt-ar  
* cdt-cc  
* cdt-cpp  
* cdt-init  
* cdt-ld  
* cdt-nm  
* cdt-objcopy  
* cdt-objdump  
* cdt-ranlib  
* cdt-readelf  
* cdt-strip  
* eosio-pp  
* eosio-wasm2wast  
* eosio-wast2wasm  

Также ставятся CMake-файлы CDT (каталог `cmake/cdt` в системном `lib`).

#### Ручная установка

Из каталога `build`:

```
sudo make install
```

#### Установка из собранного пакета

Удобнее сгенерировать пакет и поставить его — так проще удалить CDT позже.

```sh
cd packages
bash ./generate_package.sh deb ubuntu-20.04 amd64
sudo apt install ./cdt_*_amd64.deb
```

### Удаление CDT

#### После `make install`

```sh
sudo rm -fr /usr/local/cdt
sudo rm -fr /usr/local/lib/cmake/cdt
sudo rm /usr/local/bin/eosio-*
sudo rm /usr/local/bin/cdt-*
```

#### После пакета Debian

```sh
sudo apt remove cdt
```

## Лицензия

[MIT](./LICENSE)
