///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <dirent.h>
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
    shouldClose = true;
  }
  if (strcmp(command, "ls") == 0) {
    DIR* dir = opendir(".");
    if (dir) {
      string dirContents;
      struct dirent* en;
      while ((en = readdir(dir)) != NULL) {
        dirContents.append(en->d_name);
        dirContents.append("\t");
      }
      closedir(dir);
      int lsLength = dirContents.size();
      sock.Write(lsLength);
      sock.Write(dirContents.c_str(), (dirContents.size() + 1));
    } else
      sock.Write(0);
  }
  if (string(command).substr(0, 3) == string("get")) {
    string comm(command);
    FILE* file = fopen(string(command).substr(4, strlen(command) - 1).c_str(), "rb");
    if (file != NULL) {
      fseek(file, 0L, SEEK_END);
      int fileSize = ftell(file);
      rewind(file);
      constexpr int size = 8000;
      char readBuffer[size + 1];
      sock.Write(fileSize);
      sock.Write(size);
      while (fileSize > 0) {
        int cSize = size;
        if (fileSize < size) cSize = fileSize;
        fread(&readBuffer, 1, cSize, file);
        printf("Fragment size %i\n", int(ftell(file)));
        sock.Write(readBuffer, cSize);
        fileSize -= cSize;
      }
      fclose(file);
    } else
      sock.Write(0);
  }
}

static void makeConnections() {
  while (!shouldClose) {
    server.Listen(port);
    if (int(connections.size()) < connectionLimit) {
      connections.push_back(server.Accept());
      connections.back().SetNonblocking();
      printf("Connection made!\n");
    }
  }
}

static void serverMain() {
  server.SetNonblocking();

  printf("Server created!\n");
  t0 = getTimeInSeconds();
  thread connect = thread(makeConnections);
  int bytes = 100;
  char input[bytes + 1];
  while (!shouldClose) {
    for (Socket conn : connections) {
      if (conn.CanRead()) {
        int cbytes = 0;
        conn.Read(cbytes);
        memset(input, 0, 100);
        conn.ReadPartial(input, bytes);
        serverCommand(input, conn);
      }
    }
  }
  for (Socket conn : connections) conn.Close();
}

// Client

static void clientCommand(string command) {
  if (command == string("quit")) {
    s.Write(int(command.size()));
    s.Write(command.c_str(), command.size() + 1);
    shouldClose = true;
  }
  if (command == string("ls")) {
    s.Write(int(command.size()));
    s.Write(command.c_str(), command.size() + 1);
    int lsLength = 0;
    s.Read(lsLength);
    char cont[lsLength + 1];
    if (lsLength > 0) {
      s.ReadPartial(cont, lsLength + 1);
      printf("%s\n", cont);
    } else
      printf("Error\n");
  }
  if (command.substr(0, 3) == string("get")) {
    s.Write(int(command.size()));
    s.Write(command.c_str(), command.size() + 1);
    constexpr int size = 8000;
    int bytes = 0;
    s.Read(bytes);
    s.Read(size);
    char fileFrag[size + 1];
    if (bytes > 0) {
      // FILE* out = fopen("out.txt", "w");
      while (bytes > 0) {
        int fbytes = size;
        if (bytes < size) fbytes = bytes;
        printf("Chunk Size: %i\n", fbytes);
        if (fbytes > size) printf("Chunk size exceeds buffer size - fbytes=%d, chunkSize=%d!\n", fbytes, size);
        int actualBytes = s.ReadPartial(fileFrag, fbytes);
        if (fbytes != actualBytes) printf("We received fewer bytes than expected!\n");
        // fwrite(fileFrag, 1, fbytes, out);
        bytes -= fbytes;
      }
    } else
      printf("Error\n");
  }
}

static void clientMain() {
  printf("Client created!\n");
  printf("Attempting connection...\n");
  s.Connect(GetIpAddress(hostname), port);
  printf("Connected!\n");
  string command;
  while (!shouldClose) {
    getline(cin, command);
    clientCommand(command);
  }
  s.Close();
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
