#include <thread>
#include <memory>
#include <mutex>
#include <fstream>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include "myproto/messages.grpc.pb.h"

#include "logger/logger.h"
#include "clientmonitor/clientmonitor.h"
#include "dataservice/dataservice.h"

int main(int argc, char **argv)
{
  auto logger = std::make_shared<Logger>("trace.log");
  auto service = std::make_shared<DataServiceImpl>(logger);

  service->StartServer();

  ClientMonitor client_monitor(service, logger);
  client_monitor.StartMonitoring();

  // Wait for user input to stop the server and monitoring
  std::cin.get();

  client_monitor.StopMonitoring();
  service->StopServer();

  return 0;
}