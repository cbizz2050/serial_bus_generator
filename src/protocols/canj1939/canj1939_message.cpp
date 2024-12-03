#include "serial_bus_generator/protocols/canj1939/canj1939_message.hpp"
#include <sstream>
#include <iomanip>
#include <cmath>
#include <bitset>

namespace serial_bus_generator {

CANJ1939Message::CANJ1939Message(CANJ1939PGN pgn, float value, CANJ1939Priority priority)
    : BaseMessage(MessageType::CANJ1939),
      pgn_(pgn),
      priority_(priority),
      data_(8, 0)  // Initialize 8 bytes of data to 0
{
    if (!isValidPGN(pgn)) {
        throw MessageValidationError("Invalid J1939 PGN");
    }
    
    encodeValue(value);
}

bool CANJ1939Message::isValid() const {
    return isValidPGN(pgn_) && data_.size() == 8;
}

std::vector<uint8_t> CANJ1939Message::serialize() const {
    std::vector<uint8_t> frame(8, 0);  // Initialize all bytes to 0
    
    // Store PGN in the first 3 bytes (little-endian)
    uint32_t pgn_value = static_cast<uint32_t>(pgn_);
    frame[2] = (pgn_value >> 16) & 0xFF;  // MSB
    frame[1] = (pgn_value >> 8) & 0xFF;   // Middle byte
    frame[0] = pgn_value & 0xFF;          // LSB
    
    // Copy remaining data bytes (starting at byte 3)
    for (size_t i = 0; i < data_.size() && i < 5; ++i) {
        frame[i + 3] = data_[i];
    }
    
    return frame;
}

std::string CANJ1939Message::toString() const {
    std::stringstream ss;
    ss << "J1939 Message: PGN=0x" << std::hex << std::uppercase 
       << static_cast<uint32_t>(pgn_)
       << " Priority=" << static_cast<int>(priority_)
       << " Value=" << std::fixed << std::setprecision(2) << getDecodedValue();
    
    return ss.str();
}

float CANJ1939Message::getDecodedValue() const {
    // Decode based on PGN type
    switch (pgn_) {
        case CANJ1939PGN::ENGINE_SPEED: {
            // Engine speed: 0.125 RPM/bit, 0 offset
            uint16_t raw_value = (static_cast<uint16_t>(data_[1]) << 8) | data_[0];
            return raw_value * 0.125f;
        }
        
        case CANJ1939PGN::ENGINE_TEMPERATURE: {
            // Temperature: 1°C/bit, -40°C offset
            return static_cast<float>(data_[0]) - 40.0f;
        }
        
        case CANJ1939PGN::ENGINE_HOURS: {
            // Engine hours: 0.05 hour/bit
            uint32_t raw_value = (static_cast<uint32_t>(data_[3]) << 24) |
                                (static_cast<uint32_t>(data_[2]) << 16) |
                                (static_cast<uint32_t>(data_[1]) << 8) |
                                data_[0];
            return raw_value * 0.05f;
        }
        
        case CANJ1939PGN::ENGINE_FLUID_LEVEL: {
            // Fluid level: 0.4%/bit, 0 offset
            return data_[0] * 0.4f;
        }
        
        default:
            return static_cast<float>(data_[0]);
    }
}

void CANJ1939Message::encodeValue(float value) {
    // Clear existing data
    std::fill(data_.begin(), data_.end(), 0);
    
    // Encode based on PGN type
    switch (pgn_) {
        case CANJ1939PGN::ENGINE_SPEED: {
            // Convert RPM to raw value (0.125 RPM/bit)
            uint16_t raw_value = static_cast<uint16_t>(std::round(value / 0.125f));
            data_[0] = raw_value & 0xFF;
            data_[1] = (raw_value >> 8) & 0xFF;
            break;
        }
        
        case CANJ1939PGN::ENGINE_TEMPERATURE: {
            // Convert temperature (+40°C offset)
            data_[0] = static_cast<uint8_t>(std::round(value + 40.0f));
            break;
        }
        
        case CANJ1939PGN::ENGINE_HOURS: {
            // Convert hours (0.05 hour/bit)
            uint32_t raw_value = static_cast<uint32_t>(std::round(value / 0.05f));
            data_[0] = raw_value & 0xFF;
            data_[1] = (raw_value >> 8) & 0xFF;
            data_[2] = (raw_value >> 16) & 0xFF;
            data_[3] = (raw_value >> 24) & 0xFF;
            break;
        }
        
        case CANJ1939PGN::ENGINE_FLUID_LEVEL: {
            // Convert percentage (0.4%/bit)
            data_[0] = static_cast<uint8_t>(std::round(value / 0.4f));
            break;
        }
        
        default:
            data_[0] = static_cast<uint8_t>(std::round(value));
            break;
    }
}

bool CANJ1939Message::isValidPGN(CANJ1939PGN pgn) const {
    switch (pgn) {
        case CANJ1939PGN::ENGINE_SPEED:
        case CANJ1939PGN::ENGINE_TEMPERATURE:
        case CANJ1939PGN::ENGINE_HOURS:
        case CANJ1939PGN::ENGINE_FLUID_LEVEL:
        case CANJ1939PGN::ENGINE_CONFIG:
            return true;
        default:
            return false;
    }
}

uint32_t CANJ1939Message::calculateIdentifier() const {
    // Build 29-bit CAN identifier
    uint32_t identifier = 0;
    
    // Priority (bits 26-28)
    identifier |= (static_cast<uint32_t>(priority_) & 0x07) << 26;
    
    // PGN (bits 8-25)
    identifier |= (static_cast<uint32_t>(pgn_) & 0x3FFFF) << 8;
    
    // Source Address (bits 0-7) - Using 0xFE as default source
    identifier |= 0xFE;
    
    return identifier;
}

} // namespace serial_bus_generator