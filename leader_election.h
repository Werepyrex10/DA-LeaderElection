#pragma once

#include "simgrid/msg.h"

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <algorithm>
#include <atomic>

#define START "start"
#define FINISH "finish"

#define NO_LEADER -1

#define LEADER_IDLE		"a"
#define	LEADER_START	"b"
#define	LEADER_PROPOSE	"c"
#define	LEADER_ACK		"d"

XBT_LOG_NEW_DEFAULT_CATEGORY(msg_app_masterworker, "Messages specific for this msg example");

class Node {
public:
	Node(std::vector<std::string> neighbors, int id) {
		mailbox = neighbors[id];
		this->neighbors = neighbors;
		this->id = id;
		leader = NO_LEADER;
	}

	void broadcast(std::string task_name, int value) {
		msg_task_t task;
		for (int i = 0; i < neighbors.size(); i ++) {
			task = MSG_task_create(task_name.c_str(), 0, 0, (void *)(long)value);
			MSG_task_isend(task, neighbors[i].c_str());
		}
	}

	std::vector<long> recv() {
		std::vector<long> ret;
		msg_task_t task;
		msg_comm_t irecv;

		for (int i = 0; i < neighbors.size(); i ++) {
			task = nullptr;
			irecv = MSG_task_irecv(&task, neighbors[id].c_str());
			MSG_comm_wait(irecv, -1);
			MSG_comm_destroy(irecv);

			long proposed = (long)MSG_task_get_data(task);
			MSG_task_destroy(task);
			ret.push_back(proposed);
		}

		return ret;
	}

	std::vector<std::string> neighbors;
	std::string mailbox;
	int leader;
	int id;
};
