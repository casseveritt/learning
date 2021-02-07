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
  if ((string)argv[0] == "./server") isServer = true;

  if (isServer) {
    printf("Server created!\n");
    Listener server;
    t0 = getTimeInSeconds();
    server.Listen(2113);
    printf("Waiting...\n");
    s = server.Accept();
    printf("Connected!\n");
    char* input;
    s.Read(*input);
    printf("Recieved '%s'\nSent back.\n", input);
    s.Write(input);
  } else {
    printf("Client created!\n");
    printf("Attempting connection...\n");
    s.Connect(GetIpAddress("luc"), 2113);
    printf("Connected!\nSending 'Echo'\n");
    s.Write("Echo");
    char* input;
    s.Read(*input);
    printf("Receiving '%s'\n", input);
  }

  return 0;
}
