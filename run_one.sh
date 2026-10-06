TEST_DIR="$1"
GEN="$(realpath "$2")"
N="$3"

OUT_DIR="$(dirname "$(realpath "$0")")/${TEST_DIR}/N${N}"
mkdir -p "$OUT_DIR"
cd "${OUT_DIR}"

"$GEN" "$N"
iverilog -g2012 -o simv "karatsuba_${N}.sv" "tb_karatsuba_${N}.sv"
echo "RUNNING TEST FOR N=${N}"
OUT="$(vvp simv)"

if echo "$OUT" | grep -q '^PASS'; then
    echo "PASSED: N=${N}"
else
    echo "$OUT"
    echo "FAIL: N=${N}"
    exit 1
fi