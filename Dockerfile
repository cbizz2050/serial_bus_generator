# Production environment
FROM gcc:12 as builder

RUN apt-get update && apt-get install -y \
    cmake \
    build-essential \
    && rm -rf /var/lib/apt/lists/*



WORKDIR /build
COPY . .

# Build without tests for production
RUN cmake -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_TESTING=OFF && \
    cmake --build build --verbose

# Change the runtime base image to match the builder
FROM debian:bookworm-slim

# Add required runtime libraries
RUN apt-get update && apt-get install -y \
    libstdc++6 \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /build/build/src/serial_bus_generator /usr/local/bin/serial_bus_generator

ENV PROTOCOL=CANJ1939 \
    RATE=100

ENTRYPOINT ["/usr/local/bin/serial_bus_generator"]