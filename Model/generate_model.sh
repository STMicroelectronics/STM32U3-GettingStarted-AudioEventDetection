#!/bin/sh
# Check ST Edge AI version
required_major=4
required_minor=0

version_output=$(stedgeai --version 2>&1)
version_line=$(echo "$version_output" | grep -Eo 'ST Edge AI Core v[0-9]+\.[0-9]+')
if [ -z "$version_line" ]; then
	echo "Error: Could not determine ST Edge AI version." >&2
	exit 1
fi
version=$(echo "$version_line" | grep -Eo '[0-9]+\.[0-9]+')
major=$(echo $version | cut -d. -f1)
minor=$(echo $version | cut -d. -f2)

if [ "$major" -lt "$required_major" ] || { [ "$major" -eq "$required_major" ] && [ "$minor" -lt "$required_minor" ]; }; then
	echo "Error: ST Edge AI version must be at least v$required_major.$required_minor. Found v$version." >&2
	exit 1
fi

stedgeai.exe  generate -m yamnet_256_64x96_tl_int8_5_classes.tflite --target stm32u3 --c-api st-ai --hsp 4096 -O time

#--input-data-type uint8 --output-data-type int8
cp st_ai_output/* .