# Use an official C++ runtime as a parent image
# Using a specific mirror to avoid timeout issues in some regions
FROM docker.m.daocloud.io/ubuntu:22.04

# Install build dependencies
RUN apt-get update && \
    apt-get install -y build-essential make && \
    rm -rf /var/lib/apt/lists/*

# Set the working directory in the container
WORKDIR /app

# Copy the current directory contents into the container at /app
COPY . /app

# Compile the C++ program
RUN make

# Ensure the results directory exists
RUN mkdir -p results

# The default command will run the benchmark with default arguments.
# Users can override these arguments by passing them to `docker run`.
ENTRYPOINT ["./bin/advanced_main"]
CMD ["--option", "6", "--file", "data/log.txt", "--secret", "AcademicBenchmarkData", "--iterations", "10", "--outdir", "results"]
