#include <memory>
#include <atomic>
#include <thread>

#include "clientmonitor.h"

ClientMonitor::ClientMonitor(std::shared_ptr<DataServiceImpl> service, std::shared_ptr<Logger> logger)
    : service_(service), logger_(logger), stop_monitoring_(false) {}

void ClientMonitor::StartMonitoring()
{
  monitoring_thread_ = std::thread(&ClientMonitor::MonitorClients, this);
}

void ClientMonitor::StopMonitoring()
{
  stop_monitoring_.store(true);
  if (monitoring_thread_.joinable())
  {
    monitoring_thread_.join();
  }
}

void ClientMonitor::MonitorClients()
{
  while (!stop_monitoring_.load())
  {
    int client_count = service_->GetClientCount();
    logger_->Log("Connected clients: " + std::to_string(client_count));
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}