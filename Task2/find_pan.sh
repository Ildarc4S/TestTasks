#!/bin/sh

ShowHelp() {
  echo "Usage: $0 [FILE]"
  echo "   or: $0 --help"
  echo ""
  echo "Extracts unique bank card numbers from a text file."
  echo "Card numbers must be in the format: PAN: <13-19 digits>"
  echo ""
  echo "Arguments:"
  echo "  FILE    text file to search (default: trans.txt)"
  echo "  --help  show this help message"
  echo ""
  echo "Examples:"
  echo "  $0                    # uses trans.txt"
  echo "  $0 transactions.log   # uses specified file"
  echo "  $0 --help             # show this help"
}

if [ "$1" = "--help" ] || [ "$1" = "-h" ]; then
  ShowHelp
  exit 0
fi

filename="${1:-example_files/trans.txt}"

if [ ! -f "$filename" ]; then
  echo "Error: File $filename not found!"
  exit 1
fi

grep -oP "PAN: \K[0-9]{13,19}" "$filename" | sort -u
