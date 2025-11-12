#!/bin/bash
set -euo pipefail
threshold=$(date -v-90d +%s)

find nightly -type f -regex '.*/D2TM-[0-9]\{4\}-[0-9]\{2\}-[0-9]\{2\}T[0-9]\{2\}-[0-9]\{2\}\..*\.zip$' \
  -print | while read -r f; do
    name=$(basename "$f")
    datepart=$(echo "$name" | grep -oE '[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}' || true)
    if [ -z "$datepart" ]; then
      echo "Skipping $name (no date found)"
      continue
    fi
    iso=$(echo "$datepart" | sed 's/T\([0-9][0-9]\)-\([0-9][0-9]\)/T\1:\2/')

    # macOS date conversion
    file_epoch=$(date -j -u -f "%Y-%m-%dT%H:%M" "$iso" +%s 2>/dev/null || echo 0)

    if [ "$file_epoch" -lt "$threshold" ]; then
      echo "Deleting $f (older than 90 days)"
      rm -f "$f"
    fi
  done
