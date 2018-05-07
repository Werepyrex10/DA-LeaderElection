#include "leader_election.hpp"

int main(int argc, char** argv) {
    Node my_node(TIMEOUT);

    while(true) {
        std::vector<std::string> results;
        std::vector<int> proposals;
        int counter;
        int leader;
        std::this_thread::sleep_for(std::chrono::milliseconds(ELECTION_TIMEOUT));

        if (my_node.checkLeaderStatus()) {
            continue;
        }

        //Elect new leader
        //LEADER_START
        results.clear();
        counter = 0;
        my_node.broadcast(LEADER_START);
        results = my_node.gatherWithTimeout();

        for (int i = 0; i < results.size(); i ++) {
            if (results[i] == LEADER_START) {
                counter++;
            }
        }

        if (counter == 0) {
            continue;
        }

        my_node.broadcast(LEADER_PROPOSE + std::to_string(my_node.getId()));
        results = gatherWithTimeout();
        results.push_back(std::to_string(my_node.getId()));

        leader = std::stoi(std::max_element(results.begin(), results.end(),
            [](std::string left, std::string right) -> bool {
                if (left[0] == LEADER_PROPOSE && right[0] == LEADER_PROPOSE) {
                    return left[1] < right[1];
                }
                else if (left[0] == LEADER_PROPOSE) {
                    return false;
                }
                else
                    return true;
            }
        ));

        my_node.broadcast(LEADER_ACK + std::to_string(leader));
        results = my_node.gatherWithTimeout();
        for (int i = 0; i < results.size(); i++) {
            if (results[i][1] == std::to_string(leader)) {
                my_node.setLeader(leader);
            }
        }
    }

    my_node.broadcast("Hello");
    auto ret = my_node.gatherWithTimeout();

    std:: cout << my_node.getId() << " " << ret.size() << std::endl;

    return 0;
}
