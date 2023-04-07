#pragma once

#include <memory>
#include <atomic>
#include <thread>

#include "logger/logger.h"
#include "dataservice/dataservice.h"

class ClientMonitor
{
public:
  ClientMonitor(std::shared_ptr<DataServiceImpl> service, std::shared_ptr<Logger> logger);

  void StartMonitoring();

  void StopMonitoring();

private:
  void MonitorClients();

  std::shared_ptr<DataServiceImpl> service_;
  std::shared_ptr<Logger> logger_;
  std::atomic<bool> stop_monitoring_;
  std::thread monitoring_thread_;
};