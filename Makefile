CXX=g++
CXXFLAGS=--std=c++11 -g
CINCLUDE=-I./simgrid/include
LDFLAGS=-lsimgrid -lpthread

build: leader_election

leader_election: leader_election.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(CINCLUDE) $(LDFLAGS)

leader_election.o: leader_election.cpp
	$(CXX) -c $(CXXFLAGS) $(CINCLUDE) $^ -o $@

run: build
	./leader_election platform.xml deployment.xml

clean:
	rm -rf leader_election leader_election.o
