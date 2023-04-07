#pragma once

#include <thread>
#include <memory>

#include <grpcpp/grpcpp.h>
#include "myproto/messages.grpc.pb.h"

class DataClient
{
public:
  DataClient(std::shared_ptr<grpc::Channel> channel);

  void StartClient();

  void StopClient();

private:
  void RunClient();

  std::string GetData(const std::string &client_id);

  std::unique_ptr<dataservice::DataService::Stub> stub_;
  std::atomic<bool> stop_client_;
  std::thread client_thread_;
};
