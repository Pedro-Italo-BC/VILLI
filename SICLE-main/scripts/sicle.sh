#!/bin/bash

# Paths

NAME="Skebob!"

INPUT="../assets/INPUT/${NAME}.jpeg"

OUTPUT="../assets/LABEL/${NAME}_label.png"

OUTPUT_KPV="../assets/SVG/${NAME}_k"

INPUT_L="../assets/LABEL/${NAME}_label.png"

INPUT_UPS="../assets/SVG/${NAME}_k.kpv"

OUTPUT_UPS="../assets/SVG/${NAME}_ups"

# Upscale factor

SCALE="0.6"

# SICLE CONFIGS

#EXTREME DETAIL CONFIG


../bin/RunSICLE \
--img "$INPUT" \
  --out "$OUTPUT" \
  --conn-opt fsum \
  --n0 8000 \
  --nf 2000 --irreg 0.5
  # --crit-opt spread \
  # --multiscale \
  # --adhr 30 \

# BALANCED
# ../bin/RunSICLE \
#   --img "$INPUT" \
#   --out "$OUTPUT" \
#   --conn-opt fmax \
#   --crit-opt minsc \
#   --pen-opt none \
#   --multiscale \
#   --alpha 0.28 \
#   --irreg 0.05 \
#   --adhr 18 \
#   --max-iters 12 \
#   --n0 4000 \
#   --nf 1200

# SMOOTH
# ../bin/RunSICLE \
#   --img "$INPUT" \
#   --out "$OUTPUT" \
#   --conn-opt fmax \
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


../bin/RunOvlayBorders \
  --img "$INPUT" \
  --labels "$INPUT_L" \
  --out "../assets/SVG/Border.png" 

../bin/RunVILLI \
  --img "$INPUT" \
  --labels "$INPUT_L" \
  --out "$OUTPUT_KPV" \
  --keepScratch

# VILLI UPSCALE

# ../bin/RunVILLIUpscale \
#   --kpv "$INPUT_UPS" \
#   --upscale "$SCALE" \
#   --out "$OUTPUT_UPS"


echo ""
echo "=========================================="
echo "Segmentation finished!"
echo "Label output : $OUTPUT"
echo "KPV output   : $OUTPUT_KPV"
echo "Upscale out  : $OUTPUT_UPS"
echo "=========================================="