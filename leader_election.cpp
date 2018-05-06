#include "leader_election.h"

std::vector<std::string> mailbox;

/* Main function of the master process */
static int master(int argc, char * argv[]) {
    xbt_assert(argc == 2, "The master function expects 1 argument from the XML deployment file");
    long nodes_count = xbt_str_parse_int(argv[1], "Invalid amount of nodes: %s");

    XBT_INFO("Got %ld nodes", nodes_count);

    for (int i = 0; i < nodes_count; i++) {
        mailbox.push_back("node-" + std::to_string(i));
    }

    for (int i = 0; i < nodes_count; i ++) {
        msg_task_t task = MSG_task_create(START, 0, 0, nullptr);
        MSG_task_send(task, mailbox[i].c_str());
    }

    sleep(1);

    for (int i = 0; i < nodes_count; i++) {
        msg_task_t finalize = MSG_task_create(FINISH, 0, 0, nullptr);
        MSG_task_send(finalize, mailbox[i].c_str());
    }

    return 0;
}

void node_func(long id)
{
    Node my_node(mailbox, id);

    if (my_node.leader == NO_LEADER) {
        std::string task_name;
        std::vector<long> ret;
        long leader;

        task_name = LEADER_START;
        my_node.broadcast(task_name, 0);
        ret = my_node.recv();

        task_name = LEADER_PROPOSE;
        my_node.broadcast(task_name, id);
        ret = my_node.recv();
        ret.push_back(id);
        leader = *std::max_element(ret.begin(), ret.end());

        task_name = LEADER_ACK;
        XBT_INFO("Propose %ld", leader);
        my_node.broadcast(task_name, leader);
        ret = my_node.recv();
        

        bool ok = true;
        for (int i = 0; i < ret.size(); i ++) {
            if (ret[i] != leader) {
                ok = false;
            }
        }

        if (ok) {
            my_node.leader = leader;
            XBT_INFO("New leader is %ld", leader);
        }
    }
}

/* Main functions of the Worker processes */
static int worker(int argc, char * argv[]) {
    std::string master;
    std::string task_name;
    msg_task_t task;
    int id = xbt_str_parse_int(argv[1], "Invalid argument %s");
    int nodes_count = xbt_str_parse_int(argv[2], "Invalid argument %s");

    while (1) {
        task = nullptr;
        MSG_task_receive(&task, mailbox[id].c_str());
        task_name = MSG_task_get_name(task);

        if (task_name == START) {
            XBT_INFO("STARTED");
            MSG_task_destroy(task);
            node_func(id);
        }
        else if (task_name == FINISH) {
            XBT_INFO(FINISH);
            MSG_task_destroy(task);
            break;
        }
    }

    XBT_INFO("I'm done. See you!");
    return 0;
}

int main(int argc, char * argv[]) {
    MSG_init( & argc, argv);
    xbt_assert(argc > 2, "Usage: %s platform_file deployment_file\n"
        "\tExample: %s msg_platform.xml msg_deployment.xml\n", argv[0], argv[0]);

    MSG_create_environment(argv[1]);

    MSG_function_register("master", master);
    MSG_function_register("worker", worker);
    MSG_launch_application(argv[2]);

    msg_error_t res = MSG_main();

    XBT_INFO("Simulation time %g", MSG_get_clock());

    return res != MSG_OK;
}
