BIN = bin
CXX = g++
CXXFLAGS = -g -fopenmp

all: $(BIN)/Potato.out

$(BIN)/Lock.o: Lock.h Lock.cc
	$(CXX) $(CXXFLAGS) -c Lock.cc -o $@

$(BIN)/Semaphore.o: Semaphore.h Semaphore.cc
	$(CXX) $(CXXFLAGS) -c Semaphore.cc -o $@

$(BIN)/Condition.o: Condition.h Condition.cc
	$(CXX) $(CXXFLAGS) -c Condition.cc -o $@

$(BIN)/Potato.o: Potato.cc
	$(CXX) $(CXXFLAGS) -c Potato.cc -o $@

$(BIN)/Potato.out: $(BIN)/Potato.o $(BIN)/Semaphore.o $(BIN)/Lock.o
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BIN)/omp_sem_test.o: omp_sem_test.cc
	$(CXX) $(CXXFLAGS) -c omp_sem_test.cc -o $@

$(BIN)/omp_sem_test.out: $(BIN)/omp_sem_test.o $(BIN)/Semaphore.o
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BIN)/Mesa.o: Mesa.h Mesa.cc
	$(CXX) $(CXXFLAGS) -c Mesa.cc -o $@

$(BIN)/Filosofos.o: Filosofos.cc
	$(CXX) $(CXXFLAGS) -c Filosofos.cc -o $@

$(BIN)/Filosofos.out: $(BIN)/Filosofos.o $(BIN)/Mesa.o $(BIN)/Lock.o $(BIN)/Condition.o
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	@rm -f $(BIN)/*.o $(BIN)/*.out
