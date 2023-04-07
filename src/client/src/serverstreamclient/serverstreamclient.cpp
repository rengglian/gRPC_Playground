#include "serverstreamclient.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::Status;

using dataservice::DataRequest;
using dataservice::DataResponse;
using dataservice::DataService;
using dataservice::HeartBeatResponse;

ServerStreamClient::ServerStreamClient(std::shared_ptr<Channel> channel)
    : stub_(DataService::NewStub(channel)), stop_heartbeat_(false) {}

void ServerStreamClient::StartHeartBeat()
{
    heartbeat_thread_ = std::thread(&ServerStreamClient::RunHeartBeat, this);
}

void ServerStreamClient::StopHeartBeat()
{
    stop_heartbeat_.store(true);
    if (heartbeat_thread_.joinable())
    {
        heartbeat_thread_.join();
    }
}

void ServerStreamClient::RunHeartBeat()
{
    while (!stop_heartbeat_.load())
    {
        ::google::protobuf::Empty request;
        ClientContext context;
        HeartBeatResponse response;
        std::unique_ptr<ClientReader<HeartBeatResponse>> reader(stub_->HeartBeat(&context, request));

        while (!stop_heartbeat_.load() && reader->Read(&response))
        {
            std::cout << "Heartbeat status: " << response.status() << std::endl;
        }

        Status status = reader->Finish();
        if (!status.ok())
        {
            std::cout << "HeartBeat RPC failed: " << status.error_message() << std::endl;
        }

        if (!stop_heartbeat_.load())
        {
            std::cout << "Attempting to reconnect in 5 seconds..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
    }
}