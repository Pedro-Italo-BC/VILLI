#!/bin/bash

# Paths

NAME="rukia"

INPUT="../assets/INPUT/${NAME}.jpeg"

OUTPUT="../assets/LABEL/${NAME}_label.png"

OUTPUT_KPV="../assets/SVG/${NAME}_k"

INPUT_L="../assets/LABEL/${NAME}_label_3.png"

INPUT_UPS="../assets/SVG/${NAME}_k.kpv"

OUTPUT_UPS="../assets/SVG/${NAME}_ups"

# Upscale factor

SCALE="0.1"

# SICLE CONFIGS

#EXTREME DETAIL CONFIG
# ../bin/RunSICLE \
#   --img "$INPUT" \
#   --out "$OUTPUT" \
#   --conn-opt fsum \
#   --crit-opt size \
#   --multiscale \
#   --alpha 0.4 \
#   --irreg 0.02 \
#   --adhr 30 \
#   --n0 20000 \
#   --nf 8000

# BALANCED DETAIL CONFIG
../bin/RunSICLE \
  --img "$INPUT" \
  --out "$OUTPUT" \
  --conn-opt fmax \
  --crit-opt minsc \
  --pen-opt none \
  --multiscale \
  --alpha 0.28 \
  --irreg 0.05 \
  --adhr 18 \
  --max-iters 12 \
  --n0 4000 \
  --nf 1200

# SMOOTH / ROUNDED SUPERPIXELS CONFIG
# ../bin/RunSICLE \
#   --img "$INPUT" \
#   --out "$OUTPUT" \
#   --conn-opt fsum \
#   --crit-opt spread \
#   --pen-opt none \
#   --multiscale \
#   --alpha 0.18 \
#   --irreg 0.09 \
#   --adhr 10 \
#   --max-iters 10 \
#   --n0 3500 \
#   --nf 1200

# VILLI

../bin/RunVILLI \
  --img "$INPUT" \
  --labels "$INPUT_L" \
  --out "$OUTPUT_KPV" \
  --keepScratch

# VILLI UPSCALE

../bin/RunVILLIUpscale \
  --kpv "$INPUT_UPS" \
  --upscale "$SCALE" \
  --out "$OUTPUT_UPS"


echo ""
echo "=========================================="
echo "Segmentation finished!"
echo "Label output : $OUTPUT"
echo "KPV output   : $OUTPUT_KPV"
echo "Upscale out  : $OUTPUT_UPS"
echo "=========================================="