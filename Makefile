CXX=g++
CXXFLAGS=--std=c++11
CINCLUDE=-I/home/dimi/Desktop/app-masterworker/simgrid/include
LDFLAGS=-lsimgrid

build: app-masterworker

app-masterworker: app-masterworker.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(CINCLUDE) $(LDFLAGS)

app-masterworker.o: app-masterworker.c
	$(CXX) -c $(CXXFLAGS) $(CINCLUDE) $^ -o $@

run: build
	./app-masterworker /home/dimi/Desktop/app-masterworker/simgrid/examples/platforms/small_platform.xml app-masterworker_d.xml

clean:
	rm -rf app-masterworker app-masterworker.o
