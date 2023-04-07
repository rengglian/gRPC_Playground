#pragma once

#include <thread>
#include <memory>
#include <mutex>
#include <fstream>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include "myproto/messages.grpc.pb.h"

#include "logger/logger.h"

using grpc::ClientContext;
using grpc::ClientReader;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerWriter;
using grpc::Status;

using dataservice::DataRequest;
using dataservice::DataResponse;
using dataservice::DataService;
using dataservice::HeartBeatResponse;

class DataServiceImpl final : public DataService::Service
{
public:
  DataServiceImpl(std::shared_ptr<Logger> logger);
  int GetClientCount() const;

  void StartServer();

  void StopServer();

  Status HeartBeat(ServerContext *context, const ::google::protobuf::Empty *request, ServerWriter<HeartBeatResponse> *writer) override;

private:
  Status GetData(ServerContext *context, const DataRequest *request, DataResponse *response) override;

  void RunServer();

  std::shared_ptr<Logger> logger_;
  std::atomic<int> client_count_;
  std::mutex connection_mutex_;
  std::unique_ptr<Server> server_;
  std::atomic<bool> stop_server_;
  std::thread server_thread_;
};