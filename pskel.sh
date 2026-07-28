#!/bin/sh

set -e

get_pskel_root_dir() {
  PSKEL_PATH="${0}"

  case "${PSKEL_PATH}" in
    */*)
      ;;
    *)
      PSKEL_PATH="$(command -v "${PSKEL_PATH}")"
      ;;
  esac

  if type "readlink" >/dev/null 2>&1; then
    PSKEL_PATH="$(readlink -f "${PSKEL_PATH}" 2>/dev/null || printf '%s' "${PSKEL_PATH}")"
  fi

  CDPATH='' cd -- "$(dirname -- "${PSKEL_PATH}")" && pwd
}

find_workspace_dir_from_pwd() {
  SEARCH_DIR="${PWD}"

  while test -n "${SEARCH_DIR}"; do
    if test -f "${SEARCH_DIR}/pskel.sh" && test -f "${SEARCH_DIR}/.pskel/LICENSE.template"; then
      echo "${SEARCH_DIR}"
      return 0
    fi

    if test "${SEARCH_DIR}" = "/"; then
      break
    fi

    SEARCH_DIR="$(dirname -- "${SEARCH_DIR}")"
  done

  return 1
}

get_workspace_dir() {
  if test -n "${PSKEL_WORKSPACE_DIR}" && test -d "${PSKEL_WORKSPACE_DIR}"; then
    echo "${PSKEL_WORKSPACE_DIR}"
  elif PWD_WORKSPACE_DIR="$(find_workspace_dir_from_pwd 2>/dev/null)" && test -n "${PWD_WORKSPACE_DIR}"; then
    echo "${PWD_WORKSPACE_DIR}"
  elif test -d "${CODESPACE_VSCODE_FOLDER}"; then
    echo "${CODESPACE_VSCODE_FOLDER}"
  elif test -f "/workspace/pskel.sh" && test -f "/workspace/.pskel/LICENSE.template"; then
    echo "/workspace"
  elif test -d "/workspaces/pskel"; then
    echo "/workspaces/pskel"
  else
    PSKEL_ROOT_DIR="$(get_pskel_root_dir)" || return 1
    if test -f "${PSKEL_ROOT_DIR}/pskel.sh"; then
      echo "${PSKEL_ROOT_DIR}"
    else
      echo "Error: Workspace root not found." >&2
      return 1
    fi
  fi
}


get_project_license_template_path() {
  PSKEL_ROOT_DIR="$(get_pskel_root_dir)" || return 1
  LICENSE_TEMPLATE_PATH="${PSKEL_ROOT_DIR}/.pskel/LICENSE.template"

  if test -f "${LICENSE_TEMPLATE_PATH}"; then
    echo "${LICENSE_TEMPLATE_PATH}"
  else
    echo "Error: LICENSE template not found." >&2
    return 1
  fi
}

escape_sed_replacement() {
  printf '%s' "${1}" | sed -e 's/[\\/&]/\\&/g'
}

slugify_vendor_name() {
  printf '%s' "${1}" \
    | tr '[:upper:]' '[:lower:]' \
    | sed \
      -e 's/[^a-z0-9._-]/-/g' \
      -e 's/-\{2,\}/-/g' \
      -e 's/^[._-]*//' \
      -e 's/[._-]*$//'
}

create_project_license() {
  LICENSE_TEMPLATE_PATH="$(get_project_license_template_path)" || return 1
  WORKSPACE_DIR="$(get_workspace_dir)" || return 1
  LICENSE_YEAR="$(date -u "+%Y")"
  LICENSE_YEAR_ESCAPED="$(escape_sed_replacement "${LICENSE_YEAR}")"
  EXT_VENDOR_DISPLAY_ESCAPED="$(escape_sed_replacement "${EXT_VENDOR_DISPLAY}")"

  sed \
    -e "s/%YEAR%/${LICENSE_YEAR_ESCAPED}/g" \
    -e "s/%VENDOR%/${EXT_VENDOR_DISPLAY_ESCAPED}/g" \
    "${LICENSE_TEMPLATE_PATH}" > "${WORKSPACE_DIR}/LICENSE"
}

create_project_composer_manifest() {
  WORKSPACE_DIR="$(get_workspace_dir)" || return 1
  EXT_COMPOSER_PATH="${PSKEL_TMP_DIR}/${EXT_NAME}/composer.json"

  if ! test -f "${EXT_COMPOSER_PATH}"; then
    echo "Error: composer.json template not found." >&2
    return 1
  fi

  EXT_COMPOSER_PATH="${EXT_COMPOSER_PATH}" \
  PROJECT_COMPOSER_PATH="${WORKSPACE_DIR}/composer.json" \
  EXT_NAME="${EXT_NAME}" \
  /usr/local/bin/php -r '
    $sourcePath = getenv("EXT_COMPOSER_PATH");
    $targetPath = getenv("PROJECT_COMPOSER_PATH");
    $extensionName = getenv("EXT_NAME");

    $manifest = json_decode((string) file_get_contents($sourcePath), true);
    if (!is_array($manifest)) {
        fwrite(STDERR, "Error: Failed to parse composer.json template.\n");
        exit(1);
    }

    if (!isset($manifest["php-ext"]) || !is_array($manifest["php-ext"])) {
        $manifest["php-ext"] = [];
    }

    $manifest["php-ext"]["extension-name"] = $extensionName;
    $manifest["php-ext"]["build-path"] = "ext";
    $manifest["php-ext"]["download-url-method"] = [
        "pre-packaged-binary",
        "pre-packaged-source",
        "composer-default",
    ];

    $encoded = json_encode($manifest, JSON_PRETTY_PRINT | JSON_UNESCAPED_SLASHES);
    if (!is_string($encoded)) {
        fwrite(STDERR, "Error: Failed to encode project composer.json.\n");
        exit(1);
    }

    file_put_contents($targetPath, $encoded . PHP_EOL);
  ' || return 1

  rm -f "${EXT_COMPOSER_PATH}"
}

get_ext_dir() {
  WORKSPACE_DIR="$(get_workspace_dir 2>/dev/null || true)"

  if test -n "${WORKSPACE_DIR}" && test -d "${WORKSPACE_DIR}/ext"; then
    if test -f "${WORKSPACE_DIR}/ext/.gitkeep" && test "$(cat "${WORKSPACE_DIR}/ext/.gitkeep")" = "pskel_uninitialized" && test "${1}" != "--no-init"; then
      echo "[Pskel] Uninitialized project detected, initializing default skeleton." >&2
      cmd_init "skeleton" >&2
    fi
    echo "[Pskel] Workspace extension directory detected, using \"${WORKSPACE_DIR}/ext\"." >&2
    PSKEL_EXT_DIR="${WORKSPACE_DIR}/ext"
  elif test -f "/ext/.gitkeep" && test "$(cat "/ext/.gitkeep")" = "pskel_uninitialized" && test "${1}" != "--no-init"; then
    echo "[Pskel] Uninitialized project detected, initializing default skeleton." >&2
    cmd_init "skeleton" >&2
    PSKEL_EXT_DIR="/ext"
  elif test -d "/ext"; then
    PSKEL_EXT_DIR="/ext"
  else
    echo "Error: Extension directory not found." >&2
    return 1
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
    init               create new extension
    test               test extension
    build              build PHP runtime
    clean-build-cache  remove built PHP runtimes and cache
    coverage           generate code coverage
    coverage-report    merge lcov .info files and generate HTML report
    package            build and package a PIE pre-packaged binary
EOF
}

cmd_init() {
  case "${1}" in
    -h|--help)
      cat << EOF
Usage: ${0} init <extension_name> [vendor_name] [ext_skel.php options...]
EOF
      return 0
      ;;
    "")
      echo "Error: Extension name is required." >&2
      return 1
      ;;
  esac

  EXT_NAME="${1}"
  shift

  if test -n "${1}" && test "${1}" = "${1#-}"; then
    EXT_VENDOR_DISPLAY="${1}"
    shift
  else
    EXT_VENDOR_DISPLAY="pskel"
  fi

  PSKEL_TMP_DIR="$(mktemp -d "${TMPDIR:-/tmp}/pskel_extension_tmp.XXXXXX")"
  cleanup_pskel_tmp_dir() {
    if test -n "${PSKEL_TMP_DIR}" && test -d "${PSKEL_TMP_DIR}"; then
      rm -rf "${PSKEL_TMP_DIR}"
    fi
  }
  trap cleanup_pskel_tmp_dir EXIT HUP INT TERM

  EXT_VENDOR="$(slugify_vendor_name "${EXT_VENDOR_DISPLAY}")"

  case "${EXT_NAME}" in
    *[!-a-z0-9_.]*)
      echo "Error: Extension name must only contain lowercase letters, numbers, hyphens, underscores, and dots." >&2
      return 1
      ;;
  esac

  if test -z "${EXT_VENDOR}"; then
    echo "Error: Vendor name must contain at least one ASCII letter or number." >&2
    return 1
  fi

  if test "$(/usr/local/bin/php -r 'echo PHP_VERSION_ID;')" -lt "80500"; then
    /usr/local/bin/php "/usr/src/php/ext/ext_skel.php" --ext "${EXT_NAME}" --dir "${PSKEL_TMP_DIR}" "${@}"
    cat > "${PSKEL_TMP_DIR}/${EXT_NAME}/composer.json" << COMPOSER_EOF
{
    "name": "${EXT_VENDOR}/${EXT_NAME}",
    "type": "php-ext",
    "license": "BSD-3-Clause",
    "description": "Describe your extension here",
    "require": {
        "php": ">= 8.1"
    },
    "php-ext": {
        "extension-name": "${EXT_NAME}",
        "configure-options": [
            {
                "name": "enable-${EXT_NAME}",
                "needs-value": false,
                "description": "whether to enable ${EXT_NAME} support"
            }
        ]
    }
}
COMPOSER_EOF
  else
    /usr/local/bin/php "/usr/src/php/ext/ext_skel.php" --vendor "${EXT_VENDOR}" --ext "${EXT_NAME}" --dir "${PSKEL_TMP_DIR}" "${@}"
  fi

  create_project_composer_manifest
  create_project_license

  PSKEL_EXT_DIR="$(get_ext_dir --no-init)"
  rm -f "${PSKEL_EXT_DIR}/composer.json"
  rm -rf "${PSKEL_TMP_DIR}/${EXT_NAME}/.gitkeep"
  cp -a "${PSKEL_TMP_DIR}/${EXT_NAME}/." "${PSKEL_EXT_DIR}/"
  rm -rf "${PSKEL_EXT_DIR}/.gitkeep"
}

cmd_test() {
  PSKEL_TRACK_ARENA_ALLOC=0

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
      PSKEL_TRACK_ARENA_ALLOC=1
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
      PSKEL_TRACK_ARENA_ALLOC=1
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
    if ! type "${BIN}" >/dev/null 2>&1; then
      echo "Error: Invalid argument '${CMD}', executable file not found" >&2
      exit 1
    fi
  done

  PSKEL_EXT_DIR="$(get_ext_dir)"

  if test "${PSKEL_TRACK_ARENA_ALLOC}" = "1"; then
    CFLAGS="${CFLAGS} -DZEND_TRACK_ARENA_ALLOC"
    CPPFLAGS="${CPPFLAGS} -DZEND_TRACK_ARENA_ALLOC"
  fi

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
        remove_path_if_exists "/usr/local/bin/${PREFIX}-${BIN}"
        ln -sf "${CACHE_DIR}/usr/local/bin/${PREFIX}-${BIN}" "/usr/local/bin/${PREFIX}-${BIN}"
      fi
    done

    if test -d "${CACHE_DIR}/usr/local/include/${PREFIX}-php"; then
      remove_path_if_exists "/usr/local/include/${PREFIX}-php"
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
  ARCH="$(uname -m)"

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

  echo "php-${PHP_VERSION}-${PHP_ZTS}-${BUILD_TYPE}-${COMPILER}-${ARCH}-${IMAGE_HASH}${EXTRA_KEY}"
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

remove_path_if_exists() {
  if test -e "${1}" || test -L "${1}"; then
    rm -rf "${1}"
    echo "[Pskel > Clean] Removed: ${1}" >&2
  fi
}

remove_built_php_runtime() {
  PREFIX="${1}"

  for BIN in php phpize php-config; do
    remove_path_if_exists "/usr/local/bin/${PREFIX}-${BIN}"
  done

  remove_path_if_exists "/usr/local/include/${PREFIX}-php"
}

cmd_clean_build_cache() {
  case "${1}" in
    -h|--help)
      cat << EOF
Usage: ${0} clean-build-cache
Environment variables:
  PHP_CACHE_DIR:        PHP runtime cache directory
EOF
      return 0
      ;;
    ?*)
      echo "Error: clean-build-cache does not accept arguments." >&2
      return 1
      ;;
  esac

  for PREFIX in \
    gcc-debug gcc-debug-static gcc-gcov gcc-valgrind \
    clang-msan clang-asan clang-ubsan
  do
    remove_built_php_runtime "${PREFIX}"
  done

  if test -n "${PHP_CACHE_DIR}" && test -d "${PHP_CACHE_DIR}"; then
    for CACHE_ENTRY in "${PHP_CACHE_DIR}"/*; do
      if test -f "${CACHE_ENTRY}/.build_complete"; then
        if test -d "${CACHE_ENTRY}/usr/local/bin"; then
          for BIN in "${CACHE_ENTRY}/usr/local/bin/"*; do
            if test -f "${BIN}"; then
              remove_path_if_exists "/usr/local/bin/$(basename "${BIN}")"
            fi
          done
        fi

        if test -d "${CACHE_ENTRY}/usr/local/include"; then
          for INCLUDE_DIR in "${CACHE_ENTRY}/usr/local/include/"*; do
            if test -d "${INCLUDE_DIR}"; then
              remove_path_if_exists "/usr/local/include/$(basename "${INCLUDE_DIR}")"
            fi
          done
        fi

        remove_path_if_exists "${CACHE_ENTRY}"
      fi
    done
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

  lcov --capture --directory "${PSKEL_EXT_DIR}" \
    ${LCOV_OPTS} \
    --exclude "/usr/local/include/*" \
    --output-file "${PSKEL_EXT_DIR}/lcov.info.tmp"

  cat "${PSKEL_EXT_DIR}/lcov.info.tmp" > "${PSKEL_EXT_DIR}/lcov.info" \
    && rm -f "${PSKEL_EXT_DIR}/lcov.info.tmp"

  lcov --list "${PSKEL_EXT_DIR}/lcov.info"
}

cmd_coverage_report() {
  case "${1}" in
    -h|--help)
      cat << EOF
Usage: ${0} coverage-report <info_dir> <output_dir>
Merge all lcov .info files in <info_dir> and generate an HTML report in
<output_dir> (merged data is written to <output_dir>/total.info). Shards must
be captured with the same container image so lcov versions match.
Environment variables:
  GENHTML_OPTS:    genhtml options
EOF
      return 0
      ;;
  esac

  INFO_DIR="${1}"
  OUTPUT_DIR="${2}"

  if test -z "${INFO_DIR}" || test -z "${OUTPUT_DIR}"; then
    echo "Error: info_dir and output_dir are required." >&2
    return 1
  fi

  if ! test -d "${INFO_DIR}"; then
    echo "Error: info_dir '${INFO_DIR}' does not exist." >&2
    return 1
  fi

  INFO_FILES=""
  for INFO_FILE in "${INFO_DIR}"/*.info; do
    if ! test -f "${INFO_FILE}"; then
      continue
    fi
    if test "$(basename "${INFO_FILE}")" = "total.info"; then
      echo "[Pskel > Coverage] Skipping previously merged file: ${INFO_FILE}" >&2
      continue
    fi
    INFO_FILES="${INFO_FILES} ${INFO_FILE}"
  done

  if test -z "${INFO_FILES}"; then
    echo "Error: no .info files found in '${INFO_DIR}'." >&2
    return 1
  fi

  MERGE_ARGS=""
  for INFO_FILE in ${INFO_FILES}; do
    verify_function_records "${INFO_FILE}" || return 1
    lcov --summary "${INFO_FILE}" || return 1
    MERGE_ARGS="${MERGE_ARGS} -a ${INFO_FILE}"
  done

  mkdir -p "${OUTPUT_DIR}"

  lcov ${MERGE_ARGS} --output-file "${OUTPUT_DIR}/total.info" || return 1

  verify_function_records "${OUTPUT_DIR}/total.info" || return 1

  genhtml "${OUTPUT_DIR}/total.info" \
    ${GENHTML_OPTS} \
    --output-directory "${OUTPUT_DIR}" \
    --title "Extension code coverage" || return 1

  lcov --summary "${OUTPUT_DIR}/total.info"
}

verify_function_records() {
  for RECORD in "FNL:" "FNA:" "FNF:" "FNH:"; do
    if ! grep -q "^${RECORD}" "${1}"; then
      echo "Error: missing ${RECORD} records in '${1}'." >&2
      return 1
    fi
  done
}

cmd_package() {
  case "${1}" in
    -h|--help)
      cat << EOF
Usage: ${0} package <release_tag> [output_dir] [extension_name]
Build the extension against the environment PHP and package the shared object
as a PIE pre-packaged binary, using the same naming convention as
php/pie-ext-binary-builder:
  php_{ext}-{tag}_php{maj.min}-{arch}-{os}-{libc}[-debug][-zts].zip
Environment variables:
  EXT_CONFIGURE_OPTS:    extra ./configure options
EOF
      return 0
      ;;
    "")
      echo "Error: release tag is required." >&2
      return 1
      ;;
  esac

  RELEASE_TAG="${1}"
  OUTPUT_DIR="${2:-.}"
  PKG_EXT_NAME="${3}"

  PSKEL_EXT_DIR="$(get_ext_dir)"

  if test -z "${PKG_EXT_NAME}"; then
    PKG_WORKSPACE_DIR="$(get_workspace_dir 2>/dev/null)" || PKG_WORKSPACE_DIR=""
    if test -n "${PKG_WORKSPACE_DIR}" && test -f "${PKG_WORKSPACE_DIR}/composer.json"; then
      PKG_EXT_NAME="$(COMPOSER_JSON_PATH="${PKG_WORKSPACE_DIR}/composer.json" php -n -r '
        $manifest = json_decode((string) file_get_contents(getenv("COMPOSER_JSON_PATH")), true);
        $name = $manifest["php-ext"]["extension-name"] ?? basename((string) ($manifest["name"] ?? ""));
        echo preg_replace("/^ext-/", "", (string) $name);
      ')"
    fi
  fi
  if test -z "${PKG_EXT_NAME}"; then
    PKG_EXT_NAME="$(sed -n 's/^PHP_NEW_EXTENSION(\[\{0,1\}\([A-Za-z0-9_]*\).*/\1/p' "${PSKEL_EXT_DIR}/config.m4" | head -n 1)"
  fi
  if test -z "${PKG_EXT_NAME}"; then
    echo "Error: could not determine extension name; pass it as the third argument." >&2
    return 1
  fi

  cd "${PSKEL_EXT_DIR}"
    phpize
    if test "$(uname -s)" != "Darwin" && test "$(php -r "echo PHP_VERSION_ID;")" -lt "80400"; then
      patch "./build/ltmain.sh" "./../patches/ltmain.sh.patch"
      echo "[Pskel] ltmain.sh patched" >&2
    fi
    ./configure --with-php-config="$(command -v "php-config")" ${EXT_CONFIGURE_OPTS}
    make clean
    make -j"$(nproc 2>/dev/null || sysctl -n hw.ncpu)"
  cd -

  if ! php -n -d "extension=${PSKEL_EXT_DIR}/modules/${PKG_EXT_NAME}.so" -m | grep -q "^${PKG_EXT_NAME}\$"; then
    echo "Error: built ${PKG_EXT_NAME}.so failed to load." >&2
    return 1
  fi

  PKG_PHP_MAJMIN="$(php-config --version | cut -d. -f1,2)"
  case "$(uname -m)" in
    x86_64|amd64) PKG_ARCH="x86_64";;
    aarch64|arm64) PKG_ARCH="arm64";;
    i386|i486|i586|i686) PKG_ARCH="x86";;
    *) echo "Error: unsupported architecture '$(uname -m)'." >&2; return 1;;
  esac
  case "$(uname -s)" in
    Linux) PKG_OS="linux";;
    Darwin) PKG_OS="darwin";;
    *) echo "Error: unsupported operating system '$(uname -s)'." >&2; return 1;;
  esac
  if test "${PKG_OS}" = "darwin"; then
    PKG_LIBC="bsdlibc"
  elif test -f "/etc/alpine-release" || ldd --version 2>&1 | grep -qi "musl"; then
    PKG_LIBC="musl"
  else
    PKG_LIBC="glibc"
  fi
  PKG_DEBUG="$(php -n -r 'echo PHP_DEBUG ? "-debug" : "";')"
  PKG_ZTS="$(php -n -r 'echo ZEND_THREAD_SAFE ? "-zts" : "";')"

  PKG_NAME="php_${PKG_EXT_NAME}-${RELEASE_TAG}_php${PKG_PHP_MAJMIN}-${PKG_ARCH}-${PKG_OS}-${PKG_LIBC}${PKG_DEBUG}${PKG_ZTS}.zip"

  mkdir -p "${OUTPUT_DIR}"
  rm -f "${OUTPUT_DIR}/${PKG_NAME}"
  zip -j "${OUTPUT_DIR}/${PKG_NAME}" "${PSKEL_EXT_DIR}/modules/${PKG_EXT_NAME}.so"

  echo "[Pskel > Package] Created: ${OUTPUT_DIR}/${PKG_NAME}"
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
  clean-build-cache) shift; cmd_clean_build_cache "${@}";;
  coverage) shift; cmd_coverage "${@}";;
  coverage-report) shift; cmd_coverage_report "${@}";;
  package) shift; cmd_package "${@}";;
  *)
    echo "${0} error: invalid command: '${1}'" >&2
    cmd_usage
    exit 1
    ;;
esac
