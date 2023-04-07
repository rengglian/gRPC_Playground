#pragma once

#include <thread>
#include <atomic>
#include <memory>

#include <grpcpp/grpcpp.h>
#include "myproto/messages.grpc.pb.h"

class ServerStreamClient
{
public:
  ServerStreamClient(std::shared_ptr<grpc::Channel> channel);
  void StartHeartBeat();
  void StopHeartBeat();

private:
  void RunHeartBeat();

  std::unique_ptr<dataservice::DataService::Stub> stub_;
  std::atomic<bool> stop_heartbeat_;
  std::thread heartbeat_thread_;
};