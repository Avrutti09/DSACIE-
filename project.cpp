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

    void mergeFreeBlocks() {
        Block* cur = head;
        while (cur != nullptr && cur->next != nullptr) {
            if (cur->free && cur->next->free) {
                Block* gone = cur->next;
                cur->size += gone->size;
                cur->next = gone->next;
                delete gone;
            } else {
                cur = cur->next;
            }
        }
    }

    int totalFree() {
        int sum = 0;
        for (Block* c = head; c != nullptr; c = c->next)
            if (c->free) sum += c->size;
        return sum;
    }

    int largestFree() {
        int best = 0;
        for (Block* c = head; c != nullptr; c = c->next)
            if (c->free && c->size > best) best = c->size;
        return best;
    }

public:
    MemoryManager(int totalSize) {
        total = totalSize;
        head = new Block(0, totalSize, true);
    }

    ~MemoryManager() {
        while (head != nullptr) {
            Block* tmp = head;
            head = head->next;
            delete tmp;
        }
    }

    void display() {
        cout << "\n  Memory Map (total = " << total << ")\n";
        cout << "  +-------+-------+-------+-----------+\n";
        cout << "  | Start |  End  | Size  |  Status   |\n";
        cout << "  +-------+-------+-------+-----------+\n";
        for (Block* c = head; c != nullptr; c = c->next) {
            cout << "  | " << setw(5) << c->start
                 << " | " << setw(5) << c->start + c->size - 1
                 << " | " << setw(5) << c->size
                 << " | ";
            if (c->free) cout << "FREE     ";
            else cout << "P" << left << setw(8) << c->pid << right;
            cout << " |\n";
        }
        cout << "  +-------+-------+-------+-----------+\n";
        cout << "  Total free: " << totalFree()
             << " | Largest free block: " << largestFree() << "\n";
    }

    bool allocate(int pid, int size) {
        cout << "\n>> Allocate P" << pid << " (size " << size << ")\n";

        if (size <= 0) {
            cout << "   ERROR: size must be positive.\n";
            return false;
        }

        for (Block* c = head; c != nullptr; c = c->next) {
            if (!c->free && c->pid == pid) {
                cout << "   ERROR: P" << pid << " already has a block.\n";
                return false;
            }
        }

        for (Block* c = head; c != nullptr; c = c->next) {
            if (c->free && c->size >= size) {
                if (c->size > size) {
                    Block* rest = new Block(c->start + size, c->size - size, true);
                    rest->next = c->next;
                    c->next = rest;
                    c->size = size;
                }
                c->free = false;
                c->pid = pid;
                cout << "   SUCCESS: allocated at address " << c->start << "\n";
                display();
                return true;
            }
        }

        if (totalFree() >= size) {
            cout << "   FAILED (EXTERNAL FRAGMENTATION): total free = " << totalFree()
                 << " >= " << size << ", but largest contiguous hole = "
                 << largestFree() << ".\n";
        } else {
            cout << "   FAILED (INSUFFICIENT MEMORY): only " << totalFree()
                 << " free, " << size << " needed.\n";
        }
        display();
        return false;
    }

    bool deallocate(int pid) {
        cout << "\n>> Deallocate P" << pid << "\n";
        for (Block* c = head; c != nullptr; c = c->next) {
            if (!c->free && c->pid == pid) {
                c->free = true;
                c->pid = -1;
                cout << "   Freed block at address " << c->start
                     << " (size " << c->size << ")\n";
                mergeFreeBlocks();
                display();
                return true;
            }
        }
        cout << "   ERROR: P" << pid << " not found in memory.\n";
        display();
        return false;
    }
};

int main() {
    MemoryManager mem(100);
    cout << "Initial state:";
    mem.display();

    mem.allocate(1, 20);
    mem.allocate(2, 30);
    mem.allocate(3, 10);
    mem.allocate(4, 25);

    mem.deallocate(1);
    mem.deallocate(3);

    mem.allocate(5, 30);

    mem.deallocate(2);
    mem.allocate(5, 30);

    mem.allocate(6, 40);
    mem.allocate(7, 200);

    return 0;
}