#include "node.hpp"

Node::Node(int timeout) : timeout(timeout), leader(NO_LEADER)
{
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
}

Node::~Node()
{
    for (int i = 0; i < sends.size(); i ++) {
        MPI_Cancel(&sends[i]);
    }
    MPI_Finalize();
}

void Node::send(int dst, std::string msg)
{
    if (dst == id) {
        return;
    }
    MPI_Request request;

    MPI_Isend(msg.c_str(), msg.size(), MPI_CHAR, dst, 0, MPI_COMM_WORLD, &request);
    sends.push_back(request);
}

std::string Node::receive(int src, int timeout)
{
    MPI_Status status;
    MPI_Request request;
    char msg[MSG_SIZE];
    memset(msg, 0, MSG_SIZE * sizeof(char));
    int flag = 0;
    MPI_Iprobe(src, 0, MPI_COMM_WORLD, &flag, &status);
    if (!flag) {
        std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
        MPI_Iprobe(src, 0, MPI_COMM_WORLD, &flag, &status);
    }

    if (flag) {
        MPI_Irecv(msg, MSG_SIZE, MPI_CHAR, src, 0, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, &status);

        return std::string(msg);
    }

    return "";
}

void Node::broadcast(std::string msg)
{
    for (int i = 0; i < size; i ++) {
        send(i, msg);
    }
}

std::vector<std::string> Node::gather()
{
    std::vector<std::string> ret;
    for (int i = 0; i < size; i ++) {
        if (i == id) {
            continue;
        }
        
        std::string msg = receive(i, 0);
        if (!msg.empty()) {
            ret.push_back(msg);
        }
    }

    return ret;
}

std::vector<std::string> Node::gatherWithTimeout()
{
    std::vector<std::string> ret;

    for (int i = 0; i < size; i ++) {
        if (i == id) {
            continue;
        }

        std::string msg = receive(i, timeout);
        if (!msg.empty()) {
            ret.push_back(msg);
        }
    }

    return ret;
}

bool Node::checkStatus(int leader, std::string msg)
{
    if (leader == NO_LEADER) {
        return false;
    }

    send(leader, msg);

    std::string ret = receive(leader, timeout);

    return !ret.empty();
}

int Node::getId() { return id; }

int Node::getLeaderId() { return leader; }

int Node::getState() { return state; }

void Node::setLeaderId(int leader) { this->leader = leader; }

void Node::setState(int state) { this->state = state; }
