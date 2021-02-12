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
      dirContents.append("\n");
      string out;
      for (int i = 3; i > 0; i--) {
        if (dirContents.size() < pow(10, i)) out.append("0");
      }
      out.append(to_string(dirContents.size()));
      out.append(dirContents);
      sock.Write(out.c_str(), int(out.size()) + 1);
    } else
      sock.Write("0000", 5);
  }
  if (string(command).substr(0, 3) == string("get")) {
    string comm(command);
    FILE* file = fopen(string(command).substr(4, strlen(command) - 1).c_str(), "rb");
    if (file != NULL) {
      fseek(file, 0L, SEEK_END);
      int fileSize = ftell(file);
      rewind(file);
      int size = 8000;
      string totalSize;
      for (int i = 19; i > 1; i--) {
        if (fileSize < pow(10, i)) totalSize.append("0");
      }
      totalSize.append(to_string(fileSize));
      sock.Write(totalSize.c_str(), 21);
      sock.Write(to_string(size).c_str(), 5);
      int iterations = (fileSize - (fileSize % size)) / size;
      if (fileSize % size > 0) iterations++;
      for (int i = 0; i < iterations; i++) {
        int cSize = size;
        if (i == iterations - 1 && fileSize % size != 0) cSize = fileSize % size;
        printf("Chunk Size int: '%s'\n", to_string(cSize).c_str());
        string chunkSize;
        for (int j = 4; j > 1; j--) {
          if (cSize < pow(10, j)) {
            chunkSize.append("0");
            break;
          }
        }
        chunkSize.append(to_string(cSize));
        char readBuffer[cSize];
        fread(&readBuffer, 1, cSize, file);
        // printf("\nChunk %i: Size = %i\nEnd\n", i, size);
        // printf("%s\n\n%i : %i\n\n", readBuffer, (size*i)+cSize, int(ftell(file)));
        sock.Write(to_string(cSize).c_str(), 5);
        sock.Write(string(readBuffer).c_str(), cSize);
        // printf("Size: %i\n", int(strlen(readBuffer)));
      }
      printf("Transfered\n");
      fclose(file);
    } else
      sock.Write("00000000000000000000", 21);
  }
}

static void makeConnections() {
  while (!shouldClose) {
    server.Listen(port);
    if (int(connections.size()) < connectionLimit) {
      connections.push_back(server.Accept());
      connections.back().SetNonblocking();
      printf("Connection made!\n");
    }  // else
       // printf("Connection attempted! (Full)\n");
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
        serverCommand(input, conn);
      }
    }
  }
  for (Socket conn : connections) conn.Close();
}

// Client

static void clientCommand(string command) {
  if (command == string("quit")) {
    s.Write(command.c_str(), command.size() + 1);
    shouldClose = true;
  }
  if (command == string("ls")) {
    s.Write(command.c_str(), command.size() + 1);
    char size[4];
    s.ReadPartial(size, 4);
    int bytes = atoi(size);
    char cont[bytes + 1];
    if (string(size) != string("0000")) {
      s.ReadPartial(cont, bytes + 1);
      printf("%s", cont);
    } else
      printf("Error\n");
  }
  if (command.substr(0, 3) == string("get")) {
    s.Write(command.c_str(), command.size() + 1);
    char fileSize[20];
    s.ReadPartial(fileSize, 20);
    int bytes = atoi(fileSize);
    printf("Total Size: %i\n", bytes);
    char file[bytes + 1];
    if (string(fileSize) != string("00000000000000000000")) {
      int iterations = (bytes - (bytes % 8000)) / 8000;
      if (bytes % 8000 > 0) iterations++;
      for (int i = 0; i < iterations; i++) {
        char size[5];
        s.ReadPartial(size, 5);
        int fbytes = atoi(size);
        printf("Chunk Size: %i\n", fbytes);
        char fileFrag[fbytes + 1];
        s.ReadPartial(fileFrag, fbytes);
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
