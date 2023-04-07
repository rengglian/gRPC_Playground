#include "dataclient.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::Status;

using dataservice::DataRequest;
using dataservice::DataResponse;
using dataservice::DataService;
using dataservice::HeartBeatResponse;

DataClient::DataClient(std::shared_ptr<Channel> channel)
    : stub_(DataService::NewStub(channel)), stop_client_(false) {}

void DataClient::StartClient()
{
    client_thread_ = std::thread(&DataClient::RunClient, this);
}

void DataClient::StopClient()
{
    stop_client_.store(true);
    if (client_thread_.joinable())
    {
        client_thread_.join();
    }
}

void DataClient::RunClient()
{
    while (!stop_client_.load())
    {
        std::string client_id("client1");
        std::string data = GetData(client_id);
        std::cout << "Data received: " << data << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
}

std::string DataClient::GetData(const std::string &client_id)
{
    DataRequest request;
    request.set_client_id(client_id);

    DataResponse response;
    ClientContext context;

    Status status = stub_->GetData(&context, request, &response);

    if (status.ok())
    {
        return response.data();
    }
    else
    {
        std::cout << "gRPC call failed: " << status.error_message() << std::endl;
        return "RPC failed";
    }
}