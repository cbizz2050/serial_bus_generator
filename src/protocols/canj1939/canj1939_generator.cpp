#include "serial_bus_generator/protocols/canj1939/canj1939_generator.hpp"
#include <cmath>

namespace serial_bus_generator {

CANJ1939Generator::CANJ1939Generator()
    : rng_(std::random_device{}())
    , temp_variation_(-2.0f, 2.0f)
    , rpm_variation_(-50.0f, 50.0f)
{
    engine_state_.rpm = 750.0;  // Idle
    engine_state_.temperature = 25.0;  // Room temp
    engine_state_.hours = 0.0;
    engine_state_.running = true;
}

void CANJ1939Generator::startGeneration() {
    last_updates_.clear();
    DataGenerator::startGeneration();
}

std::vector<std::unique_ptr<IMessage>> CANJ1939Generator::generateMessages(
    std::chrono::milliseconds duration) {
    
    std::vector<std::unique_ptr<IMessage>> messages;
    
    if (engine_state_.running) {
        // Update engine state
        engine_state_.temperature += temp_variation_(rng_) * (duration.count() / 1000.0);
        engine_state_.rpm += rpm_variation_(rng_);
        engine_state_.hours += duration.count() / 3600000.0;  // Convert ms to hours

        // Clamp values
        engine_state_.temperature = std::max(-40.0, std::min(150.0, engine_state_.temperature));
        engine_state_.rpm = std::max(0.0, std::min(8000.0, engine_state_.rpm));
    }

    messages.push_back(generateEngineSpeedMessage());
    messages.push_back(generateEngineTemperatureMessage());
    messages.push_back(generateEngineHoursMessage());

    return messages;
}

void CANJ1939Generator::processMessages(std::vector<std::unique_ptr<IMessage>>&& messages) {
    if (!messages.empty()) {
        last_message_ = messages.back()->toString();
    }

    auto now = std::chrono::steady_clock::now();
    for (const auto& msg : messages) {
        if (auto j1939_msg = dynamic_cast<const CANJ1939Message*>(msg.get())) {
            last_updates_[j1939_msg->getPGN()] = now;
        }
    }
    
    DataGenerator::processMessages(std::move(messages));
}

std::unique_ptr<CANJ1939Message> CANJ1939Generator::generateEngineSpeedMessage() {
    return std::make_unique<CANJ1939Message>(
        CANJ1939PGN::ENGINE_SPEED,
        static_cast<float>(engine_state_.rpm),
        CANJ1939Priority::PRIORITY_3
    );
}

std::unique_ptr<CANJ1939Message> CANJ1939Generator::generateEngineTemperatureMessage() {
    return std::make_unique<CANJ1939Message>(
        CANJ1939PGN::ENGINE_TEMPERATURE,
        static_cast<float>(engine_state_.temperature),
        CANJ1939Priority::PRIORITY_3
    );
}

std::unique_ptr<CANJ1939Message> CANJ1939Generator::generateEngineHoursMessage() {
    return std::make_unique<CANJ1939Message>(
        CANJ1939PGN::ENGINE_HOURS,
        static_cast<float>(engine_state_.hours),
        CANJ1939Priority::PRIORITY_6
    );
}

std::string CANJ1939Generator::getLastMessage() {
    return last_message_;
}

} // namespace serial_bus_generator