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
    while (server.Listen(127.0.0.1) == false) {
      if (getTimeInSeconds() - t0 >= 10) {
        printf("Waiting...\n");
        t0 = getTimeInSeconds();
      }
    }
    s = server.Accept(127.0.0.1);
  } else {
    printf("Client created!\n");
  }

  return 0;
}
