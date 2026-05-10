#!/usr/bin/env bash
# SPARTAN-16 — one-shot build + run (for easy download-and-try).
# Usage: ./demo.sh [hello|fibonacci|timer]
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT"

which make >/dev/null 2>&1 || { echo "error: need 'make' in PATH"; exit 1; }
which g++ >/dev/null 2>&1 || { echo "error: need g++ (C++17). On macOS: xcode-select --install"; exit 1; }

make

case "${1:-hello}" in
  hello)     ./cpu exec programs/hello.asm ;;
  fibonacci) ./cpu exec programs/fibonacci.asm ;;
  timer)     ./cpu exec programs/timer.asm ;;
  *)         echo "usage: $0 [hello|fibonacci|timer]"; exit 1 ;;
esac
