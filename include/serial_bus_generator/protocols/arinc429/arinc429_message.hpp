#pragma once

#include "serial_bus_generator/messages/base_message.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace serial_bus_generator {

enum class ARINC429Label : uint16_t {  // Changed to uint16_t to accommodate larger values
    LATITUDE = 310,
    LONGITUDE = 311,
    ALTITUDE = 203,
    GROUND_SPEED = 312,
    TRACK_HEADING = 313,
    VERTICAL_SPEED = 324,
    EQUIPMENT_STATUS = 270,
    NAVIGATION_MODE = 272,
    GPS_SATELLITE_STATUS = 273,
    SYSTEM_CONFIG = 274
};

enum class ARINC429SSM : uint8_t {
    NORMAL_OPERATION = 0b00,
    NO_COMPUTED_DATA = 0b01,
    FUNCTIONAL_TEST = 0b10,
    FAILURE_WARNING = 0b11
};

/**
 * @brief ARINC429 message implementation
 */
class ARINC429Message : public BaseMessage {
public:
    ARINC429Message(ARINC429Label label, float value, ARINC429SSM ssm);
    
    bool isValid() const override;
    std::vector<uint8_t> serialize() const override;
    std::string toString() const override;
    
    ARINC429Label getLabel() const { return label_; }
    ARINC429SSM getSSM() const { return ssm_; }
    float getDecodedValue() const;
    bool verifyParity() const;

private:
    ARINC429Label label_;
    ARINC429SSM ssm_;
    uint32_t raw_data_;  // Full 32-bit word
    
    void encodeValue(float value);
    uint8_t calculateParity() const;
    bool isValidLabel(ARINC429Label label) const;
};

} // namespace serial_bus_generator