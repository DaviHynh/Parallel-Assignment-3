// David Huynh
// COP 4520, Spring 2024

#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <shared_mutex>
#include <random>
#include <numeric>
#include <algorithm>

// Number of presents.
#define NUMP 500000

// Linked list node, where every node has a mutex.
struct Node
{
    std::shared_mutex mtx;
    int val;
    Node *next;
    Node(int n) : val(n), next(nullptr) {}
};

class PresentRoom
{
private:
    std::shared_mutex mutex_;
    Node *head;
    Node *tail;
    std::vector<int> presentBag;
    int cardCount;

public:
    // Default constructor that creates an empty head/tail for the linked list and generates the shuffled present bag.
    PresentRoom() : presentBag(NUMP), cardCount(0)
    {
        head = new Node(-1);
        tail = new Node(500001);
        head->next = tail;
        std::iota (std::begin(presentBag), std::end(presentBag), 1);

        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        shuffle(presentBag.begin(), presentBag.end(), std::default_random_engine(seed));
    }

    void addPresent();
    void removePresent();
    void findPresent();
    int getCardCount();
};

// Adds a present to the chain of presents.
void PresentRoom::addPresent()
{
    int num = -1;

    // Grabs a random present from the bag to add.
    std::unique_lock lock(mutex_);
    
    if (!presentBag.empty())
    {
        num = presentBag.front();
        presentBag.erase(presentBag.begin());
    }
    else
    {
        return;
    }

    lock.unlock();

    // Locks two nodes while traversing (hand-over-hand locking).
    head->mtx.lock();
    Node *prev = head;
    Node *curr = head->next;
    curr->mtx.lock();

    while (num > curr->val)
    {
        prev->mtx.unlock();
        prev = curr;
        curr = curr->next;
        curr->mtx.lock();
    }

    // Create a new node to insert and unlock nodes.
    Node *newNode = new Node(num);
    newNode->next = curr;
    prev->next = newNode;

    curr->mtx.unlock();
    prev->mtx.unlock();
}

// Removes a present from the chain of presents.
void PresentRoom::removePresent()
{
    // Lock the head and the first node.
    head->mtx.lock();
    Node *curr = head->next;
    curr->mtx.lock();

    // Check that the node to delete isn't tail.
    if (curr == tail)
    {
        curr->mtx.unlock();
        head->mtx.unlock();
        return;
    }

    // Relink nodes and delete the node.
    head->next = curr->next;
    curr->mtx.unlock();
    head->mtx.unlock();

    delete(curr);

    // Increment the number of cards written.
    std::unique_lock lock(mutex_);
    cardCount++;

    if (cardCount % 100000 == 0)
    {
        std::cout << "Number of cards written: " << cardCount << "\n";
    }
}

// Searches the list for a present.
void PresentRoom::findPresent()
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(1, NUMP);
    int num = dist(mt);
    
    // Traverses the list to find a node with a specific value.
    Node *curr = head->next;

    while (curr != tail)
    {
        if (num == curr->val)
        {
            std::cout << "Searched for and found present #" << num << ".\n";
            return;
        }
        curr = curr->next;
    }
}

// Returns the total number of cards written.
int PresentRoom::getCardCount()
{
    std::shared_lock lock(mutex_);
    return cardCount;
}

// Multiple servants alternate adding and removing presents.
void performTasks(PresentRoom &room)
{
    int flag = true;
    int calledToCheck = 0;

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(1, 4);

    // Loop until the card count matches present count.
    // Each servant alternates between adding a present and removing a present.
    // The minotaur also randomly asks a servant to check if a present is in the chain.
    while (room.getCardCount() < NUMP)
    {
        calledToCheck = dist(mt);

        if (flag)
        {
            room.addPresent();
            flag = false;
        }
        else
        {
            room.removePresent();
            flag = true;
        }
        
        if (calledToCheck == 1)
        {
            room.findPresent();
        }
    }
}

int main(void)
{
    PresentRoom myRoom;

    std::cout << "The card writing process has begun! (This may take a while...)\n";

    std::thread servant1(performTasks, std::ref(myRoom));
    std::thread servant2(performTasks, std::ref(myRoom));
    std::thread servant3(performTasks, std::ref(myRoom));
    std::thread servant4(performTasks, std::ref(myRoom));

    servant1.join();
    servant2.join();
    servant3.join();
    servant4.join();

    std::cout << "The servants have finished writing cards for each present!\n";

    return 0;
}
