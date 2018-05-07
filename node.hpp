#pragma once

#include <mpi.h>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <algorithm>
#include <iostream>
#include <cstring>

#define NO_LEADER       -1
#define MSG_SIZE        64

class Node
{
public:
    Node(int timeout);
    ~Node();

    void send(int dst, std::string msg);
    std::string receive(int src, int timeout);

    void broadcast(std::string msg);
    std::vector<std::string> gather();
    std::vector<std::string> gatherWithTimeout();

    bool checkStatus(int leader, std::string msg);

    int getId();
    int getLeaderId();
    int getState();

    void setLeaderId(int leader);
    void setState(int state);
private:
    int id, size, timeout, leader, state;
    std::vector<MPI_Request> sends;
};
