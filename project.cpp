#include <iostream>
using namespace std;

struct Block {
    int start;
    int size;
    bool free;
    int pid;
    Block* next;
    Block* prev;

    Block(int s, int sz, bool f, int p = -1)
        : start(s), size(sz), free(f), pid(p), next(nullptr), prev(nullptr) {}
};

class MemoryManager {
private:
    Block* head;
    int total;

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

    int totalFree()
    {
        int sum = 0;
        Block* temp = head;
        while (temp != nullptr)
        {
            if (temp->free)
                sum += temp->size;
            temp = temp->next;
        }
        return sum;
    }

    int largestFree()
    {
        int largest = 0;
        Block* temp = head;
        while (temp != nullptr)
        {
            if (temp->free && temp->size > largest)
                largest = temp->size;
            temp = temp->next;
        }
        return largest;
    }

public:
    MemoryManager(int totalSize)
    {
        total = totalSize;
        head = new Block(0, totalSize, true);
    }

    ~MemoryManager()
    {
        while (head != nullptr)
        {
            Block* tmp = head;
            head = head->next;
            delete tmp;
        }
    }
void display()
{
    cout << "\n  Memory Map (total = " << total << ")\n";
    cout << "  +-----------------------------------+\n";
    cout << "  | Start |  End  | Size  | Status    |\n";
    cout << "  +-----------------------------------+\n";

    Block* temp = head;

    while (temp != nullptr)
    {
        cout << "  | " << temp->start
             << " | " << temp->start + temp->size - 1
             << " | " << temp->size << " | ";

        if (temp->free)
            cout << "FREE";
        else
            cout << "P" << temp->pid;

        cout << "\n";
        temp = temp->next;
    }

    cout << "  +-----------------------------------+\n";

    cout << "  Total free: " << totalFree()
         << " | Largest free block: " << largestFree() << "\n";
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

    bool deallocate(int pid)
    {
        cout << "\n>> Deallocate P" << pid << "\n";

        Block* b = head;
        while (b != nullptr)
        {
            if (!b->free && b->pid == pid) break;
            b = b->next;
        }

        if (b == nullptr)
        {
            cout << "   ERROR: P" << pid << " not found in memory.\n";
            display();
            return false;
        }

        cout << "   Freed block at address " << b->start
             << " (size " << b->size << ")\n";

        b->free = true;
        b->pid = -1;
        merge(b);
        display();
        return true;
    }
};

bool readInt(int& x)
{
    cin >> x;
    if (cin.fail())
    {
        cin.clear();
        cin.ignore(1000, '\n');
        return false;
    }
    return true;
}

int main()
{
    int totalSize;
    cout << "Enter total memory size: ";
    while (!readInt(totalSize) || totalSize <= 0)
    {
        cout << "Invalid size. Enter a positive number: ";
    }

    MemoryManager mem(totalSize);
    mem.display();

    int choice;
    do
    {
        cout << "\n===== MENU =====\n";
        cout << "1. Allocate\n";
        cout << "2. Deallocate\n";
        cout << "3. Display memory map\n";
        cout << "4. Exit\n";
        cout << "Enter choice: ";

        if (!readInt(choice))
        {
            cout << "Invalid input. Enter a number from 1 to 4.\n";
            choice = 0;
            continue;
        }

        switch (choice)
        {
        case 1:
        {
            int pid, size;
            cout << "Enter process id: ";
            if (!readInt(pid)) { cout << "Invalid process id.\n"; break; }
            cout << "Enter size: ";
            if (!readInt(size)) { cout << "Invalid size.\n"; break; }
            mem.allocate(pid, size);
            break;
        }
        case 2:
        {
            int pid;
            cout << "Enter process id to free: ";
            if (!readInt(pid)) { cout << "Invalid process id.\n"; break; }
            mem.deallocate(pid);
            break;
        }
        case 3:
            mem.display();
            break;
        case 4:
            cout << "Exiting simulator.\n";
            break;
        default:
            cout << "Invalid choice. Enter a number from 1 to 4.\n";
        }
    } while (choice != 4);

    return 0;
}
