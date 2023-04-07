#include <thread>
#include <memory>

#include <grpcpp/grpcpp.h>
#include "myproto/messages.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using dataservice::DataService;
using dataservice::DataRequest;
using dataservice::DataResponse;

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

int main(int argc, char **argv) {
  DataClient client(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));

  client.StartClient();

  // Wait for user input to stop the client
  std::cin.get();

  client.StopClient();

  return 0;
}