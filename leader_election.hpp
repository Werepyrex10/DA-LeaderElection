#pragma once

#include <iostream>
#include <chrono>
#include <sstream>
#include <cmath>
#include <csignal>
#include "node.hpp"

#define NO_LEADER       	-1
#define TIMEOUT             1000 //milliseconds
#define ELECTION_TIMEOUT    3.0 //seconds