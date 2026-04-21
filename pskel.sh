#!/bin/sh

set -e

get_ext_dir() {
  PSKEL_EXT_DIR="/ext"

  if test -d "${CODESPACE_VSCODE_FOLDER}"; then
    echo "[Pskel] GitHub Codespace workspace detected, using \"${CODESPACE_VSCODE_FOLDER}/ext\"." >&2
    PSKEL_EXT_DIR="${CODESPACE_VSCODE_FOLDER}/ext"
  elif test -d "/workspaces/pskel/ext"; then
    echo "[Pskel] Development Containers workspace detected, using \"/workspaces/pskel/ext\"." >&2
    PSKEL_EXT_DIR="/workspaces/pskel/ext"
  elif test -f "/ext/.gitkeep" && test "$(cat "/ext/.gitkeep")" = "pskel_uninitialized" && test "${1}" != "--no-init"; then
    echo "[Pskel] Uninitialized project detected, initializing default skeleton." >&2
    cmd_init "skeleton" >&2
  fi

  echo "${PSKEL_EXT_DIR}"
}

get_in_tree_ext_dir() {
  echo "/usr/src/php/ext/colopl_bc"
}

sync_ext_into_php_source() {
  SOURCE_DIR="${1}"
  TARGET_DIR="$(get_in_tree_ext_dir)"

  rm -rf "${TARGET_DIR}"
  mkdir -p "${TARGET_DIR}"

  rsync -a \
    --exclude ".git/" \
    --exclude "autom4te.cache/" \
    --exclude "build/" \
    --exclude "modules/" \
    --exclude "*.dep" \
    --exclude "*.la" \
    --exclude "*.lo" \
    --exclude "*.o" \
    --exclude "Makefile" \
    --exclude "Makefile.fragments" \
    --exclude "Makefile.global" \
    --exclude "Makefile.objects" \
    --exclude "config.h" \
    --exclude "config.h.in" \
    --exclude "config.nice" \
    --exclude "config.status" \
    --exclude "configure" \
    --exclude "configure.ac" \
    --exclude "libtool" \
    --exclude "run-tests.php" \
    --exclude "*~" \
    "${SOURCE_DIR}/" "${TARGET_DIR}/"

  echo "${TARGET_DIR}"
}

generate_ext_source_hash() {
  SOURCE_DIR="${1}"

  (
    cd "${SOURCE_DIR}"
    find . \
      \( -path "./autom4te.cache" -o -path "./autom4te.cache/*" -o -path "./build" -o -path "./build/*" -o -path "./modules" -o -path "./modules/*" \) -prune -o \
      -type f \
      ! -name "*.dep" \
      ! -name "*.la" \
      ! -name "*.lo" \
      ! -name "*.o" \
      ! -name "Makefile" \
      ! -name "Makefile.fragments" \
      ! -name "Makefile.global" \
      ! -name "Makefile.objects" \
      ! -name "config.h" \
      ! -name "config.h.in" \
      ! -name "config.nice" \
      ! -name "config.status" \
      ! -name "configure" \
      ! -name "configure.ac" \
      ! -name "libtool" \
      ! -name "run-tests.php" \
      ! -name "*~" \
      -print \
    | LC_ALL=C sort \
    | while IFS= read -r FILE; do
        sha256sum "${FILE}"
      done \
    | sha256sum \
    | cut -d' ' -f1 \
    | cut -c1-16
  )
}

get_php_build_key_file() {
  echo "/usr/local/include/${1}-php/.pskel-build-key"
}

php_build_is_current() {
  PREFIX="${1}"
  BUILD_KEY="${2}"
  BUILD_KEY_FILE="$(get_php_build_key_file "${PREFIX}")"

  if ! test -x "/usr/local/bin/${PREFIX}-php"; then
    return 1
  fi

  if ! test -f "${BUILD_KEY_FILE}"; then
    return 1
  fi

  test "$(cat "${BUILD_KEY_FILE}")" = "${BUILD_KEY}"
}

mark_php_build() {
  PREFIX="${1}"
  BUILD_KEY="${2}"
  INCLUDE_DIR="/usr/local/include/${PREFIX}-php"

  if ! test -d "${INCLUDE_DIR}"; then
    mkdir -p "${INCLUDE_DIR}"
  fi

  printf '%s\n' "${BUILD_KEY}" > "${INCLUDE_DIR}/.pskel-build-key"
}

clear_php_build() {
  PREFIX="${1}"

  rm -f "/usr/local/bin/${PREFIX}-php" "/usr/local/bin/${PREFIX}-phpize" "/usr/local/bin/${PREFIX}-php-config"
  rm -rf "/usr/local/include/${PREFIX}-php"
}

run_in_tree_static_tests() {
  PHP_BIN="${1}"
  TESTS_DIR="${2}"

  if ! "${PHP_BIN}" -m | grep -qx "colopl_bc"; then
    echo "Error: colopl_bc is not built into ${PHP_BIN}" >&2
    return 1
  fi

  (
    if test -n "${TEST_PHP_ARGS}"; then
      eval "set -- ${TEST_PHP_ARGS}"
    else
      set --
    fi

    set -- "$@" --show-diff -q -p "${PHP_BIN}" "${TESTS_DIR}"
    "${PHP_BIN}" "/usr/src/php/run-tests.php" "$@"
  )
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

  mkdir -p "/tmp/pskel_extension_tmp"
  if test "$(/usr/local/bin/php -r 'echo PHP_VERSION_ID;')" -lt "80500"; then
    /usr/local/bin/php "/usr/src/php/ext/ext_skel.php" --ext "${1}" --dir "/tmp/pskel_extension_tmp" "${@}"
  else
    if test -z "${2}"; then
      EXT_VENDOR="pskel"
    else
      EXT_VENDOR="${2}"
    fi
    /usr/local/bin/php "/usr/src/php/ext/ext_skel.php" --vendor "${EXT_VENDOR}" --ext "${1}" --dir "/tmp/pskel_extension_tmp" "${@}"
  fi
  PSKEL_EXT_DIR="$(get_ext_dir --no-init)"
  rm -rf "/tmp/pskel_extension_tmp/${1}/.gitkeep"
  rsync -av "/tmp/pskel_extension_tmp/${1}/" "${PSKEL_EXT_DIR}/"
  rm -rf "${PSKEL_EXT_DIR}/.gitkeep"
}

cmd_test() {
  case "${1}" in
    -h|--help)
      cat << EOF
Usage: ${0} test [debug|debug-static|gcov|valgrind|msan|asan|ubsan|php_binary_name]
Environment variables:
  CFLAGS, CPPFLAGS:     Compilation flags
  TEST_PHP_ARGS:        Test flags
EOF
      return 0
      ;;
    debug-static)
      CC="$(command -v "gcc")"
      CXX="$(command -v "g++")"
      PSKEL_EXT_DIR="$(get_ext_dir)"
      sync_ext_into_php_source "${PSKEL_EXT_DIR}" >/dev/null
      CACHE_KEY_SUFFIX="$(generate_ext_source_hash "${PSKEL_EXT_DIR}")"
      CONFIGURE_OPTS="${CONFIGURE_OPTS} --enable-colopl_bc"
      build_php_if_not_exists "debug-static"
      CMD="$(basename "${CC}")-debug-static-php"
      run_in_tree_static_tests "$(command -v "${CMD}")" "$(get_in_tree_ext_dir)/tests"
      return 0
      ;;
    debug|gcov|valgrind)
      CC="$(command -v "gcc")"
      CXX="$(command -v "g++")"
      case "${1}" in
        debug) build_php_if_not_exists "debug";;
        gcov)
          CONFIGURE_OPTS="${CONFIGURE_OPTS} --enable-gcov"
          build_php_if_not_exists "gcov"
          CFLAGS="${CFLAGS} --coverage"
          ;;
        valgrind)
          CONFIGURE_OPTS="${CONFIGURE_OPTS} --with-valgrind"
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
          CONFIGURE_OPTS="${CONFIGURE_OPTS} --enable-memory-sanitizer"
          build_php_if_not_exists "msan"
          CFLAGS="${CFLAGS} -fsanitize=memory"
          LDFLAGS="${LDFLAGS} -fsanitize=memory"
          ;;
        asan)
          CONFIGURE_OPTS="${CONFIGURE_OPTS} --enable-address-sanitizer"
          build_php_if_not_exists "asan"
          CFLAGS="${CFLAGS} -fsanitize=address"
          LDFLAGS="${LDFLAGS} -fsanitize=address"
          ;;
        ubsan)
          CONFIGURE_OPTS="${CONFIGURE_OPTS} --enable-undefined-sanitizer"
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
    if test "$("${CMD}" -r "echo PHP_VERSION_ID;")" -lt "80400"; then
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
  PREFIX="$(basename "${CC}")-${1}"
  BUILD_KEY="$(generate_cache_key "${1}" "${CC}")"

  if php_build_is_current "${PREFIX}" "${BUILD_KEY}"; then
    return 0
  fi

  clear_php_build "${PREFIX}"

  if test -n "${GITHUB_ACTIONS}" && test -d "${PHP_CACHE_DIR}"; then
    if check_and_restore_cached_php "${PREFIX}" "${BUILD_KEY}"; then
      return 0
    fi

    clear_php_build "${PREFIX}"
  fi

  CC="${CC}" \
  CXX="${CXX}" \
  CFLAGS="-DZEND_TRACK_ARENA_ALLOC" \
  CPPFLAGS="${CFLAGS}" \
  LDFLAGS="${LDFLAGS}" \
  CONFIGURE_OPTS="${CONFIGURE_OPTS} --enable-debug $(php -r "echo (bool)PHP_ZTS ? '--enable-zts' : '';") --enable-option-checking=fatal --disable-phpdbg --disable-cgi --disable-fpm --enable-cli --without-pcre-jit --disable-opcache-jit --disable-zend-max-execution-timers" \
  cmd_build "${PREFIX}"

  mark_php_build "${PREFIX}" "${BUILD_KEY}"

  if test -n "${GITHUB_ACTIONS}" && test -d "${PHP_CACHE_DIR}"; then
    cache_php_build "${PREFIX}" "${BUILD_KEY}"
  fi
}

check_and_restore_cached_php() {
  PREFIX="${1}"
  BUILD_KEY="${2}"
  CACHE_DIR="${PHP_CACHE_DIR}/${BUILD_KEY}"

  if test -f "${CACHE_DIR}/.build_complete"; then
    for BIN in php phpize php-config; do
      if test -f "${CACHE_DIR}/usr/local/bin/${PREFIX}-${BIN}"; then
        ln -sf "${CACHE_DIR}/usr/local/bin/${PREFIX}-${BIN}" "/usr/local/bin/${PREFIX}-${BIN}"
      fi
    done

    if test -d "${CACHE_DIR}/usr/local/include/${PREFIX}-php"; then
      ln -sf "${CACHE_DIR}/usr/local/include/${PREFIX}-php" "/usr/local/include/${PREFIX}-php"
    fi

    if php_build_is_current "${PREFIX}" "${BUILD_KEY}"; then
      echo "[Pskel > Cache] Restored PHP header and binary: ${PREFIX}-php" >&2

      return 0
    fi
  fi

  return 1
}

generate_cache_key() {
  BUILD_TYPE="${1}"
  COMPILER="${2}"

  PHP_VERSION="$(php -r 'echo PHP_VERSION;')"
  PHP_ZTS="$(php -r 'echo (bool)PHP_ZTS ? "zts" : "nts";')"

  if test -n "${CONTAINER_IMAGE_HASH}"; then
    IMAGE_HASH="${CONTAINER_IMAGE_HASH}"
  else
    IMAGE_HASH=""
    if test -d "/usr/src/php"; then
      IMAGE_HASH="$(cd /usr/src/php && find . -type f \( -name "*.c" -o -name "*.h" \) -exec sha256sum {} \; | sha256sum | cut -d' ' -f1 | cut -c1-16)"
    fi
  fi

  EXTRA_KEY=""
  if test -n "${CACHE_KEY_SUFFIX}"; then
    EXTRA_KEY="-${CACHE_KEY_SUFFIX}"
  fi

  echo "php-${PHP_VERSION}-${PHP_ZTS}-${BUILD_TYPE}-${COMPILER}-${IMAGE_HASH}${EXTRA_KEY}"
}

cache_php_build() {
  PREFIX="${1}"
  BUILD_KEY="${2}"
  CACHE_DIR="${PHP_CACHE_DIR}/${BUILD_KEY}"

  mkdir -p "${CACHE_DIR}/usr/local/bin"

  for BIN in php phpize php-config; do
    if test -f "/usr/local/bin/${PREFIX}-${BIN}"; then
      cp -a "/usr/local/bin/${PREFIX}-${BIN}" "${CACHE_DIR}/usr/local/bin/"
    fi
    done

  if test -d "/usr/local/include/${PREFIX}-php"; then
    mkdir -p "${CACHE_DIR}/usr/local/include"
    cp -a "/usr/local/include/${PREFIX}-php" "${CACHE_DIR}/usr/local/include/"
  fi

  touch "${CACHE_DIR}/.build_complete"
  echo "[Pskel > Cache] Cached PHP header and binary: ${PREFIX}-php" >&2
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

  lcov --capture --directory "${PSKEL_EXT_DIR}" \
    ${LCOV_OPTS} \
    --exclude "/usr/local/include/*" \
    --output-file "${PSKEL_EXT_DIR}/lcov.info"
  lcov --list "${PSKEL_EXT_DIR}/lcov.info"
}

if test $# -eq 0; then
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
