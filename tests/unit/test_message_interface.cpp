#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "serial_bus_generator/interfaces/message_interface.hpp"
#include <vector>
#include <stdexcept>

using namespace serial_bus_generator;

// Mock implementation for testing
class MockMessage : public IMessage {
public:
    MOCK_METHOD(bool, isValid, (), (const, override));
    MOCK_METHOD(std::vector<uint8_t>, serialize, (), (const, override));
    MOCK_METHOD(std::string, toString, (), (const, override));
    MOCK_METHOD(MessageType, getType, (), (const, override));
    MOCK_METHOD(uint32_t, getTimestamp, (), (const, override));
};

class MessageInterfaceTest : public ::testing::Test {
protected:
    void SetUp() override {
        message = std::make_unique<MockMessage>();
    }

    std::unique_ptr<MockMessage> message;
};

TEST_F(MessageInterfaceTest, BasicMessageProperties) {
    // Setup basic expectations
    EXPECT_CALL(*message, isValid())
        .WillOnce(testing::Return(true));
    
    EXPECT_CALL(*message, getType())
        .WillOnce(testing::Return(MessageType::ARINC429));
    
    EXPECT_CALL(*message, getTimestamp())
        .WillOnce(testing::Return(1234567890));

    // Test the expectations
    EXPECT_TRUE(message->isValid());
    EXPECT_EQ(message->getType(), MessageType::ARINC429);
    EXPECT_EQ(message->getTimestamp(), 1234567890);
}

TEST_F(MessageInterfaceTest, SerializationBehavior) {
    std::vector<uint8_t> testData = {0x01, 0x02, 0x03, 0x04};
    
    EXPECT_CALL(*message, serialize())
        .WillOnce(testing::Return(testData));

    auto serialized = message->serialize();
    EXPECT_EQ(serialized, testData);
}

TEST_F(MessageInterfaceTest, StringRepresentation) {
    EXPECT_CALL(*message, toString())
        .WillOnce(testing::Return("ARINC429 Message: Label=310"));

    EXPECT_EQ(message->toString(), "ARINC429 Message: Label=310");
}

TEST_F(MessageInterfaceTest, InvalidMessageHandling) {
    EXPECT_CALL(*message, isValid())
        .WillOnce(testing::Return(false));

    EXPECT_FALSE(message->isValid());
}