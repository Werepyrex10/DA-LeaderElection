#pragma once

#include <mpi.h>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <algorithm>

#define START "start"
#define FINISH "finish"

#define NO_LEADER -1

#define	LEADER_START	"a"
#define	LEADER_PROPOSE	"b"
#define	LEADER_ACK		"c"

#define TIMEOUT 50

class Node {
public:
	Node(int timeout) {
	    MPI_Init(NULL, NULL);
	    MPI_Comm_size(MPI_COMM_WORLD, &size);
	    MPI_Comm_rank(MPI_COMM_WORLD, &id);

	    this->timeout = timeout;
	}

	~Node() {
		for (int i = 0; i < sends.size(); i ++) {
			MPI_Cancel(&sends[i]);
		}
		MPI_Finalize();
	}

	void broadcast(std::string msg) {
		for (int i = 0; i < size; i ++) {
			if (i == id) {
				continue;
			}
			MPI_Request request;
			MPI_Isend(msg.c_str(), msg.size(), MPI_CHAR, i, 0, MPI_COMM_WORLD, &request);
			sends.push_back(request);
		}
	}

	std::string receive(int source, int timeout) {
		MPI_Status status;
		MPI_Request request;
		std::vector<char> msg;
		int flag = 0;
		MPI_Iprobe(source, 0, MPI_COMM_WORLD, &flag, &status);
		if (!flag) {
			std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
			MPI_Iprobe(source, 0, MPI_COMM_WORLD, &flag, &status);
		}

		if (flag) {
			msg.reserve(10);
			MPI_Irecv(msg.data(), msg.capacity(), MPI_CHAR, source, 0, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, &status);

			return msg.data();
		}

		return "";
	}

	std::vector<std::string> gather() {
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

	std::vector<std::string> gather_with_timeout() {
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

	int getId() {
		return id;
	}

private:
	int id, size, timeout;
	std::vector<MPI_Request> sends;
};