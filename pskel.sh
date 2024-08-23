#!/bin/sh

set -e

get_ext_dir() {
  PSKEL_EXT_DIR="/ext"

  if [ -d "${CODESPACE_VSCODE_FOLDER}" ]; then
    echo "[Pskel] GitHub Codespace workspace detected, using \"${CODESPACE_VSCODE_FOLDER}/ext\"." >&2
    PSKEL_EXT_DIR="${CODESPACE_VSCODE_FOLDER}/ext"
  elif [ -d "/workspaces/pskel/ext" ]; then
    echo "[Pskel] Development Containers workspace detected, using \"/workspaces/pskel/ext\"." >&2
    PSKEL_EXT_DIR="/workspaces/pskel/ext"
  elif [ -f "/ext/.gitkeep" ] && [ "$(cat "/ext/.gitkeep")" = "pskel_uninitialized" ] && [ "${1}" != "--no-init" ]; then
    echo "[Pskel] Uninitialized project detected, initializing default skeleton." >&2
    cmd_init "skeleton" >&2
  fi

  echo "${PSKEL_EXT_DIR}"
}

cmd_usage() {
    cat << EOF
Usage: ${0} [task] ...

Available commands:
    init        create new extension
    test        test extension
    build       build PHP runtime
    coverage    generate code coverage
EOF
}

cmd_init() {
  case "${1}" in
    -h|--help)
      cat << EOF
Usage: ${0} init [extension_name] [ext_skel.php options...]
EOF
      return 0
      ;;
    "")
      echo "Error: Extension name is required." >&2
      return 1
      ;;
  esac

  PSKEL_EXT_DIR="$(get_ext_dir --no-init)"
  /usr/local/bin/php "/usr/src/php/ext/ext_skel.php" --ext "${1}" --dir "/tmp" "${@}"
  rm "${PSKEL_EXT_DIR}/.gitkeep"
  rsync -av "/tmp/${1}/" "${PSKEL_EXT_DIR}/"
  rm -rf "/tmp/${1}"
}

cmd_test() {
  case "${1}" in
    -h|--help)
      cat << EOF
Usage: ${0} test [test_type|php_binary_name]
Environment variables:
  CFLAGS, CPPFLAGS:     Compilation flags
  TEST_PHP_ARGS:        Test flags
EOF
      return 0
      ;;
    debug|gcov|valgrind)
      CC="$(command -v "gcc")"
      CXX="$(command -v "g++")"
      case "${1}" in
        debug) build_php_if_not_exists "debug";;
        gcov)
          CONFIGURE_OPTS="--enable-gcov"
          build_php_if_not_exists "gcov"
          CFLAGS="${CFLAGS} --coverage"
          ;;
        valgrind)
          CONFIGURE_OPTS="--with-valgrind"
          build_php_if_not_exists "valgrind"
          TEST_PHP_ARGS="${TEST_PHP_ARGS} -m"
          ;;
      esac
      CMD="$(basename "${CC}")-${1}-php"
      ;;
    msan|asan|ubsan)
      CC="$(command -v "clang")"
      CXX="$(command -v "clang++")"
      case "${1}" in
        msan)
          CONFIGURE_OPTS="--enable-memory-sanitizer"
          build_php_if_not_exists "msan"
          CFLAGS="${CFLAGS} -fsanitize=memory"
          LDFLAGS="${LDFLAGS} -fsanitize=memory"
          ;;
        asan)
          CONFIGURE_OPTS="--enable-address-sanitizer"
          build_php_if_not_exists "asan"
          CFLAGS="${CFLAGS} -fsanitize=address"
          LDFLAGS="${LDFLAGS} -fsanitize=address"
          ;;
        ubsan)
          CONFIGURE_OPTS="--enable-undefined-sanitizer"
          build_php_if_not_exists "ubsan"
          CFLAGS="${CFLAGS} -fsanitize=undefined"
          LDFLAGS="${LDFLAGS} -fsanitize=undefined"
          ;;
      esac
      CMD="$(basename "${CC}")-${1}-php"
      ;;
    "")
      CMD="php"
      ;;
    *)
      CMD="${1}"
      ;;
  esac

  for BIN in "${CMD}" "${CMD}ize" "${CMD}-config"; do
    if ! command -v "${BIN}" >/dev/null 2>&1; then
      echo "Error: Invalid argument '${CMD}', executable file not found" >&2
      exit 1
    fi
  done

  PSKEL_EXT_DIR="$(get_ext_dir)"

  cd "${PSKEL_EXT_DIR}"
    "${CMD}ize"
    if [ "$("${CMD}" -r "echo PHP_VERSION_ID;")" -lt "80400" ]; then
      patch "./build/ltmain.sh" "./../patches/ltmain.sh.patch"
      echo "[Pskel] ltmain.sh patched" >&2
    fi
    CC="${CC}" CXX="${CXX}" CFLAGS="${CFLAGS}" CPPFLAGS="${CPPFLAGS}" LDFLAGS="${LDFLAGS}" ./configure --with-php-config="$(command -v "${CMD}-config")"
    make clean
    make -j"$(nproc)"
    TEST_PHP_ARGS="${TEST_PHP_ARGS} --show-diff -q" make test
  cd -
}

build_php_if_not_exists() {
  if ! command -v "$(basename "${CC}")-${1}-php" >/dev/null 2>&1; then
    CC="${CC}" \
    CXX="${CXX}" \
    CFLAGS="-DZEND_TRACK_ARENA_ALLOC" \
    CPPFLAGS="${CFLAGS}" \
    LDFLAGS="${LDFLAGS}" \
    CONFIGURE_OPTS="${CONFIGURE_OPTS} --enable-debug $(php -r "echo PHP_ZTS === 1 ? '--enable-zts' : '';") --enable-option-checking=fatal --disable-phpdbg --disable-cgi --disable-fpm --enable-cli --without-pcre-jit --disable-opcache-jit --disable-zend-max-execution-timers" \
    cmd_build "$(basename "${CC}")-${1}"
  fi
}

cmd_build() {
  case "${1}" in
    -h|--help)
      cat << EOF
Usage: ${0} build [php_binary_prefix]
Environment variables:
  CFLAGS, CPPFLAGS:     Compilation flags
  CONFIGURE_OPTS:       ./configure options
EOF
      return 0
      ;;
    ?*)
      CONFIGURE_OPTS="--program-prefix=${1}- --includedir=/usr/local/include/${1}-php ${CONFIGURE_OPTS}"
      ;;
  esac

  cd "/usr/src/php"
    ./buildconf --force
    ./configure ${CONFIGURE_OPTS}
    make clean
    make -j"$(nproc)"
    make install
    make clean
  cd -
}

cmd_coverage() {
  case "${1}" in
    -h|--help)
      cat << EOF
Usage: ${0} coverage
Environment variables:
  LCOV_OPTS:    lcov capture options
EOF
      return 0
      ;;
  esac

  cmd_test "gcov"

  PSKEL_EXT_DIR="$(get_ext_dir)"

  lcov --capture --directory "${PSKEL_EXT_DIR}" ${LCOV_OPTS} --exclude "/usr/local/include/*" --output-file "${PSKEL_EXT_DIR}/lcov.info"
  lcov --list "${PSKEL_EXT_DIR}/lcov.info"
}

if [ $# -eq 0 ]; then
  cmd_usage
  exit 1
fi

case "${1}" in
  help) shift; cmd_usage;;
  init) shift; cmd_init "${@}";;
  test) shift; cmd_test "${@}";;
  build) shift; cmd_build "${@}";;
  coverage) shift; cmd_coverage "${@}";;
  *)
    echo "${0} error: invalid command: '${1}'" >&2
    cmd_usage
    exit 1
    ;;
esac
