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

const char* hostname = "localhost";
bool shouldClose = false, isServer = false;
int port = 2113;
double t0, t1;
Socket s;

Listener server;
vector<Socket> connections;
int connectionLimit = 4;

static double getTimeInSeconds() {
  timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return double(int64_t(ts.tv_sec) * int64_t(1e9) + int64_t(ts.tv_nsec)) * 1e-9;
}

// Server

static void serverCommand(char* command, Socket& sock) {
  if (strcmp(command, "quit") == 0) {
    sock.Close();
    for (const int i = 0; i < connections.size(); i++) {
      if (connections[i] == sock) {
        sock.Close();
        connections.erase(i);
      }
    }
    if (connections.size() == 0) shouldClose = true;
  }
}

static void makeConnections() {
  while (!shouldClose) {
    server.Listen(port);
    if (int(connections.size()) < connectionLimit) {
      connections.push_back(server.Accept());
      connections.back().SetNonblocking();
      printf("Connection made!\n");
    } else
      printf("Connection attempted! (Full)\n");
  }
}

static void serverMain() {
  server.SetNonblocking();

  printf("Server created!\n");
  t0 = getTimeInSeconds();
  thread connect = thread(makeConnections);
  int bytes = 100;
  while (!shouldClose) {
    char input[bytes];
    for (Socket conn : connections) {
      if (conn.CanRead()) {
        conn.ReadPartial(input, bytes);
        printf("Input: '%s'\n", input);
        serverCommand(input, conn);
      }
    }
  }
  for (Socket conn : connections) conn.Close();
}

// Client

static void clientCommand(string command) {
  const char* comm;
  if (command == "quit") {
    comm = "quit";
    s.Write(comm, strlen(comm) + 1);
    shouldClose = true;
  }
}

static void clientMain() {
  printf("Client created!\n");
  printf("Attempting connection...\n");
  s.Connect(GetIpAddress(hostname), port);
  printf("Connected!\n");
  string command;
  while (!shouldClose) {
    cin >> command;
    clientCommand(command);
  }
}

// Main

int main(int argc, char** argv) {
  isServer = strcmp(argv[0], "./server") == 0;

  if (argc >= 2) hostname = argv[1];

  if (isServer)
    serverMain();
  else
    clientMain();

  return 0;
}
