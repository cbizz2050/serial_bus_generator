#include "serial_bus_generator/protocols/arinc429/arinc429_message.hpp"
#include <sstream>
#include <iomanip>
#include <cmath>
#include <bitset>

namespace serial_bus_generator {

ARINC429Message::ARINC429Message(ARINC429Label label, float value, ARINC429SSM ssm)
    : BaseMessage(MessageType::ARINC429),
      label_(label),
      ssm_(ssm),
      raw_data_(0)
{
    if (!isValidLabel(label)) {
        throw MessageValidationError("Invalid ARINC429 label");
    }
    
    // Encode the full 32-bit word
    // Label (8 bits)
    raw_data_ = static_cast<uint32_t>(label) & 0xFF;
    
    // Encode the value (19 bits)
    encodeValue(value);
    
    // SSM (2 bits)
    raw_data_ |= (static_cast<uint32_t>(ssm) & 0x03) << 29;
    
    // Calculate and set parity bit
    raw_data_ |= (calculateParity() & 0x01) << 31;
}

bool ARINC429Message::isValid() const {
    return isValidLabel(label_) && verifyParity();
}

std::vector<uint8_t> ARINC429Message::serialize() const {
    std::vector<uint8_t> data(4);
    data[0] = (raw_data_ >> 0) & 0xFF;  // Label
    data[1] = (raw_data_ >> 8) & 0xFF;  // Data LSB
    data[2] = (raw_data_ >> 16) & 0xFF; // Data MSB
    data[3] = (raw_data_ >> 24) & 0xFF; // SSM and Parity
    return data;
}

std::string ARINC429Message::toString() const {
    std::stringstream ss;
    ss << "ARINC429 Message: Label=" << static_cast<int>(static_cast<uint8_t>(label_))
       << " Value=" << std::fixed << std::setprecision(3) << getDecodedValue()
       << " SSM=";
    
    switch (ssm_) {
        case ARINC429SSM::NORMAL_OPERATION:
            ss << "Normal";
            break;
        case ARINC429SSM::NO_COMPUTED_DATA:
            ss << "No Data";
            break;
        case ARINC429SSM::FUNCTIONAL_TEST:
            ss << "Test";
            break;
        case ARINC429SSM::FAILURE_WARNING:
            ss << "Failure";
            break;
    }
    
    return ss.str();
}

float ARINC429Message::getDecodedValue() const {
    // Extract the 19-bit data field
    uint32_t data_bits = (raw_data_ >> 8) & 0x7FFFF;
    
    // Handle different label formats
    switch (label_) {
        case ARINC429Label::LATITUDE:
        case ARINC429Label::LONGITUDE: {
            // BNR format with resolution of 180°/(2^18)
            float value = static_cast<float>(data_bits) * (180.0f / 262144.0f);
            // Apply sign bit
            if (data_bits & 0x40000) value = -value;
            return value;
        }
        
        case ARINC429Label::ALTITUDE: {
            // BNR format, resolution of 0.125 feet
            return static_cast<float>(data_bits) * 0.125f;
        }
        
        case ARINC429Label::GROUND_SPEED: {
            // BNR format, resolution of 1 knot
            return static_cast<float>(data_bits);
        }
        
        default:
            // Default BNR format
            return static_cast<float>(data_bits);
    }
}

void ARINC429Message::encodeValue(float value) {
    uint32_t encoded_value = 0;
    
    // Encode based on label type
    switch (label_) {
        case ARINC429Label::LATITUDE:
        case ARINC429Label::LONGITUDE: {
            // Convert to BNR format
            float abs_value = std::abs(value);
            encoded_value = static_cast<uint32_t>(abs_value * (262144.0f / 180.0f));
            if (value < 0) encoded_value |= 0x40000; // Set sign bit
            break;
        }
        
        case ARINC429Label::ALTITUDE: {
            // Convert to BNR format (0.125 feet resolution)
            encoded_value = static_cast<uint32_t>(value / 0.125f);
            break;
        }
        
        case ARINC429Label::GROUND_SPEED: {
            // Direct BNR format
            encoded_value = static_cast<uint32_t>(value);
            break;
        }
        
        default:
            encoded_value = static_cast<uint32_t>(value);
    }
    
    // Place the encoded value in the raw data word (19 bits)
    raw_data_ |= (encoded_value & 0x7FFFF) << 8;
}

uint8_t ARINC429Message::calculateParity() const {
    // Calculate odd parity over all bits except the parity bit
    std::bitset<32> bits(raw_data_ & 0x7FFFFFFF);
    return bits.count() % 2 == 0 ? 1 : 0; // Ensure odd parity
}

bool ARINC429Message::verifyParity() const {
    // Extract current parity bit
    uint8_t stored_parity = (raw_data_ >> 31) & 0x01;
    // Compare with calculated parity
    return stored_parity == calculateParity();
}

bool ARINC429Message::isValidLabel(ARINC429Label label) const {
    // Check if the label is one of our defined values
    switch (label) {
        case ARINC429Label::LATITUDE:
        case ARINC429Label::LONGITUDE:
        case ARINC429Label::ALTITUDE:
        case ARINC429Label::GROUND_SPEED:
        case ARINC429Label::TRACK_HEADING:
        case ARINC429Label::VERTICAL_SPEED:
        case ARINC429Label::EQUIPMENT_STATUS:
        case ARINC429Label::NAVIGATION_MODE:
        case ARINC429Label::GPS_SATELLITE_STATUS:
        case ARINC429Label::SYSTEM_CONFIG:
            return true;
        default:
            return false;
    }
}

} // namespace serial_bus_generator