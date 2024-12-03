#include <gtest/gtest.h>
#include "serial_bus_generator/protocols/arinc429/arinc429_message.hpp"
#include <vector>
#include <stdexcept>
#include <iostream>

using namespace serial_bus_generator;

class ARINC429MessageTest : public ::testing::Test {
protected:
    void SetUp() override {
        try {
            position_msg = std::make_unique<ARINC429Message>(
                ARINC429Label::LATITUDE,
                45.5f,
                ARINC429SSM::NORMAL_OPERATION
            );
        } catch (const std::exception& e) {
            std::cerr << "SetUp failed: " << e.what() << std::endl;
            throw;
        }
    }

    std::unique_ptr<ARINC429Message> position_msg;
};

TEST_F(ARINC429MessageTest, BasicProperties) {
    ASSERT_TRUE(position_msg != nullptr);
    EXPECT_EQ(position_msg->getType(), MessageType::ARINC429)
        << "Message type should be ARINC429";
    EXPECT_EQ(position_msg->getLabel(), ARINC429Label::LATITUDE)
        << "Label should be LATITUDE (310)";
    EXPECT_EQ(position_msg->getSSM(), ARINC429SSM::NORMAL_OPERATION)
        << "SSM should be NORMAL_OPERATION";
    EXPECT_TRUE(position_msg->isValid())
        << "Message should be valid";
}

TEST_F(ARINC429MessageTest, DataEncoding) {
    ASSERT_TRUE(position_msg != nullptr);
    float value = position_msg->getDecodedValue();
    // Increased delta to account for BNR encoding/decoding precision
    const float DELTA = 0.01f;  // Allow for 0.01 degree precision
    EXPECT_NEAR(value, 45.5f, DELTA)
        << "Decoded value should be within " << DELTA << " degrees of 45.5";
}

TEST_F(ARINC429MessageTest, Serialization) {
    ASSERT_TRUE(position_msg != nullptr);
    auto data = position_msg->serialize();
    EXPECT_EQ(data.size(), 4)
        << "ARINC429 word should be 32 bits (4 bytes)";
    
    // Check label (first 8 bits)
    EXPECT_EQ(data[0] & 0xFF, static_cast<uint8_t>(ARINC429Label::LATITUDE))
        << "First byte should contain the label";
    
    EXPECT_TRUE(position_msg->verifyParity())
        << "Parity check should pass";
}