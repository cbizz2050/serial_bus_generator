#include "serial_bus_generator/core/data_generator.hpp"
#include <thread>
#include <stdexcept>

namespace serial_bus_generator {

DataGenerator::DataGenerator()
    : state_(GeneratorState::STOPPED)
    , rate_(100)  // Default 100Hz
    , running_(false)
{}

DataGenerator::~DataGenerator() {
    stop();
}

void DataGenerator::start() {
    if (state_ == GeneratorState::RUNNING) {
        return;
    }

    running_ = true;
    state_ = GeneratorState::RUNNING;
    generation_thread_ = std::thread(&DataGenerator::startGeneration, this);
}


void DataGenerator::stop() {
    if (state_ == GeneratorState::STOPPED) {
        return;
    }

    running_ = false;
    if (generation_thread_.joinable()) {
        generation_thread_.join();
    }
    state_ = GeneratorState::STOPPED;
    stopGeneration();
}


void DataGenerator::setRate(uint32_t rate) {
    if (rate == 0 || rate > MAX_RATE) {
        throw std::invalid_argument("Invalid rate specified");
    }
    rate_ = rate;
}

GeneratorState DataGenerator::getState() const {
    return state_;
}

void DataGenerator::startGeneration() {
    while (running_) {
        try {
            auto messages = generateMessages(std::chrono::milliseconds(1000 / rate_));
            processMessages(std::move(messages));
            
            std::this_thread::sleep_for(std::chrono::milliseconds(1000 / rate_));
        } catch (const std::exception& e) {
            state_ = GeneratorState::ERROR;
            running_ = false;
            handleError(e.what());
        }
    }
}

void DataGenerator::stopGeneration() {
    running_ = false;
}

void DataGenerator::handleError(const std::string& error) {
    last_error_ = error;
}

void DataGenerator::processMessages(std::vector<std::unique_ptr<IMessage>>&& messages) {
    message_count_ += messages.size();
}

} // namespace serial_bus_generator