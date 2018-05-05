CXX=g++
CXXFLAGS=--std=c++11
CINCLUDE=-I./simgrid/include
LDFLAGS=-lsimgrid

build: app-masterworker

app-masterworker: app-masterworker.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(CINCLUDE) $(LDFLAGS)

app-masterworker.o: app-masterworker.c
	$(CXX) -c $(CXXFLAGS) $(CINCLUDE) $^ -o $@

test: build
	./app-masterworker small_platform.xml app-masterworker_d.xml

run: build
	./app-masterworker platform.xml deployment.xml

clean:
	rm -rf app-masterworker app-masterworker.o
