#!/bin/bash
# -------------------------------------------------------
#  ▞▚ ▚▗▘▛▚ ▛▀ ▛▚ ▛▚ ▌▐ ▙▐ ▌▞  ▞▚ ▞▚ ▙▟ ▛▚ ▞▚ ▙▐ ▚▗▘
#  ▌▗  ▌ ▛▚ ▛  ▛▚ ▛▘ ▌▐ ▌▜ ▛▖  ▌▗ ▌▐ ▌▐ ▛▘ ▛▜ ▌▜  ▌
#  ▝▘  ▘ ▀▘ ▀▀ ▘▝ ▘  ▝▘ ▘▝ ▘▝ ▘▝▘ ▝▘ ▘▝ ▘  ▘▝ ▘▝  ▘
# Copyright© 2024 - Cyberpunk.Company - GPLv3
# This program is free software
# See LICENSE.txt - https://cyberpunk.company/tixlegeek
# -------------------------------------------------------

# Define the names of the output files
CERT_PATH="./certs"
PRIVATE_KEY_FILE="${CERT_PATH}/prvtkey.pem"
CERTIFICATE_FILE="${CERT_PATH}/servercert.pem"

# Create destination path
[[ ! -d ${CERT_PATH} ]] && mkdir -p ${CERT_PATH} && (
  echo "[!] Could not create destination path: ${CERT_PATH}" >&2
  return 1
)

# Remove old certificates
echo "[.] Removing old certificates..."
[[ -f ${PRIVATE_KEY_FILE} ]] && rm ${PRIVATE_KEY_FILE}
[[ -f ${CERTIFICATE_FILE} ]] && rm ${CERTIFICATE_FILE}

# Certificate details - customize these variables
COUNTRY="FR"
STATE="-"
LOCALITY="-"
ORGANIZATION="CYBERPUNK.COMPANY"
ORGANIZATIONAL_UNIT="CYBERPUNK.COMPANY"
COMMON_NAME="cyberpunk.company"

# Generate a new private key
openssl genpkey -algorithm RSA -out ${PRIVATE_KEY_FILE}

# Generate a self-signed certificate from the private key
openssl req -config ssl_cert_gen.conf -new -x509 -sha256 -key ${PRIVATE_KEY_FILE} -out ${CERTIFICATE_FILE} -days 365 \
    -subj "/C=${COUNTRY}/ST=${STATE}/L=${LOCALITY}/O=${ORGANIZATION}/OU=${ORGANIZATIONAL_UNIT}/CN=${COMMON_NAME}"

[[ ! -f ${CERTIFICATE_FILE} || ! -f ${PRIVATE_KEY_FILE} ]] && (
  echo "[!] Could not create cert!" >&2
  return 1
)

echo "Private key stored in: ${PRIVATE_KEY_FILE}"
echo "Self-signed certificate stored in: ${CERTIFICATE_FILE}"
