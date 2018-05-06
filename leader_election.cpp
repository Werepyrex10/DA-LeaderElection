#include "leader_election.h"

int main(int argc, char** argv) {
    Node my_node(50);

    my_node.broadcast("Hello");
    auto ret = my_node.gather_with_timeout();

    std:: cout << my_node.getId() << " " << ret.size() << std::endl;

    return 0;
}