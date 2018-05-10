#include "leader_election.hpp"

static std::string LEADER_IDLE("a");
static std::string LEADER_START("b");
static std::string LEADER_PROPOSE("c");
static std::string LEADER_ACK("d");
static std::string LEADER_ALIVE("e");

bool running = true;

void int_handler(int x)
{
    running = false;
}

std::vector<std::string> splitStringByDelim(std::string str, std::string delim)
{
    std::vector<std::string> ret;
    std::stringstream ss(str);
    std::string item;

    while (std::getline(ss, item, delim[0])) {
        ret.push_back(item);
    }

    return ret;
}

int startState(Node& my_node, std::vector<std::string>& msg)
{

}

int proposeState(Node& my_node, std::vector<std::string>& msg)
{
    msg.push_back(std::to_string(my_node.getEpoch())+ "-" + LEADER_PROPOSE + "-" + std::to_string(my_node.getId()));

    for (int i = 0; i < msg.size(); i ++) {
        auto parts = splitStringByDelim(msg[i], "-");
        int num = parts[2][0] - '0';

        if (leader <= num) {
            leader = num;
        }
    }

    return leader;
}

int ackState(Node& my_node, std::vector<std::string>& msg)
{
    for (int i = 0; i < msg.size(); i++) {
        auto parts = splitStringByDelim(msg[i], "-");
        int ret = checkMessage(my_node, parts);

        if (parts[2][0] != std::to_string(my_node.getLeaderId())[0]) {
            return false;
        }
    }

    return true;
}

void checkMessages(Node& my_node, std::vector<std::string>& msg)
{
    for (int i = 0; i < 2; i++) {
        for (auto it = msg.begin(); it != msg.end(); it++) {
            auto parts = splitStringByDelim(*it, "-");
            int msg_epoch = std::stoi(parts[0]);

            if (msg_epoch < my_node.getEpoch()) {
                msg.erase(it);
                continue;
            }
            else if (msg_epoch > my_node.getEpoch()) {
                my_node.setEpoch(msg_epoch);
                my_node.setState(parts[1]);
                continue;
            }
            else {
                if (parts[1] < my_node.getState()) {
                    msg.erase(it);
                    continue;
                }
                else if (parts[1] > my_node.getState()){
                    my_node.setState(parts[1]);
                }
            }
        }
    }
}

void processByState(Node& my_node, std::vector<std::string>& msg)
{
    switch(my_node.getState[0])
    {
        case LEADER_START[0]:
        auto ret = startState(my_node, msg);
        break;

        case LEADER_PROPOSE[0]:
        auto ret = proposeState(my_node, msg);
        my_node.setLeaderId(std::max(my_node.getLeaderId(), ret));
        break;

        case LEADER_ACK[0]:
        auto ret = ackState(my_node, msg);
        break;
    }
}

int main(int argc, char** argv) {
    Node my_node(TIMEOUT);
    auto start;
    auto end = start;
    std::chrono::duration<double> diff;
    int wait = std::round(ELECTION_TIMEOUT);

    signal(SIGINT, int_handler);

    do {
        switch(my_node.getState[0])
        {
            case LEADER_IDLE[0]:
            //LEADER_START
            my_node.setState(LEADER_START);
            my_node.broadcast(std::to_string(epoch) + "-" + LEADER_START + "-0");
            break;

            case LEADER_START[0]:
            //LEADER_PROPOSE
            my_node.setLeaderId(NO_LEADER);
            my_node.setState(LEADER_PROPOSE);
            my_node.broadcast(std::to_string(epoch) + "-" + LEADER_PROPOSE + "-" + std::to_string(my_node.getId()));
            break;

            case LEADER_PROPOSE[0]:
            //LEADER_ACK
            my_node.setState(LEADER_ACK);
            my_node.broadcast(std::to_string(epoch) + "-" + LEADER_ACK + std::to_string(my_node.getId()));

            case LEADER_ACK[0]:
            //LEADER_IDLE
            my_node.setState(LEADER_IDLE);
            std::cout << my_node.getId() << "'s leader is " << my_node.getLeaderId() << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(wait));
            my_node.advanceEpoch();
            break;

            default:
            return 0;
        }

        start = std::chrono::system_clock::now();
        do {
            auto msg = my_node.gatherWithTimeout();
            checkMessages(my_node, msg);
            processByState(my_node, msg);
            end = std::chrono::system_clock::now();
            diff = end - start;
        } while (diff.count() < ELECTION_TIMEOUT);


    } while(running);
    return 0;
}
