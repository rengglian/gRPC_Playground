#include "logger.h"

  Logger::Logger(const std::string& log_file) : log_file_(log_file) {
    std::ofstream file(log_file_, std::ios::trunc);
  }

  void Logger::Log(const std::string& message) {
    std::lock_guard<std::mutex> lock(log_mutex_);
    std::ofstream file(log_file_, std::ios::app);
    if (file.is_open()) {
      file << message << std::endl;
    } else {
      std::cerr << "Unable to open log file: " << log_file_ << std::endl;
    }
  }