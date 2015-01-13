#!/bin/bash
if [ -z "$1" ]; then
	echo "please specify output file"
	exit 0
fi
printf "writing to %s...\n" $1
runAndPrint() {
	echo "cgol.mrbl"
	>&2 echo "cgol.mrbl"
	>&2 perf stat ./marblerun test/cgol.mrbl
	echo "bouncingColors.mrbl"
	>&2 echo "\n\n\n\nbouncingColors.mrbl"
	>&2 perf stat ./marblerun test/bouncingColors.mrbl
	echo "fibonacciX16.mrbl"
	>&2 echo "\n\n\n\nfibonacciX16.mrbl"
	>&2 perf stat ./marblerun test/fibonacciX16.mrbl
}
runAndPrint 2> $1
exit 0
