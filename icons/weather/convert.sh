#!/bin/bash

set -xe

EXPORT_SIZE_LARGE=128
EXPORT_SIZE_SMALL=64

for file in `ls *svg`; do
  base_file=$(basename $file ".svg")

  inkscape --without-gui "${base_file}.svg" -w ${EXPORT_SIZE_LARGE} -h ${EXPORT_SIZE_LARGE} -b ffffff -o "${base_file}_large.png" 2> /dev/null
  inkscape --without-gui "${base_file}.svg" -w ${EXPORT_SIZE_SMALL} -h ${EXPORT_SIZE_SMALL} -b ffffff -o "${base_file}_small.png" 2> /dev/null

  rm -f "${base_file}_large.bmp" "${base_file}_small.bmp"

  convert "${base_file}_large.png" -colors 256 -compress none -depth 8 -type palette "${base_file}_large.bmp"
  convert "${base_file}_small.png" -colors 256 -compress none -depth 8 -type palette "${base_file}_small.bmp"

  rm -f "${base_file}_large.png" "${base_file}_small.png"
done
