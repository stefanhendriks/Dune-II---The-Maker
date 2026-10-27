#!/bin/sh
# Golden master test: seed2map must produce the same maps as the files in expected/.
# Seeds: 0 1 42 4242 65535 plus all campaign seeds ([MAP] Seed in resources/bin/campaign/maps/scen*.ini).
# Line endings (CR) are ignored: Windows writes CRLF, and git may convert the expected files.
# Usage: sh test.sh          compare against expected/
#        sh test.sh update   regenerate expected/ (only when the algorithm is changed on purpose)
cd "$(dirname "$0")" || exit 1
SEEDS="0 1 42 4242 65535 5 12 16 24 27 33 53 57 59 62 89 103 106 113 117 122 123 157 173 182 203 207 223 233 268 269 298 305 332 353 355 367 392 500 501 504 552 563 582 604 717 723 1498 2106 2437 2620 2674 3161"
BIN=./seed2map
out=$(mktemp)
fail=0
for s in $SEEDS; do
    : > "$out"
    $BIN "$s" "$out" > /dev/null || { echo "FAIL: seed $s: seed2map failed"; fail=1; continue; }
    [ -s "$out" ] || { echo "FAIL: seed $s: seed2map wrote no map"; fail=1; continue; }
    if [ "$1" = "update" ]; then
        tr -d '\r' < "$out" > "expected/seed_$s.txt"
    else
        # plain string comparison: cmp/diff are not in every MSYS2 shell
        if [ "$(tr -d '\r' < "$out")" != "$(tr -d '\r' < "expected/seed_$s.txt")" ]; then
            echo "FAIL: seed $s differs from expected/seed_$s.txt"
            fail=1
        fi
    fi
done
rm -f "$out"
[ "$fail" = 0 ] && echo "seedgen golden master: OK"
exit $fail
