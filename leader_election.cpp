#include "leader_election.hpp"

int main(int argc, char** argv) {
    Node my_node(TIMEOUT);

    while(true) {
        std::vector<std::string> results;
        std::vector<int> old_proposals, proposals;
        int counter = 0, leader = NO_LEADER;
        bool change = true;
        std::this_thread::sleep_for(std::chrono::milliseconds(ELECTION_TIMEOUT));

        if (my_node.checkLeaderStatus()) {
            continue;
        }

        //LEADER_START
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

        //LEADER_PROPOSE
        do {
            my_node.broadcast(LEADER_PROPOSE + std::to_string(my_node.getId()));
            results = gatherWithTimeout();
            results.push_back(std::to_string(my_node.getId()));

            leader = std::max (leader, std::stoi(std::max_element(results.begin(), results.end(),
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
            )));
            
            change = proposals.size() != 0 ? true : old_proposals.size() != proposals.size();

        } while(change);

        //LEADER_ACK
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
