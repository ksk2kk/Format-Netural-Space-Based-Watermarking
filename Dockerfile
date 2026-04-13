FROM ethereum/client-go:v1.13.15 AS geth-image
FROM ubuntu:22.04

# Avoid tzdata interactive prompt during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Copy geth from the official image
COPY --from=geth-image /usr/local/bin/geth /usr/local/bin/geth

# Install dependencies (compiler, tools, python)
RUN apt-get update && \
    apt-get install -y software-properties-common wget build-essential make python3 python3-pip dos2unix && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . /app

# Convert line endings just in case, to avoid Windows LF/CRLF issues inside the Linux container
RUN dos2unix /app/entrypoint.sh /app/scripts/simulation.py /app/build.sh /app/Makefile

# Compile the C++ project
RUN make clean && make

# Ensure results and data directories exist
RUN mkdir -p results data

# Make the entrypoint executable
RUN chmod +x /app/entrypoint.sh

# Default command will run the benchmark. Pass "simulate" to run the simulation instead.
ENTRYPOINT ["/app/entrypoint.sh"]
