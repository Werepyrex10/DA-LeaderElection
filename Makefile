CXX=mpic++
CXXFLAGS=--std=c++11 -g
CINCLUDE=
LDFLAGS=

build: leader_election

leader_election: leader_election.o node.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(CINCLUDE) $(LDFLAGS)

leader_election.o: leader_election.cpp leader_election.hpp
	$(CXX) -c $(CXXFLAGS) $(CINCLUDE) $^ -o $@

node.o: node.cpp node.hpp
	$(CXX) -c $(CXXFLAGS) $(CINCLUDE) $^ -o $@

run: build
	mpirun -np 5 ./leader_election

clean:
	rm -rf leader_election leader_election.o
