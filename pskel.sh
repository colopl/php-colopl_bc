#!/bin/sh -e

cmd_usage() {
    cat << EOF
Usage: ${0} [task] ...

Available commands:
    init	create new extension
    test	test extension
    build	build PHP runtime
EOF
}

cmd_init() {
  if test "${1}" = "-h" || test "${1}" = "--help" || test "${#}" -lt 1; then
    cat << EOF
Usage: $0 init [extension_name] [ext_skel.php options...]
EOF
    return 0
  fi

  PSKEL_EXT_DIR="/ext"

  if test -d "/workspace/pskel/ext"; then
    echo "[Pskel] Development containers workspace detected, use \"/workspace/pskel/ext\"." >&2
    PSKEL_EXT_DIR="/workspace/pskel/ext"
  fi

  /usr/local/bin/php "/usr/src/php/ext/ext_skel.php" --ext "${1}" --dir "/tmp" ${@}
  rm -rf "${PSKEL_EXT_DIR}"
  mv "/tmp/${1}" "${PSKEL_EXT_DIR}"
}

cmd_test() {
  if test "${1}" = "-h" || test "${1}" = "--help"; then
    cat << EOF
Usage: ${0} test [test_type|php_binary_name]
env:
  CFLAGS, CPPFLAGS:	Compile flags
  TEST_PHP_ARGS:	Test flags
EOF
    return 0
  fi

  if test "x${1}" = "x"; then
    CC="$(which "gcc")"; CXX="$(which "g++")"; CMD="php"
  else
    case "${1}" in
      debug)
        if ! type "debug-php" > /dev/null 2>&1; then
          CC="$(which "gcc")" CXX="$(which "g++")" CFLAGS="-DZEND_TRACK_ARENA_ALLOC" CPPFLAGS="${CFLAGS}" CONFIGURE_OPTS="--enable-debug $(php -r "echo PHP_ZTS === 1 ? '--enable-zts' : '';") --enable-option-checking=fatal --disable-phpdbg --disable-cgi --disable-fpm --enable-cli --without-pcre-jit --disable-opcache-jit --disable-zend-max-execution-timers" cmd_build "debug"
        fi && \
        CC="$(which "gcc")"; CXX="$(which "g++")"; CMD="debug-php";;
      valgrind)
        if ! type "gcc-valgrind-php" > /dev/null 2>&1; then
          CC="$(which "gcc")" CXX="$(which "g++")" CFLAGS="-DZEND_TRACK_ARENA_ALLOC" CPPFLAGS="${CFLAGS}" CONFIGURE_OPTS="--enable-debug $(php -r "echo PHP_ZTS === 1 ? '--enable-zts' : '';") --enable-option-checking=fatal --disable-phpdbg --disable-cgi --disable-fpm --enable-cli --without-pcre-jit --disable-opcache-jit --disable-zend-max-execution-timers --with-valgrind" cmd_build "gcc-valgrind"
        fi && \
        TEST_PHP_ARGS="${TEST_PHP_ARGS} -m" CC="$(which "gcc")"; CXX="$(which "g++")"; CMD="gcc-valgrind-php";;
      msan)
        if ! type "clang-msan-php" > /dev/null 2>&1; then
          CC="$(which "clang")" CXX="$(which "clang++")" CFLAGS="-DZEND_TRACK_ARENA_ALLOC" CPPFLAGS="${CFLAGS}" LDFLAGS="${LDFLAGS} -fsanitize=memory" CONFIGURE_OPTS="--enable-debug $(php -r "echo PHP_ZTS === 1 ? '--enable-zts' : '';") --enable-option-checking=fatal --disable-phpdbg --disable-cgi --disable-fpm --enable-cli --without-pcre-jit --disable-opcache-jit --disable-zend-max-execution-timers --enable-memory-sanitizer" cmd_build "clang-msan"
        fi && \
        CFLAGS="${CFLAGS} -fsanitize=memory"; CPPFLAGS="${CPPFLAGS}"; LDFLAGS="-fsanitize=memory"; CC="$(which "clang")"; CXX="$(which "clang++")"; CMD="clang-msan-php";;
      asan)
        if ! type "clang-asan-php" > /dev/null 2>&1; then
          CC="$(which "clang")" CXX="$(which "clang++")" CFLAGS="-DZEND_TRACK_ARENA_ALLOC" CPPFLAGS="${CFLAGS}" LDFLAGS="${LDFLAGS} -fsanitize=address" CONFIGURE_OPTS="--enable-debug $(php -r "echo PHP_ZTS === 1 ? '--enable-zts' : '';") --enable-option-checking=fatal --disable-phpdbg --disable-cgi --disable-fpm --enable-cli --without-pcre-jit --disable-opcache-jit --disable-zend-max-execution-timers --enable-address-sanitizer" cmd_build "clang-asan"
        fi && \
        CFLAGS="${CFLAGS} -fsanitize=address"; CPPFLAGS="${CPPFLAGS}"; LDFLAGS="-fsanitize=address"; CC="$(which "clang")"; CXX="$(which "clang++")"; CMD="clang-asan-php";;
      ubsan)
        if ! type "clang-ubsan-php" > /dev/null 2>&1; then
          CC="$(which "clang")" CXX="$(which "clang++")" CFLAGS="-DZEND_TRACK_ARENA_ALLOC" CPPFLAGS="${CFLAGS}" LDFLAGS="${LDFLAGS} -fsanitize=undefined" CONFIGURE_OPTS="--enable-debug $(php -r "echo PHP_ZTS === 1 ? '--enable-zts' : '';") --enable-option-checking=fatal --disable-phpdbg --disable-cgi --disable-fpm --enable-cli --without-pcre-jit --disable-opcache-jit --disable-zend-max-execution-timers --enable-undefined-sanitizer" cmd_build "clang-ubsan"
        fi && \
        CFLAGS="${CFLAGS} -fsanitize=undefined"; CPPFLAGS="${CPPFLAGS}"; LDFLAGS="-fsanitize=undefined"; CC="$(which "clang")"; CXX="$(which "clang++")"; CMD="clang-ubsan-php";;
      *) CMD="${1}"
    esac
  fi

  for BIN in "${CMD}" "${CMD}ize" "${CMD}-config"; do
    if ! type "${BIN}" > /dev/null 2>&1; then
      echo "Invalid argument: '${CMD}', executable file not found" >&2
      exit 1
    fi
  done

  PSKEL_EXT_DIR="/ext"

  if test -d "/workspace/pskel/ext"; then
    echo "[Pskel] Development containers workspace detected, use \"/workspace/pskel/ext\"." >&2
    PSKEL_EXT_DIR="/workspace/pskel/ext"
  else
    if test -f "/ext/.gitkeep" && test $(cat "/ext/.gitkeep") = "pskel_uninitialized"; then
       echo "[Pskel] Uninitialized project detected, initialize default skeleton." >&2
       cmd_init "skeleton"
    fi
  fi

  cd "${PSKEL_EXT_DIR}"
    "${CMD}ize"
    CC=${CC} CXX=${CXX} CFLAGS="${CFLAGS}" CPPFLAGS="${CPPFLAGS}" LDFLAGS="${LDFLAGS}" ./configure --with-php-config="$(which "${CMD}-config")"
    make clean
    make -j"$(nproc)"
    TEST_PHP_ARGS="${TEST_PHP_ARGS} --show-diff -q" make test
  cd -
}

cmd_build() {
  if test "${1}" = "-h" || test "${1}" = "--help"; then
    cat << EOF
Usage: ${0} build [php_binary_prefix]
env:
  CFLAGS, CPPFLAGS:	Compile flags
  CONFIGURE_OPTS:	./configure options
EOF
    return 0
  fi

  if ! test "x${1}" = "x"; then
    CONFIGURE_OPTS="--program-prefix="${1}-" --includedir="/usr/local/include/${1}-php" ${CONFIGURE_OPTS}"
  fi

  cd "/usr/src/php"
    ./buildconf --force
    ./configure ${CONFIGURE_OPTS}
    make clean
    make -j"$(nproc)"
    make install
    make clean
  cd -
}

if [ $# -eq 0 ]; then
  cmd_usage
  exit 1
fi

case "${1}" in
  help) shift && cmd_usage;;
  init) shift && cmd_init "${@}";;
  test) shift && cmd_test "${@}";;
  build) shift && cmd_build "${@}";;
  *)
    echo "${0} error: invalid command: '${1}'" >&2
    cmd_usage
    exit 1
    ;;
esac
