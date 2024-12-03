#!/bin/bash
# scripts/run_tests_in_docker.sh

set -e  # Exit on any error

# Build the test image
echo "Building test image..."
docker build -t serial-bus-generator-test -f Dockerfile.test .

# Run the tests
echo "Running tests..."
docker run --rm serial-bus-generator-test

# If you want to run a specific test, uncomment and modify the following line:
# docker run --rm serial-bus-generator-test ./tests/message_interface_test

# Check exit status
if [ $? -eq 0 ]; then
    echo "All tests passed!"
else
    echo "Tests failed!"
    exit 1
fi