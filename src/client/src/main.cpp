#include <thread>
#include <memory>

#include <grpcpp/grpcpp.h>
#include "myproto/messages.grpc.pb.h"

#include "dataclient/dataclient.h"
#include "serverstreamclient/serverstreamclient.h"


int main(int argc, char **argv)
{
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