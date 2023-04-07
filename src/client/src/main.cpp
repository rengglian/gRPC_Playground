#include <thread>
#include <memory>

#include <grpcpp/grpcpp.h>
#include "myproto/messages.grpc.pb.h"

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;
using grpc::ClientReader;

using dataservice::DataService;
using dataservice::DataRequest;
using dataservice::DataResponse;
using dataservice::HeartBeatResponse;

class DataClient {
public:
  DataClient(std::shared_ptr<Channel> channel)
      : stub_(DataService::NewStub(channel)), stop_client_(false) {}

  void StartClient() {
    client_thread_ = std::thread(&DataClient::RunClient, this);
  }

  void StopClient() {
    stop_client_.store(true);
    if (client_thread_.joinable()) {
      client_thread_.join();
    }
  }

private:
  void RunClient() {
    while (!stop_client_.load()) {
      std::string client_id("client1");
      std::string data = GetData(client_id);
      std::cout << "Data received: " << data << std::endl;
      std::this_thread::sleep_for(std::chrono::seconds(3));
    }
  }

  std::string GetData(const std::string &client_id) {
    DataRequest request;
    request.set_client_id(client_id);

    DataResponse response;
    ClientContext context;

    Status status = stub_->GetData(&context, request, &response);

    if (status.ok()) {
      return response.data();
    } else {
      std::cout << "gRPC call failed: " << status.error_message() << std::endl;
      return "RPC failed";
    }
  }

  std::unique_ptr<DataService::Stub> stub_;
  std::atomic<bool> stop_client_;
  std::thread client_thread_;
};

class ServerStreamClient {
public:
  ServerStreamClient(std::shared_ptr<Channel> channel)
      : stub_(DataService::NewStub(channel)), stop_heartbeat_(false) {}

  void StartHeartBeat() {
    heartbeat_thread_ = std::thread(&ServerStreamClient::RunHeartBeat, this);
  }

  void StopHeartBeat() {
    stop_heartbeat_.store(true);
    if (heartbeat_thread_.joinable()) {
      heartbeat_thread_.join();
    }
  }

private:
  void RunHeartBeat() {
    ::google::protobuf::Empty request;
    ClientContext context;
    HeartBeatResponse response;
    std::unique_ptr<ClientReader<HeartBeatResponse>> reader(stub_->HeartBeat(&context, request));

    while (!stop_heartbeat_.load() && reader->Read(&response)) {
      std::cout << "Heartbeat status: " << response.status() << std::endl;
    }

    Status status = reader->Finish();
    if (!status.ok()) {
      std::cout << "HeartBeat RPC failed: " << status.error_message() << std::endl;
    }
  }

  std::unique_ptr<DataService::Stub> stub_;
  std::atomic<bool> stop_heartbeat_;
  std::thread heartbeat_thread_;
};



int main(int argc, char **argv) {
  auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());

  DataClient data_client(channel);
  ServerStreamClient server_stream_client(channel);

  data_client.StartClient();
  server_stream_client.StartHeartBeat();

  // Wait for user input to stop the client and heartbeat
  std::cin.get();

  server_stream_client.StopHeartBeat();
  data_client.StopClient();

  return 0;
}