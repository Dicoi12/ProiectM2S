CONFIG_FILE="config.ini"
REPORT_PREFIX="output"
BENCHMARKS_DIR="./m2s-bench-parsec-3.0"
BENCHMARKS=(
  "blackscholes"
  "bodytrack"
  "canneal"
  "facesim"
  "ferret"
  "fluidanimate"
  "streamcluster"
  "swaptions"
  "vips"
)
for BENCHMARK in "${BENCHMARKS[@]}"; do
  REPORT_FILE="${REPORT_PREFIX}_${BENCHMARK}.txt"
  m2s --x86-sim detailed --x86-config "$CONFIG_FILE" --x86-report "output/$REPORT_FILE" "$BENCHMARKS_DIR/$BENCHMARK/$BENCHMARK" 4
done
