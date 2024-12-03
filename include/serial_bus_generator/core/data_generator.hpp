#pragma once

#include "serial_bus_generator/interfaces/generator_interface.hpp"
#include <atomic>
#include <string>
#include <thread>
namespace serial_bus_generator {

class DataGenerator : public IGenerator {
public:
    static constexpr uint32_t MAX_RATE = 1000;  // Maximum 1kHz rate

    DataGenerator();
    ~DataGenerator() override;

    // IGenerator interface implementation
    void start() override;
    void stop() override;
    void setRate(uint32_t rate) override;
    GeneratorState getState() const override;
    virtual std::vector<std::unique_ptr<IMessage>> generateMessages(
        std::chrono::milliseconds duration) override = 0;

private:
    std::thread generation_thread_;

protected:
    // Template method pattern for protocol-specific generation
    virtual void startGeneration();
    virtual void stopGeneration();
    virtual void handleError(const std::string& error);
    virtual void processMessages(std::vector<std::unique_ptr<IMessage>>&& messages);

    std::atomic<GeneratorState> state_;
    std::atomic<uint32_t> rate_;
    std::atomic<bool> running_;
    std::atomic<size_t> message_count_{0};
    std::string last_error_;
};

} // namespace serial_bus_generator