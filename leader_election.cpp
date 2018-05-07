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
    results.push_back(LEADER_PROPOSE + std::to_string(my_node.getId()));

    for (int i = 0; i < results.size(); i ++) {
        if (results[i].size() == 1) {
            continue;
        }
        int num = results[i][1] - '0';
        if (leader <= num) {
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
    }

    return true;
}

int main(int argc, char** argv) {
    Node my_node(TIMEOUT);
    int leader = NO_LEADER;
    auto start = std::chrono::system_clock::now();
    auto end = start;
    std::chrono::duration<double> diff;

    //LEADER_START
    my_node.broadcast(LEADER_START);
    do {
        startState(my_node);
        end = std::chrono::system_clock::now();
        diff = end - start;
    } while (diff.count() < ELECTION_TIMEOUT);

    //LEADER_PROPOSE
    my_node.broadcast(LEADER_PROPOSE + std::to_string(my_node.getId()));
    start = std::chrono::system_clock::now();
    do {
        int ret = proposeState(my_node);
        leader = std::max(leader, ret);
        auto end = std::chrono::system_clock::now();
        diff = end - start;
    } while (diff.count() < ELECTION_TIMEOUT);

    //LEADER_ACK
    my_node.broadcast(LEADER_ACK + std::to_string(leader));
    start = std::chrono::system_clock::now();
    do {
        auto ret = ackState(my_node, leader);
        if (ret) {
            my_node.setLeaderId(leader);
        }
        end = std::chrono::system_clock::now();
        diff = end - start;

    } while (diff.count() < ELECTION_TIMEOUT);

    std::cout << my_node.getId() << "'s leader is " << my_node.getLeaderId() << std::endl;
    int wait = std::round(ELECTION_TIMEOUT);
    std::this_thread::sleep_for(std::chrono::seconds(wait));

    return 0;
}
