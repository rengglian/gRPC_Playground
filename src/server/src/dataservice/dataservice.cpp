#include <thread>
#include <memory>
#include <mutex>
#include <fstream>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include "myproto/messages.grpc.pb.h"

#include "dataservice.h"

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

DataServiceImpl::DataServiceImpl(std::shared_ptr<Logger> logger)
    : logger_(logger), client_count_(0), stop_server_(false) {}

int DataServiceImpl::GetClientCount() const
{
  return client_count_.load();
}

void DataServiceImpl::StartServer()
{
  server_thread_ = std::thread(&DataServiceImpl::RunServer, this);
}

void DataServiceImpl::StopServer()
{
  stop_server_.store(true);
  if (server_thread_.joinable())
  {
    server_thread_.join();
  }
}

Status DataServiceImpl::HeartBeat(ServerContext *context, const ::google::protobuf::Empty *request, ServerWriter<HeartBeatResponse> *writer)
{
  HeartBeatResponse response;
  while (true)
  {
    response.set_status("Alive");
    if (!writer->Write(response))
    {
      break;
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  return Status::OK;
}

Status DataServiceImpl::GetData(ServerContext *context, const DataRequest *request, DataResponse *response)
{
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

void DataServiceImpl::RunServer()
{
  std::string server_address("0.0.0.0:50051");

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  ServerBuilder builder;

  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(this);

  server_ = builder.BuildAndStart();
  logger_->Log("Server listening on " + server_address);

  while (!stop_server_.load())
  {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  server_->Shutdown();
}