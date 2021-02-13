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
#include <mutex>
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
mutex connectionsMutex;
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
      printf("len: %d, val: %s\n", lsLength, dirContents.c_str());
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
  server.Listen(port);
  server.SetNonblocking();
  while (!shouldClose) {
    if (int(connections.size()) < connectionLimit) {
      Socket conn = server.Accept();
      if (conn.s != -1) {
        const lock_guard<mutex> lock(connectionsMutex);
        connections.push_back(conn);
        printf("Connection made!\n");
      } else {
        usleep(1000 * 1000);  // sleep for 1 second on failed accept
      }
    }
  }
}

static void serverMain() {
  // server.SetNonblocking();

  printf("Server created!\n");
  t0 = getTimeInSeconds();
  thread connect = thread(makeConnections);
  int bytes = 100;
  char input[bytes + 1];
  while (!shouldClose) {
    const lock_guard<mutex> lock(connectionsMutex);
    for (Socket conn : connections) {
      if (conn.CanRead()) {
        int cbytes = 0;
        conn.Read(cbytes);
        // printf("command bytes = %d\n", cbytes);
        memset(input, 0, 100 + 1);
        conn.ReadPartial(input, bytes);
        // printf("command read: %d, %s\n", conn.s, input);
        serverCommand(input, conn);
      }
    }
  }
  {
    const lock_guard<mutex> lock(connectionsMutex);
    for (Socket conn : connections) conn.Close();
    if (connect.joinable()) {
      connect.join();
    }
  }
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
      FILE* out = fopen("out.txt", "w");
      while (bytes > 0) {
        int fbytes = size;
        if (bytes < size) fbytes = bytes;
        printf("Chunk Size: %i\n", fbytes);
        if (fbytes > size) printf("Chunk size exceeds buffer size - fbytes=%d, chunkSize=%d!\n", fbytes, size);
        int actualBytes = s.ReadPartial(fileFrag, fbytes);
        if (fbytes != actualBytes) printf("We received fewer bytes than expected!\n");
        fwrite(fileFrag, 1, fbytes, out);
        bytes -= fbytes;
      }
      fclose(out);
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
  isServer = strstr(argv[0], "server") != 0;

  if (argc >= 2) hostname = argv[1];

  if (isServer)
    serverMain();
  else
    clientMain();

  return 0;
}
