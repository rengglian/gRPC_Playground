#include <thread>
#include <memory>
#include <mutex>
#include <fstream>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include "myproto/messages.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerWriter;
using grpc::Status;
using grpc::ClientContext;
using grpc::ClientReader;

using dataservice::DataService;
using dataservice::DataRequest;
using dataservice::DataResponse;
using dataservice::HeartBeatResponse;

class Logger {
public:
  Logger(const std::string& log_file) : log_file_(log_file) {
    std::ofstream file(log_file_, std::ios::trunc);
  }

  void Log(const std::string& message) {
    std::lock_guard<std::mutex> lock(log_mutex_);
    std::ofstream file(log_file_, std::ios::app);
    if (file.is_open()) {
      file << message << std::endl;
    } else {
      std::cerr << "Unable to open log file: " << log_file_ << std::endl;
    }
  }

private:
  std::string log_file_;
  std::mutex log_mutex_;
};

class DataServiceImpl final : public DataService::Service {
public:
  DataServiceImpl(std::shared_ptr<Logger> logger)
      : logger_(logger), client_count_(0), stop_server_(false) {}

  int GetClientCount() const {
    return client_count_.load();
  }

  void StartServer() {
    server_thread_ = std::thread(&DataServiceImpl::RunServer, this);
  }

  void StopServer() {
    stop_server_.store(true);
    if (server_thread_.joinable()) {
      server_thread_.join();
    }
  }

  Status HeartBeat(ServerContext* context, const ::google::protobuf::Empty* request, ServerWriter<HeartBeatResponse>* writer) override {
    HeartBeatResponse response;
    while (true) {
      response.set_status("Alive");
      if (!writer->Write(response)) {
        break;
      }
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return Status::OK;
  }  

private:
  Status GetData(ServerContext *context, const DataRequest *request, DataResponse *response) override {
    {
      std::lock_guard<std::mutex> lock(connection_mutex_);
      ++client_count_;
    }

    std::string client_id = request->client_id();
    std::string data = "Data for client: " + client_id;
    response->set_data(data);

    {
      std::lock_guard<std::mutex> lock(connection_mutex_);
      --client_count_;
    }

    return Status::OK;
  }

  void RunServer() {
    std::string server_address("0.0.0.0:50051");

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;

    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(this);

    server_ = builder.BuildAndStart();
    logger_->Log("Server listening on " + server_address);

    while (!stop_server_.load()) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    server_->Shutdown();
  }

  std::shared_ptr<Logger> logger_;
  std::atomic<int> client_count_;
  std::mutex connection_mutex_;
  std::unique_ptr<Server> server_;
  std::atomic<bool> stop_server_;
  std::thread server_thread_;
};

class ClientMonitor {
public:
  ClientMonitor(std::shared_ptr<DataServiceImpl> service, std::shared_ptr<Logger> logger)
      : service_(service), logger_(logger), stop_monitoring_(false) {}

  void StartMonitoring() {
    monitoring_thread_ = std::thread(&ClientMonitor::MonitorClients, this);
  }

  void StopMonitoring() {
    stop_monitoring_.store(true);
    if (monitoring_thread_.joinable()) {
      monitoring_thread_.join();
    }
  }

private:
  void MonitorClients() {
    while (!stop_monitoring_.load()) {
      int client_count = service_->GetClientCount();
      logger_->Log("Connected clients: " + std::to_string(client_count));
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }

  std::shared_ptr<DataServiceImpl> service_;
  std::shared_ptr<Logger> logger_;
  std::atomic<bool> stop_monitoring_;
  std::thread monitoring_thread_;
};

int main(int argc, char **argv) {
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