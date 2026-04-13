#!/bin/bash
set -e

# Support direct arguments like `docker run stego-benchmark review`
if [ "$1" = "simulate" ]; then
    echo ">>> Starting Geth Blockchain Simulation & Steganography Engine..."
    python3 scripts/simulation.py
elif [ "$1" = "review" ]; then
    echo ">>> Starting One-Click Reviewer Mode..."
    ./bin/advanced_main --option 7
elif [ "$1" != "" ]; then
    # pass custom command
    exec "$@"
else
    echo ">>> Starting Academic Benchmark..."
    # Pass all arguments to the C++ binary if no 'simulate' keyword
    if [ $# -eq 0 ]; then
        ./bin/advanced_main --option 6 --file data/log.txt --secret "AcademicBenchmarkData" --iterations 10 --outdir results
    else
        ./bin/advanced_main "$@"
    fi
fi
