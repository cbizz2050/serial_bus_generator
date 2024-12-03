#pragma once

#include "serial_bus_generator/messages/base_message.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace serial_bus_generator {

enum class CANJ1939PGN : uint32_t {
    ENGINE_SPEED = 61444,      // 0xF004
    ENGINE_TEMPERATURE = 65262, // 0xFEEE
    ENGINE_HOURS = 65253,      // 0xFEE5
    ENGINE_FLUID_LEVEL = 65263, // 0xFEEF
    ENGINE_CONFIG = 65251       // 0xFEE3
};

enum class CANJ1939Priority : uint8_t {
    PRIORITY_0 = 0, // Highest
    PRIORITY_1 = 1,
    PRIORITY_2 = 2,
    PRIORITY_3 = 3,
    PRIORITY_4 = 4,
    PRIORITY_5 = 5,
    PRIORITY_6 = 6,
    PRIORITY_7 = 7  // Lowest
};

/**
 * @brief CAN J1939 message implementation
 * 
 * Implements the J1939 message format:
 * - 29-bit identifier
 *   - Priority (3 bits)
 *   - PGN (18 bits)
 *   - Source Address (8 bits)
 * - Up to 8 bytes of data
 */
class CANJ1939Message : public BaseMessage {
public:
    CANJ1939Message(CANJ1939PGN pgn, float value, CANJ1939Priority priority);
    
    bool isValid() const override;
    std::vector<uint8_t> serialize() const override;
    std::string toString() const override;
    
    // J1939 specific methods
    CANJ1939PGN getPGN() const { return pgn_; }
    CANJ1939Priority getPriority() const { return priority_; }
    float getDecodedValue() const;

private:
    CANJ1939PGN pgn_;
    CANJ1939Priority priority_;
    std::vector<uint8_t> data_;  // Raw data bytes
    
    void encodeValue(float value);
    bool isValidPGN(CANJ1939PGN pgn) const;
    uint32_t calculateIdentifier() const;
};

} // namespace serial_bus_generator