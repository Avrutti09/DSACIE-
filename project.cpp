#include <iostream>
#include <iomanip>
using namespace std;

struct Block {
    int start;
    int size;
    bool free;
    int pid;
    Block* next;
    Block*prev;

    Block(int s, int sz, bool f, int p = -1)
        : start(s), size(sz), free(f), pid(p), next(nullptr),prev(nullptr) {}
};

class MemoryManager {
private:
    Block* head;
    int total;
public:MemoryManager(int Totalsize)
{
    total=Totalsize;
head=new Block(0,Totalsize,true);
}
void merge(Block* b)
{
    if (b->next != nullptr && b->next->free)
    {
        Block* r = b->next;
        b->size += r->size;
        b->next = r->next;
        if (r->next != nullptr) r->next->prev = b;
        delete r;
    }
    if (b->prev != nullptr && b->prev->free)
    {
        Block* l = b->prev;
        l->size += b->size;
        l->next = b->next;
        if (b->next != nullptr) b->next->prev = l;
        delete b;
    }
}

bool allocate(int pid, int size)
{
    cout << "\n>> Allocate P" << pid << " (size " << size << ")\n";

    if (size <= 0)
    {
        cout << "   ERROR: size must be positive.\n";
        return false;
    }

    for (Block* c = head; c != nullptr; c = c->next)
    {
        if (!c->free && c->pid == pid)
        {
            cout << "   ERROR: P" << pid << " already has a block.\n";
            return false;
        }
    }

    Block* temp = head;
    while (temp != nullptr)
    {
        if (temp->free && temp->size >= size)
        {
            if (temp->size > size)
            {
                Block* rest = new Block(temp->start + size, temp->size - size, true);
                rest->next = temp->next;
                rest->prev = temp;
                if (rest->next != nullptr) rest->next->prev = rest;
                temp->next = rest;
                temp->size = size;
            }
            temp->free = false;
            temp->pid = pid;
            cout << "   SUCCESS: allocated at address " << temp->start << "\n";
            display();
            return true;
        }
        temp = temp->next;
    }

    if (totalFree() >= size)
    {
        cout << "   FAILED (EXTERNAL FRAGMENTATION): total free = " << totalFree()
             << " >= " << size << ", but largest contiguous hole = "
             << largestFree() << ".\n";
    }
    else
    {
        cout << "   FAILED (INSUFFICIENT MEMORY): only " << totalFree()
             << " free, " << size << " needed.\n";
    }
    display();
    return false;
}
    
};
