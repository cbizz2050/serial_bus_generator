#include "serial_bus_generator/protocols/arinc429/arinc429_generator.hpp"
#include <cmath>

namespace serial_bus_generator {

ARINC429Generator::ARINC429Generator()
    : rng_(std::random_device{}())
    , status_dist_(0.0f, 1.0f)
{
    flight_state_.latitude = START_POINT.latitude;
    flight_state_.longitude = START_POINT.longitude;
    flight_state_.altitude = 0.0;
    flight_state_.ground_speed = 0.0;
    flight_state_.vertical_speed = 0.0;
    calculateInitialTrack();
}

void ARINC429Generator::calculateInitialTrack() {
    // Calculate initial bearing between start and end points
    double lat1 = START_POINT.latitude * M_PI / 180.0;
    double lat2 = END_POINT.latitude * M_PI / 180.0;
    double dLon = (END_POINT.longitude - START_POINT.longitude) * M_PI / 180.0;

    double y = sin(dLon) * cos(lat2);
    double x = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(dLon);
    double initial_bearing = atan2(y, x);
    
    // Convert from radians to degrees
    flight_state_.track = fmod((initial_bearing * 180.0 / M_PI + 360.0), 360.0);
}

void ARINC429Generator::startGeneration() {
    last_updates_.clear();
    current_phase_ = FlightPhase::TAKEOFF;
    phase_start_time_ = std::chrono::steady_clock::now();
    DataGenerator::startGeneration();
}

std::vector<std::unique_ptr<IMessage>> ARINC429Generator::generateMessages(std::chrono::milliseconds delta_time) {
    updateFlightState(delta_time);
    
    std::vector<std::unique_ptr<IMessage>> messages;
    messages.push_back(generateLatitudeMessage());
    messages.push_back(generateLongitudeMessage());
    messages.push_back(generateSpeedMessage());
    messages.push_back(generateAltitudeMessage());
    return messages;
}

void ARINC429Generator::updateFlightState(std::chrono::milliseconds delta_time) {
    auto now = std::chrono::steady_clock::now();
    double phase_elapsed = std::chrono::duration<double>(now - phase_start_time_).count();
    
    if (phase_elapsed > PHASE_DURATION) {
        transitionToNextPhase();
        return;
    }

    double delta_seconds = delta_time.count() / 1000.0;
    
    // Calculate position changes based on speed and track
    // Convert speed from knots to degrees per second (approximate at given latitude)
    const double KNOTS_TO_DEGREES_PER_SECOND = 1.0 / 3600.0;  // rough approximation
    double distance = flight_state_.ground_speed * delta_seconds * KNOTS_TO_DEGREES_PER_SECOND;
    
    // Calculate lat/lon changes based on track angle
    double track_rad = flight_state_.track * M_PI / 180.0;
    double lat_change = distance * cos(track_rad);
    double lon_change = distance * sin(track_rad);
    
    switch (current_phase_) {
        case FlightPhase::TAKEOFF:
            flight_state_.altitude += TAKEOFF_RATE * delta_seconds / 60.0;
            flight_state_.ground_speed = std::min(CRUISE_SPEED, 
                flight_state_.ground_speed + 50.0 * delta_seconds);
            flight_state_.latitude += lat_change;
            flight_state_.longitude += lon_change;
            break;
            
        case FlightPhase::CRUISE:
            flight_state_.altitude = CRUISE_ALTITUDE;
            flight_state_.ground_speed = CRUISE_SPEED;
            flight_state_.latitude += lat_change;
            flight_state_.longitude += lon_change;
            break;
            
        case FlightPhase::LANDING:
            flight_state_.altitude = std::max(0.0, 
                flight_state_.altitude - TAKEOFF_RATE * delta_seconds / 60.0);
            flight_state_.ground_speed = std::max(0.0, 
                flight_state_.ground_speed - 50.0 * delta_seconds);
            flight_state_.latitude += lat_change;
            flight_state_.longitude += lon_change;
            break;
            
        case FlightPhase::STOPPED:
            flight_state_.altitude = 0.0;
            flight_state_.ground_speed = 0.0;
            break;
    }
}
void ARINC429Generator::transitionToNextPhase() {
    phase_start_time_ = std::chrono::steady_clock::now();
    
    switch (current_phase_) {
        case FlightPhase::TAKEOFF:
            current_phase_ = FlightPhase::CRUISE;
            break;
        case FlightPhase::CRUISE:
            current_phase_ = FlightPhase::LANDING;
            break;
        case FlightPhase::LANDING:
            current_phase_ = FlightPhase::STOPPED;
            break;
        case FlightPhase::STOPPED:
            current_phase_ = FlightPhase::TAKEOFF;
            // Reset position for return flight
            if (std::abs(flight_state_.latitude - START_POINT.latitude) < 0.1) {
                flight_state_.latitude = END_POINT.latitude;
                flight_state_.longitude = END_POINT.longitude;
            } else {
                flight_state_.latitude = START_POINT.latitude;
                flight_state_.longitude = START_POINT.longitude;
            }
            calculateInitialTrack();
            break;
    }
}

void ARINC429Generator::processMessages(std::vector<std::unique_ptr<IMessage>>&& messages) {
    last_messages_.clear();
    for (const auto& msg : messages) {
        last_messages_.push_back(msg->toString());
    }
    DataGenerator::processMessages(std::move(messages));   
}

std::unique_ptr<ARINC429Message> ARINC429Generator::generateLatitudeMessage() {
    return std::make_unique<ARINC429Message>(
        ARINC429Label::LATITUDE,
        static_cast<float>(flight_state_.latitude),
        ARINC429SSM::NORMAL_OPERATION
    );
}

std::unique_ptr<ARINC429Message> ARINC429Generator::generateLongitudeMessage() {
    return std::make_unique<ARINC429Message>(
        ARINC429Label::LONGITUDE,
        static_cast<float>(flight_state_.longitude),
        ARINC429SSM::NORMAL_OPERATION
    );
}

std::unique_ptr<ARINC429Message> ARINC429Generator::generateStatusMessage() {
    return std::make_unique<ARINC429Message>(
        ARINC429Label::EQUIPMENT_STATUS,
        1.0f,
        ARINC429SSM::NORMAL_OPERATION
    );
}

std::unique_ptr<ARINC429Message> ARINC429Generator::generateSpeedMessage() {
    return std::make_unique<ARINC429Message>(
        ARINC429Label::GROUND_SPEED,
        static_cast<float>(flight_state_.ground_speed),
        ARINC429SSM::NORMAL_OPERATION
    );
}

std::unique_ptr<ARINC429Message> ARINC429Generator::generateAltitudeMessage() {
    return std::make_unique<ARINC429Message>(
        ARINC429Label::ALTITUDE,
        static_cast<float>(flight_state_.altitude),
        ARINC429SSM::NORMAL_OPERATION
    );
}

std::string ARINC429Generator::getLastMessage() {
    // Combine all messages into a single string
    std::string combined;
    for (const auto& msg : last_messages_) {
        if (!combined.empty()) {
            combined += "\n";
        }
        combined += msg;
    }
    return combined;
}
} // namespace serial_bus_generator