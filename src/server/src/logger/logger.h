#pragma once

#include <memory>
#include <mutex>
#include <fstream>
#include <iostream>

class Logger
{
public:
  Logger(const std::string &log_file);

  void Log(const std::string &message);

private:
  std::string log_file_;
  std::mutex log_mutex_;
};