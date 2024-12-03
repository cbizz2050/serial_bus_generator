#pragma once

#include "serial_bus_generator/interfaces/message_interface.hpp"
#include <vector>
#include <memory>
#include <chrono>

namespace serial_bus_generator {

enum class GeneratorState {
    STOPPED,    // Generator is not running
    RUNNING,    // Generator is actively producing messages
    ERROR,      // Generator encountered an error
    PAUSED      // Generator is temporarily suspended
};

class IGenerator {
public:
    virtual ~IGenerator() = default;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void setRate(uint32_t rate) = 0;
    virtual GeneratorState getState() const = 0;
    virtual std::vector<std::unique_ptr<IMessage>> generateMessages(
        std::chrono::milliseconds duration) = 0;
    virtual std::string getLastMessage() = 0;
};

} // namespace serial_bus_generator