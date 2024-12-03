#pragma once

#include "serial_bus_generator/interfaces/message_interface.hpp"
#include <chrono>

namespace serial_bus_generator {

/**
 * @brief Base implementation of IMessage interface
 * 
 * Provides common functionality for all message types
 */
class BaseMessage : public IMessage {
public:
    explicit BaseMessage(MessageType type)
        : type_(type),
          timestamp_(std::chrono::duration_cast<std::chrono::milliseconds>(
              std::chrono::system_clock::now().time_since_epoch()).count())
    {}

    virtual ~BaseMessage() = default;

    // Implementation of common methods from IMessage
    MessageType getType() const override { return type_; }
    uint32_t getTimestamp() const override { return timestamp_; }

    // These still need to be implemented by derived classes
    bool isValid() const override = 0;
    std::vector<uint8_t> serialize() const override = 0;
    std::string toString() const override = 0;

protected:
    MessageType type_;
    uint32_t timestamp_;
};

} // namespace serial_bus_generator