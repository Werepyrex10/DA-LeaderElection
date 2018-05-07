#include "node.hpp"

Node::Node(int timeout) : leader(NO_LEADER)
{
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    this->timeout = timeout / size;
}

Node::~Node()
{
	for (int i = 0; i < sends.size(); i ++) {
		MPI_Cancel(&sends[i]);
	}
	MPI_Finalize();
}

void Node::send(int dst, std::string msg)
{
	if (i == id) {
		return;
	}

	MPI_Request request;
	MPI_Isend(msg.c_str(), msg.size(), MPI_CHAR, dst, 0, MPI_COMM_WORLD, &request);
	sends.push_back(request);
}

std::string Node::receive(int src, int timeout)
{
	MPI_Status status;
	MPI_Request request;
	std::vector<char> msg;
	int flag = 0;
	MPI_Iprobe(src, 0, MPI_COMM_WORLD, &flag, &status);
	if (!flag) {
		std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
		MPI_Iprobe(src, 0, MPI_COMM_WORLD, &flag, &status);
	}

	if (flag) {
		msg.reserve(MSG_SIZE);
		MPI_Irecv(msg.data(), msg.capacity(), MPI_CHAR, src, 0, MPI_COMM_WORLD, &request);
		MPI_Wait(&request, &status);

		return msg.data();
	}

	return msg;
}

void Node::broadcast(std::string msg)
{
	for (int i = 0; i < size; i ++) {
		send(i, msg);
	}
}

std::vector<std::string> Node::gather()
{
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

std::vector<std::string> Node::gatherWithTimeout()
{
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

bool Node::checkLeaderStatus()
{
	if (leader == NO_LEADER) {
		return false;
	}

	send(leader, LEADER_ALIVE);

	std::string ret = receive(leader, timeout);

	return !msg.empty();
}

int Node::getId() {	return id; }

int Node::getLeaderId() { return leader; }

int Node::getState() { return state; }

void Node::setLeaderId(int leader) { this->leader = leader; }

void Node::setState(int state) { this->state = state; }
