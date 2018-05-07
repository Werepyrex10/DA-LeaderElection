#include "leader_election.hpp"

static std::string LEADER_IDLE("a");
static std::string LEADER_START("b");
static std::string LEADER_PROPOSE("c");
static std::string LEADER_ACK("d");
static std::string LEADER_ALIVE("e");

void startState(Node &my_node)
{
    auto results = my_node.gatherWithTimeout();
}

int proposeState(Node &my_node)
{
    int leader = NO_LEADER;
    auto results = my_node.gatherWithTimeout();
    results.push_back(std::to_string(my_node.getId()));

    for (int i = 0; i < results.size(); i ++) {
        std :: cout << results[i] << "\n";
        if (results[i].size() == 1) {
            continue;
        }
        int num = std::stoi(results[i].substr(1,1));
        if (leader < num) {
            leader = num;
        }
    }

    return leader;
}

bool ackState(Node &my_node, int leader)
{
    bool ack = true;
    auto results = my_node.gatherWithTimeout();
    for (int i = 0; i < results.size(); i++) {
        if (results[i][1] != std::to_string(leader)[0]) {
            return false;
        }
        else {
            std::cout << my_node.getId() << " diff " << results[i] << "\n";
        }
    }

    return true;
}

int main(int argc, char** argv) {
    Node my_node(TIMEOUT);
    int leader = NO_LEADER;

    //LEADER_START
    my_node.broadcast(LEADER_START);
    auto start = std::chrono::system_clock::now();
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = end - start;

    do {
        startState(my_node);
        end = std::chrono::system_clock::now();
        diff = end - start;
    } while (diff.count() < 1);

    //LEADER_PROPOSE
    my_node.broadcast(LEADER_PROPOSE + std::to_string(my_node.getId()));

    start = std::chrono::system_clock::now();
    do {
        int ret = proposeState(my_node);
        leader = std::max(leader, ret);

        end = std::chrono::system_clock::now();
        diff = end - start;
    } while (diff.count() < 1);

    //LEADER_ACK
    my_node.broadcast(LEADER_ACK + std::to_string(leader));

    if (leader == my_node.getId()) {
        my_node.setLeaderId(leader);
    }
    else {
        start = std::chrono::system_clock::now();
        do {
            auto ret = ackState(my_node, leader);
            if (ret) {
                my_node.setLeaderId(leader);
            }

            end = std::chrono::system_clock::now();
            diff = end - start;

        } while (diff.count() < 1);
    }

    std:: cout << my_node.getId() << " " << my_node.getLeaderId() << std::endl;

    return 0;
}
