#include "serial_bus_generator/core/data_generator.hpp"
#include "serial_bus_generator/protocols/arinc429/arinc429_generator.hpp"
#include "serial_bus_generator/protocols/canj1939/canj1939_generator.hpp"
#include <memory>
#include <iostream>
#include <cstring>

void print_usage() {
    std::cout << "Usage: serial_bus_generator --protocol <ARINC429|CANJ1939> --rate <Hz>\n";
}

int main(int argc, char* argv[]) {
    std::string protocol = "ARINC429";  // Default
    uint32_t rate = 100;  // Default 100Hz

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--protocol") == 0 && i + 1 < argc) {
            protocol = argv[++i];
            std::cout << "Protocol: " << protocol << "\n";
        } else if (strcmp(argv[i], "--rate") == 0 && i + 1 < argc) {
            rate = std::stoul(argv[++i]);
            std::cout << "Rate: " << rate << "\n";
        } else if (strcmp(argv[i], "--help") == 0) {
            print_usage();
            return 0;
        }
    }

    std::unique_ptr<serial_bus_generator::IGenerator> generator;

    try {
        if (protocol == "ARINC429") {
            generator = std::make_unique<serial_bus_generator::ARINC429Generator>();
        } else if (protocol == "CANJ1939") {
            generator = std::make_unique<serial_bus_generator::CANJ1939Generator>();
        } else {
            std::cerr << "Invalid protocol: " << protocol << "\n";
            print_usage();
            return 1;
        }

        generator->setRate(rate);
        generator->start();

        // Run until interrupted
        std::cout << "Generator running (Ctrl+C to stop)...\n";
        std::cout << "Generating " << protocol << " messages at " << rate << " Hz\n\n";
        
        while (generator->getState() == serial_bus_generator::GeneratorState::RUNNING) {
            auto message = generator->getLastMessage();
            if (!message.empty()) {
                std::cout << "[" << protocol << "] " << message << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Poll every 100ms
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}