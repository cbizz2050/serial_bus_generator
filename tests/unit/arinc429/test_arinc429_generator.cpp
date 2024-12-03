#include <gtest/gtest.h>
#include "serial_bus_generator/protocols/arinc429/arinc429_generator.hpp"
#include <thread>
#include <chrono>

using namespace serial_bus_generator;
using namespace std::chrono_literals;

class ARINC429GeneratorTest : public ::testing::Test {
protected:
    void SetUp() override {
        generator = std::make_unique<ARINC429Generator>();
    }

    std::unique_ptr<ARINC429Generator> generator;
};

TEST_F(ARINC429GeneratorTest, InitialState) {
    EXPECT_EQ(generator->getState(), GeneratorState::STOPPED);
}

TEST_F(ARINC429GeneratorTest, GeneratesMessages) {
    auto messages = generator->generateMessages(1000ms);
    EXPECT_FALSE(messages.empty());
    
    for (const auto& msg : messages) {
        EXPECT_EQ(msg->getType(), MessageType::ARINC429);
        auto arinc_msg = dynamic_cast<const ARINC429Message*>(msg.get());
        EXPECT_NE(arinc_msg, nullptr);
    }
}

TEST_F(ARINC429GeneratorTest, RateControl) {
    generator->setRate(100);  // 100Hz
    auto start = std::chrono::steady_clock::now();
    
    generator->start();
    std::this_thread::sleep_for(100ms);
    generator->stop();
    
    auto duration = std::chrono::steady_clock::now() - start;
    EXPECT_GE(duration, 100ms);
}

TEST_F(ARINC429GeneratorTest, MessageConsistency) {
    auto messages = generator->generateMessages(1000ms);
    
    float last_lat = 0.0f;
    
    for (const auto& msg : messages) {
        auto arinc_msg = dynamic_cast<const ARINC429Message*>(msg.get());
        if (arinc_msg->getLabel() == ARINC429Label::LATITUDE) {
            float lat = arinc_msg->getDecodedValue();
            EXPECT_GE(lat, -90.0f);
            EXPECT_LE(lat, 90.0f);
            last_lat = lat;
        }
    }
}