#include <iostream>
#include <iomanip>
using namespace std;

struct Block {
    int start;
    int size;
    bool free;
    int pid;
    Block* next;

    Block(int s, int sz, bool f, int p = -1)
        : start(s), size(sz), free(f), pid(p), next(nullptr) {}
};

class MemoryManager {
private:
    Block* head;
    int total;

    
