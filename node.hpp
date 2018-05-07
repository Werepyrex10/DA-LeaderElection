#pragma once

#include <mpi.h>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <algorithm>

#define LEADER_IDLE     "a"
#define LEADER_START    "b"
#define LEADER_PROPOSE  "b"
#define LEADER_ACK      "d"

#define LEADER_ALIVE    "e"

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

    bool checkLeaderStatus();

    int getId();
    int getLeaderId();
    int getState();

    void setLeaderId(int leader);
    void setState(int state);
private:
    int id, size, timeout, leader, state;
    std::vector<MPI_Request> sends;
};
