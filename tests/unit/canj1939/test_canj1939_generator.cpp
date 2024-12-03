#include <gtest/gtest.h>
#include "serial_bus_generator/protocols/canj1939/canj1939_generator.hpp"
#include <thread>
#include <chrono>

using namespace serial_bus_generator;
using namespace std::chrono_literals;

class CANJ1939GeneratorTest : public ::testing::Test {
protected:
    void SetUp() override {
        generator = std::make_unique<CANJ1939Generator>();
    }

    std::unique_ptr<CANJ1939Generator> generator;
};

TEST_F(CANJ1939GeneratorTest, InitialState) {
    EXPECT_EQ(generator->getState(), GeneratorState::STOPPED);
}

TEST_F(CANJ1939GeneratorTest, GeneratesMessages) {
    auto messages = generator->generateMessages(1000ms);
    EXPECT_FALSE(messages.empty());
    
    for (const auto& msg : messages) {
        EXPECT_EQ(msg->getType(), MessageType::CANJ1939);
        auto j1939_msg = dynamic_cast<const CANJ1939Message*>(msg.get());
        EXPECT_NE(j1939_msg, nullptr);
    }
}

TEST_F(CANJ1939GeneratorTest, RateControl) {
    generator->setRate(100);  // 100Hz
    auto start = std::chrono::steady_clock::now();
    
    generator->start();
    std::this_thread::sleep_for(100ms);
    generator->stop();
    
    auto duration = std::chrono::steady_clock::now() - start;
    EXPECT_GE(duration, 100ms);
}

TEST_F(CANJ1939GeneratorTest, EngineParameters) {
    auto messages = generator->generateMessages(1000ms);
    bool found_speed = false;
    bool found_temp = false;
    
    for (const auto& msg : messages) {
        auto j1939_msg = dynamic_cast<const CANJ1939Message*>(msg.get());
        switch (j1939_msg->getPGN()) {
            case CANJ1939PGN::ENGINE_SPEED: {
                float rpm = j1939_msg->getDecodedValue();
                EXPECT_GE(rpm, 0.0f);
                EXPECT_LE(rpm, 8000.0f);
                found_speed = true;
                break;
            }
            case CANJ1939PGN::ENGINE_TEMPERATURE: {
                float temp = j1939_msg->getDecodedValue();
                EXPECT_GE(temp, -40.0f);
                EXPECT_LE(temp, 150.0f);
                found_temp = true;
                break;
            }
        }
    }
    
    EXPECT_TRUE(found_speed) << "No engine speed message found";
    EXPECT_TRUE(found_temp) << "No engine temperature message found";
}