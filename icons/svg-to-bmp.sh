#!/bin/bash

# Converts an SVG to a bitmap of the specified size

set -xe

SIZE_PX=$1
INPUT=$2
BASENAME=$(basename $INPUT ".svg")
OUTPUT_BASE=$(dirname $(realpath --relative-to=$(pwd) ${INPUT}))
OUTPUT="${BASENAME}_${SIZE_PX}px.bmp"

rm -f "${OUTPUT}"
convert "${INPUT}" -colors 256 -compress none -depth 8 -resize ${SIZE_PX}x${SIZE_PX} -type palette "${OUTPUT_BASE}/${OUTPUT}"
