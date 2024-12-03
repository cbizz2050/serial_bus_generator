#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "serial_bus_generator/interfaces/generator_interface.hpp"
#include <thread>
#include <chrono>

using namespace serial_bus_generator;
using namespace std::chrono_literals;

// Mock implementation for testing
class MockGenerator : public IGenerator {
public:
    MOCK_METHOD(void, start, (), (override));
    MOCK_METHOD(void, stop, (), (override));
    MOCK_METHOD(void, setRate, (uint32_t rate), (override));
    MOCK_METHOD(GeneratorState, getState, (), (const, override));
    MOCK_METHOD(std::vector<std::unique_ptr<IMessage>>, generateMessages, (std::chrono::milliseconds duration), (override));
};

class GeneratorInterfaceTest : public ::testing::Test {
protected:
    void SetUp() override {
        generator = std::make_unique<MockGenerator>();
    }

    std::unique_ptr<MockGenerator> generator;
};

TEST_F(GeneratorInterfaceTest, BasicOperations) {
    // Test start/stop operations
    EXPECT_CALL(*generator, start())
        .Times(1);
    EXPECT_CALL(*generator, stop())
        .Times(1);
    
    generator->start();
    generator->stop();
}

TEST_F(GeneratorInterfaceTest, RateConfiguration) {
    EXPECT_CALL(*generator, setRate(100))
        .Times(1);
    
    generator->setRate(100);  // 100 Hz
}

TEST_F(GeneratorInterfaceTest, StateManagement) {
    EXPECT_CALL(*generator, getState())
        .WillOnce(testing::Return(GeneratorState::RUNNING));
    
    EXPECT_EQ(generator->getState(), GeneratorState::RUNNING);
}

TEST_F(GeneratorInterfaceTest, MessageGeneration) {
    auto duration = 100ms;
    
    EXPECT_CALL(*generator, generateMessages(duration))
        .Times(1)
        .WillOnce(testing::Return(std::vector<std::unique_ptr<IMessage>>{}));
    
    auto messages = generator->generateMessages(duration);
    EXPECT_TRUE(messages.empty());  // For this test, we return empty vector
}