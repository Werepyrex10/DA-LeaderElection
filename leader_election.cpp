#include "leader_election.hpp"

int main(int argc, char** argv) {
    Node my_node(TIMEOUT);

    while(true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(ELECTION_TIMEOUT));

        if (my_node.checkLeaderStatus()) {
            continue;
        }

        //Elect new leader
    }

    my_node.broadcast("Hello");
    auto ret = my_node.gatherWithTimeout();

    std:: cout << my_node.getId() << " " << ret.size() << std::endl;

    return 0;
}
