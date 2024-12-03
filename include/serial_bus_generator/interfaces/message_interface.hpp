#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <stdexcept>  // Added for std::runtime_error

namespace serial_bus_generator {

/**
 * @brief Enumeration of supported message types
 */
enum class MessageType {
    ARINC429,
    CANJ1939
};

/**
 * @brief Interface for all message types in the system
 */
class IMessage {
public:
    virtual ~IMessage() = default;
    virtual bool isValid() const = 0;
    virtual std::vector<uint8_t> serialize() const = 0;
    virtual std::string toString() const = 0;
    virtual MessageType getType() const = 0;
    virtual uint32_t getTimestamp() const = 0;
};

/**
 * @brief Exception thrown when message validation fails
 */
class MessageValidationError : public std::runtime_error {
public:
    explicit MessageValidationError(const std::string& what_arg)
        : std::runtime_error(what_arg) {}
};

} // namespace serial_bus_generator