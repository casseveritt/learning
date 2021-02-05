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

//#include "socket.h"

using namespace std;

/*
  You need dev packages to build and run this:
  sudo apt install libgles2-mesa-dev libglfw3-dev
*/

bool isServer = false;

int main(int argc, char** argv) {
  if ((string)argv[0] == "./server") isServer = true;

  if (isServer) {
    printf("Server created!\n");
    // while ()
  } else {
    printf("Client created!\n");
  }

  return 0;
}
