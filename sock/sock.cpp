#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <cstring>
#include <thread>
#include <vector>

#include "socket.h"

using namespace r3;
using namespace std;

/*
  You need dev packages to build and run this:
  sudo apt install libgles2-mesa-dev libglfw3-dev
*/

bool isServer = false;
double t0, t1;
Socket s;

static double getTimeInSeconds() {
  timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return double(int64_t(ts.tv_sec) * int64_t(1e9) + int64_t(ts.tv_nsec)) * 1e-9;
}

int main(int argc, char** argv) {
  isServer = strcmp(argv[0], "./server") == 0;

  const char* hostname = "localhost";
  if (argc == 2) {
    hostname = argv[1];
  }
  const char* message = "echo";
  if (argc == 3) {
    message = argv[2];
  }

  if (isServer) {
    printf("Server created!\n");
    Listener server;
    t0 = getTimeInSeconds();
    server.Listen(2113);
    printf("Waiting...\n");
    s = server.Accept();
    printf("Connected!\n");
    char input[100];
    int bytes = s.ReadPartial(input, 100);
    printf("Recieved '%s'\nSent back.\n", input);
    s.Write(input, bytes);
  } else {
    printf("Client created!\n");
    printf("Attempting connection...\n");
    s.Connect(GetIpAddress(hostname), 2113);
    printf("Connected!\nSending '%s'\n", message);
    bool success = s.Write(message, strlen(message) + 1);
    printf("Write to socket %s.\n", success ? "succeeded" : "failed");
    char input[100];
    int bytes = s.Read(input, 100);
    printf("Receiving '%s'\n", input);
  }

  return 0;
}
