//
// Created by George Gabricht on 2019-10-14.
//
#include <deque>
#include <string>
#include <iostream>
using namespace std;
#define BLOCKED -1
#define READY 1
#define NULLS -1
#define FREE 1
#define ALLOCATED 0

#ifndef PROJECT1_MANAGER_HPP
#define PROJECT1_MANAGER_HPP


class manager {
public:
    struct readyList {
        struct readyNode {
            int value;
            readyNode *next;

            readyNode(int value, readyNode *node = nullptr) : value(value), next(node) {}
        };

        readyNode *head;

        readyList(readyNode *head = nullptr) : head(head) {}

        void enq(int newVal) {
            if (head == nullptr) {
                head = new readyNode(newVal);
            } else {
                readyNode *cur;
                for (cur = head; cur->next != nullptr; cur = cur->next) {}
                cur->next = new readyNode(newVal);
            }
        }

        int deq() {
            if (isEmpty()) {
                return -1;
            }
            int tempVal = head->value;
            readyNode *temp = head;
            head = head->next;
            delete temp;
            return tempVal;
        }

        void remove(int val) {
            if (isEmpty()) {
                return;
            }
            readyNode *cur, *nxt;
            if (head == nullptr) {
                return;
            }
            for (cur = head, nxt = head->next; nxt != nullptr && nxt->value != val; cur = cur->next, nxt = nxt->next) {}
            if (nxt && nxt->value == val) {
                cur->next = nxt->next;
                delete nxt;
            }
        }

        int Front() {
            if (head != nullptr) {
                return head->value;
            } else {
                return -1;
            }
        }

        bool isEmpty() {
            return head == nullptr;
        }

        bool isFull() {
            return false;
        }

        void recursiveDeleteNodes(readyNode *node) {
            if (node == nullptr) {
                return;
            } else if (node->next == nullptr) {
                delete node;
            } else {
                recursiveDeleteNodes(node->next);
                delete node;
            }
        }

        ~readyList() {
            recursiveDeleteNodes(head);
        }
    };

    struct childNode {
        int value;
        childNode *next;

        childNode(int value, childNode* next = nullptr) : value(value), next(next) {}
    };

    struct resNode {
        int value, quantity;
        resNode *next;

        resNode (int value, int quantity, resNode* next = nullptr) : value(value), quantity(quantity), next(next) {}
    };

    struct waitNode {
        int value, requested;
        waitNode *next;

        waitNode(int value, int requested, waitNode* next = nullptr) : value(value), requested(requested), next(next) {}
    };

    struct ProcCB {
        int priority, state, parent;
        childNode *children;
        resNode *resources;
    };

    struct ResCB {
        int inventory, state;
        waitNode *waitlist;

        ResCB(int inventory) : inventory(inventory), state(inventory), waitlist(nullptr) {}
    };

    int scheduled, ProcSize;
    readyList* RL[3];
    ProcCB* PCB[16];
    ResCB* RCB[4];

    // These methods manipulate PCBs and RCBs in various ways
    int findFreeProc();
    void insertIntoChildren(int toInsert, int parent);
    int recursiveDeleteChildren(childNode *toDelete, int count);
    void removeChild(int toRemove, int parent);
    int recursiveReleaseResources(resNode *toRelease);
    void insertIntoResources(int toInsert, int insertInto, int quantity);
    void moveToWaitlist(int toMove, int moveTo, int request);
    int releaseResource(int toRelease, int proc, int quantity);
    int removeFromWaitlist(int resource);
    int moveToReadylist(int moveFrom);
    int findScheduledProc();

    // Creates new manager and initializes appropriate values.
    manager();

    // restores object to initial values.
    manager * init();

    // allocate new PCB[j]
    // state = READY
    // insert j into children of i
    // parent = i
    // children = NULL
    // resources = NULL
    // inserts j into RL,
    // display: "process j created"
    void create(int priority);

    // for all k in children of j destroy(k)
    // remove j from list of children of i
    // remove j from RL or waiting list
    // release all resources of j
    // free PCB of j
    // display: “n processes destroyed”
    void destroy(int j, bool recur);

    // if state of r is free
    //      state of r = ALLOCATED
    //      insert r into list of resources of proc i
    //      display: “resource r allocated”
    // else
    //      state of i = BLOCKED
    //      move i from RL to waitlist of r
    //      display: “process i blocked”
    //      scheduler()
    void request(int r, int quantity);

    // remove r from resources list of process i
    // if waitlist of r is empty
    //      state of r = FREE
    // else
    //      move process j from head of waitlist of r to RL
    //      state of j = READY
    //      insert r into resources list of process j
    // display: "resource r released"
    void release(int r, int quantity, bool recur);

    // move process i from head of RL to end of RL
    // scheduler()
    void timeout();

    // find process i currently at the head of RL
    // display: "process i running"
    void scheduler();

    void recursiveDeleteList(void *node, int type);

    // destructor
    ~manager();

};

#endif //PROJECT1_MANAGER_HPP
