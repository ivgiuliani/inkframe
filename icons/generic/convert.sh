#!/bin/bash

set -xe

EXPORT_SIZE_LARGE=32
EXPORT_SIZE_SMALL=24

for file in `ls *svg`; do
  base_file=$(basename $file ".svg")

  rm -f "${base_file}_large.bmp" "${base_file}_small.bmp"

  convert "${base_file}.svg" -colors 2 -compress none -depth 8 -resize ${EXPORT_SIZE_LARGE}x${EXPORT_SIZE_LARGE} -type palette "${base_file}_large.bmp"
  convert "${base_file}.svg" -colors 2 -compress none -depth 8 -resize ${EXPORT_SIZE_SMALL}x${EXPORT_SIZE_SMALL} -type palette "${base_file}_small.bmp"
done
