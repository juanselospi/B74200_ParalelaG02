#include <condition_variable>
#include <mutex>

class DiningPh {

public:
    DiningPh();
    ~DiningPh();
    void pickup( long who );
    void putdown( long who );
    void test( long who );
    void print();

private:
    enum { Thinking, Hungry, Eating } state[ 5 ];
    std::mutex dp;			// Locks for each method in class
    std::condition_variable self[ 5 ];

};

