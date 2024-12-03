#include <gtest/gtest.h>
#include "serial_bus_generator/protocols/canj1939/canj1939_message.hpp"
#include <vector>
#include <stdexcept>
#include <iostream>

using namespace serial_bus_generator;

class CANJ1939MessageTest : public ::testing::Test {
protected:
    void SetUp() override {
        try {
            engine_msg = std::make_unique<CANJ1939Message>(
                CANJ1939PGN::ENGINE_SPEED,
                1500.0f,  // 1500 RPM
                CANJ1939Priority::PRIORITY_3
            );
        } catch (const std::exception& e) {
            std::cerr << "SetUp failed: " << e.what() << std::endl;
            throw;
        }
    }

    std::unique_ptr<CANJ1939Message> engine_msg;
};

TEST_F(CANJ1939MessageTest, BasicProperties) {
    ASSERT_TRUE(engine_msg != nullptr);
    EXPECT_EQ(engine_msg->getType(), MessageType::CANJ1939)
        << "Message type should be CANJ1939";
    EXPECT_EQ(engine_msg->getPGN(), CANJ1939PGN::ENGINE_SPEED)
        << "PGN should be ENGINE_SPEED (61444)";
    EXPECT_EQ(engine_msg->getPriority(), CANJ1939Priority::PRIORITY_3)
        << "Priority should be 3";
    EXPECT_TRUE(engine_msg->isValid())
        << "Message should be valid";
}

TEST_F(CANJ1939MessageTest, DataEncoding) {
    ASSERT_TRUE(engine_msg != nullptr);
    float value = engine_msg->getDecodedValue();
    const float DELTA = 0.1f;  // Allow for 0.1 RPM precision
    EXPECT_NEAR(value, 1500.0f, DELTA)
        << "Decoded value should be within " << DELTA << " RPM of 1500";
}

TEST_F(CANJ1939MessageTest, Serialization) {
    ASSERT_TRUE(engine_msg != nullptr);
    auto data = engine_msg->serialize();
    EXPECT_EQ(data.size(), 8)  // CAN frame is 8 bytes
        << "J1939 message should be 8 bytes";
    
    // Check PGN (bytes 1-3)
    uint32_t pgn = (static_cast<uint32_t>(data[2]) << 16) |
                   (static_cast<uint32_t>(data[1]) << 8) |
                    static_cast<uint32_t>(data[0]);
    EXPECT_EQ(pgn, static_cast<uint32_t>(CANJ1939PGN::ENGINE_SPEED))
        << "First three bytes should contain the PGN";
}