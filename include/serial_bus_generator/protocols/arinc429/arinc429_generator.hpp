#pragma once

#include "serial_bus_generator/core/data_generator.hpp"
#include "serial_bus_generator/protocols/arinc429/arinc429_message.hpp"
#include <random>
#include <map>

namespace serial_bus_generator {
enum class FlightPhase {
    STOPPED,
    TAKEOFF,
    CRUISE,
    LANDING
};

class ARINC429Generator : public DataGenerator {
public:
    ARINC429Generator();
    ~ARINC429Generator() override = default;

    std::vector<std::unique_ptr<IMessage>> generateMessages(
        std::chrono::milliseconds duration) override;

    FlightPhase current_phase_ = FlightPhase::STOPPED;
    std::chrono::steady_clock::time_point phase_start_time_;

    struct Coordinates {
        double latitude;
        double longitude;
    };
    // Flight parameters
    static constexpr double CRUISE_ALTITUDE = 35000.0;  // feet
    static constexpr double TAKEOFF_RATE = 2000.0;      // feet per minute
    static constexpr double CRUISE_SPEED = 500.0;       // knots
    static constexpr double PHASE_DURATION = 300.0;     // seconds per phase
    const Coordinates START_POINT{47.6062, -122.3321};  // Seattle-Tacoma International
    const Coordinates END_POINT{25.7959, -80.2870};     // Miami International
    void updateFlightState(std::chrono::milliseconds delta_time);
    void transitionToNextPhase();

protected:
    void startGeneration() override;
    void processMessages(std::vector<std::unique_ptr<IMessage>>&& messages) override;
    std::string getLastMessage() override;

private:
    std::unique_ptr<ARINC429Message> generateLatitudeMessage();
    std::unique_ptr<ARINC429Message> generateLongitudeMessage();
    std::unique_ptr<ARINC429Message> generateStatusMessage();
    std::unique_ptr<ARINC429Message> generateSpeedMessage();
    std::unique_ptr<ARINC429Message> generateAltitudeMessage();
    void calculateInitialTrack();

    // State tracking for realistic data generation
    struct FlightState {
        double latitude{0.0};
        double longitude{0.0};
        double altitude{0.0};
        double ground_speed{0.0};
        double vertical_speed{0.0};
        double track{0.0};
    };

    FlightState flight_state_;
    std::mt19937 rng_;  // Random number generator
    std::uniform_real_distribution<float> status_dist_;
    std::map<ARINC429Label, std::chrono::steady_clock::time_point> last_updates_;
    std::vector<std::string> last_messages_;


};

} // namespace serial_bus_generator