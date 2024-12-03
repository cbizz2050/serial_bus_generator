#pragma once

#include "serial_bus_generator/core/data_generator.hpp"
#include "serial_bus_generator/protocols/canj1939/canj1939_message.hpp"
#include <random>
#include <map>

namespace serial_bus_generator {

class CANJ1939Generator : public DataGenerator {
public:
    CANJ1939Generator();
    ~CANJ1939Generator() override = default;

    std::vector<std::unique_ptr<IMessage>> generateMessages(
        std::chrono::milliseconds duration) override;

protected:
    void startGeneration() override;
    void processMessages(std::vector<std::unique_ptr<IMessage>>&& messages) override;
    std::string getLastMessage() override;

private:
    std::unique_ptr<CANJ1939Message> generateEngineSpeedMessage();
    std::unique_ptr<CANJ1939Message> generateEngineTemperatureMessage();
    std::unique_ptr<CANJ1939Message> generateEngineHoursMessage();

    struct EngineState {
        double rpm{0.0};
        double temperature{25.0};  // Celsius
        double hours{0.0};
        double fuel_level{100.0};  // Percentage
        bool running{false};
    };

    EngineState engine_state_;
    std::mt19937 rng_;
    std::uniform_real_distribution<float> temp_variation_;
    std::uniform_real_distribution<float> rpm_variation_;
    std::map<CANJ1939PGN, std::chrono::steady_clock::time_point> last_updates_;
    std::string last_message_;
};

} // namespace serial_bus_generator