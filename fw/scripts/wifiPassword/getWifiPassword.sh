#!/bin/bash

wordz=(
  "Zort"
  "Blerp"
  "Flib"
  "Wum"
  "Jig"
  "Splat"
  "Ploof"
  "Gag"
  "Fizz"
  "Whim"
  "Drip"
  "Zing"
  "Flip"
  "Snap"
  "Crap"
  "Wiz"
  "Fop"
  "Pup"
  "Zip"
  "Zap"
  "Bop"
  "Pop"
  "Fad"
  "Vat"
  "Jam"
  "Gum"
  "Hum"
  "Wham"
  "Chum"
  "Prout"
)

# Function to convert hex string to decimal bytes
hex_to_bytes() {
  local hex_string="$1"
  local -n byte_array=$2
  for (( i=0; i<${#hex_string}; i+=2 )); do
    byte_array+=($((16#${hex_string:$i:2})))
  done
}

# Function to calculate the password
get_password() {
  local chipid=($@)
  local m1=$(( (${chipid[0]} + ${chipid[3]}) % 97 ))
  local m2=$(( (${chipid[1]} + ${chipid[4]}) % 97 ))
  local m3=$(( (${chipid[2]} + ${chipid[5]}) % 97 ))

  # Format m1, m2, and m3 as two-digit hexadecimal values
  local m1_hex=$(printf "%02x" $m1)
  local m2_hex=$(printf "%02x" $m2)
  local m3_hex=$(printf "%02x" $m3)

  local password="${wordz[m1 % 29]}$m1_hex${wordz[m2 % 29]}$m2_hex${wordz[m3 % 29]}$m3_hex"
  echo $password
}

which idf.py > /dev/null 2>&1
if [[ $? -eq 1 ]]; then
  echo "do . ~/esp/esp-idf/export.sh before"
  exit 1
fi

echo "[.] Retrieving CHIP ID..."
CHIPID=$(esptool.py chip_id | grep "MAC:" | tail -n1 | cut -d: -f2- | tr -d :)
BADGEID=${CHIPID:(-6)}
NFCUID=${CHIPID:(-8)}
ESSID="CoolKidsBadge"${BADGEID}
echo "[.] Generating procedural password..."
# Convert hex string to bytes
chipid_bytes=()
hex_to_bytes $CHIPID chipid_bytes

# Call the function with the byte values
PASSWORD=$( get_password "${chipid_bytes[@]}" )
echo "........ CHIP ID: "$(tput bold)${CHIPID^^}$(tput sgr0)
echo "........ BADGE ID: "$(tput bold)${BADGEID^^}$(tput sgr0)
echo "........ NFC ID: "$(tput bold)${NFCUID^^}$(tput sgr0)
echo "........ ESSID: "$(tput bold)${ESSID}$(tput sgr0)
echo "........ PASSWORD: "$(tput bold)${PASSWORD}$(tput sgr0)
