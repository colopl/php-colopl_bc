#!/bin/sh -eux

: "${VERSION:?VERSION is required}"

SOURCE_DIR="/tmp/src"
PACKAGING_DIR="/tmp/build/ubuntu2204_sury84/debian"
ARTIFACTS_DIR="/tmp/artifacts"

rm -rf "${SOURCE_DIR}/debian"
install -d "${SOURCE_DIR}/debian" "${ARTIFACTS_DIR}"
find "${ARTIFACTS_DIR}" -mindepth 1 -maxdepth 1 -delete

cp -a "${PACKAGING_DIR}/." "${SOURCE_DIR}/debian/"
sed "s/@VERSION@/${VERSION}/g" "${PACKAGING_DIR}/changelog.in" > "${SOURCE_DIR}/debian/changelog"
rm -f "${SOURCE_DIR}/debian/changelog.in"

cd "${SOURCE_DIR}"
dpkg-buildpackage -us -uc -b
find /tmp -maxdepth 1 -type f \( -name '*.deb' -o -name '*.buildinfo' -o -name '*.changes' \) -exec cp {} "${ARTIFACTS_DIR}/" \;
