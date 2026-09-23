#!/usr/bin/env bash
# Self-signed cert для fake-Bambu MQTT broker.
# ESP-side BambuClient делает setInsecure(), publisher — CERT_NONE: личность
# сервера никто не проверяет, поэтому IP в subjectAltName ниже не имеет
# значения и под свою сеть его подгонять не нужно.
set -e
cd "$(dirname "$0")"

if [[ -f cert.pem && -f key.pem ]]; then
  echo "cert.pem + key.pem уже существуют — пропускаю генерацию."
  exit 0
fi

openssl req -x509 -newkey rsa:2048 \
  -keyout key.pem -out cert.pem \
  -days 3650 -nodes \
  -subj "/CN=fake-bambu" \
  -addext "subjectAltName=IP:127.0.0.1,IP:192.168.0.171,DNS:localhost"

chmod 600 key.pem
echo "✓ cert.pem + key.pem созданы"
