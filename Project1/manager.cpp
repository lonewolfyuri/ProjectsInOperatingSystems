//
// Created by George Gabricht on 2019-10-14.
//

#define BLOCKED -1
#define READY 1
#define NULLS -1
#define FREE 1
#define ALLOCATED 0
#define CHILDNODE -1
#define RESNODE 0
#define WAITNODE 1
#include "manager.hpp"

// Creates new manager and initializes appropriate values.
manager::manager() : scheduled(-1), ProcSize(0) {
    for (int ndx = 0; ndx < 16; ndx++) {
        if (ndx < 4) {
            if (ndx < 3) {
                RL[ndx] = new readyList;
            }
            if (ndx == 0) {
                RCB[ndx] = new ResCB(1);
            } else {
                RCB[ndx] = new ResCB(ndx);
            }
        }
        PCB[ndx] = nullptr;
    }
    create(0);
}

manager * manager::init() {
    manager* temp = new manager();
    delete this;
    temp->scheduler();
    return temp;
}

void manager::insertIntoChildren(int toInsert, int parent) {
    if (PCB[parent]->children == nullptr) {
        PCB[parent]->children = new childNode(toInsert);
    } else {
        childNode *cur;
        for (cur = PCB[parent]->children; cur->next != nullptr; cur = cur->next) {}
        cur->next = new childNode(toInsert);
    }
}

int manager::findFreeProc() {
    for (int ndx = 0; ndx < 16; ndx++) {
        if (PCB[ndx] == nullptr) {
            return ndx;
        }
    }
    return -1;
}

// allocate new PCB[j]
// state = READY
// insert j into children of i
// parent = i
// children = NULL
// resources = NULL
// inserts j into RL,
// display: "process j created"
void manager::create(int priority) {
    ProcCB* newProc = new ProcCB;
    newProc->priority = priority;
    newProc->state = READY;
    int slot = findFreeProc();
    if (slot >= 0) {
        int parNdx = NULLS;
        if (!RL[priority]->isEmpty()) {
            parNdx = RL[priority]->Front();
        }
        if (parNdx != NULLS) {
            insertIntoChildren(slot, parNdx);
        }
        newProc->parent = parNdx;
        newProc->children = nullptr;
        newProc->resources = nullptr;
        PCB[slot] = newProc;
        RL[priority]->enq(slot);
        // cout << "process " << slot << " created" << endl;
        scheduler();
    } else {
        cout << "-1 ";
    }
}

int manager::recursiveDeleteChildren(childNode *toDelete, int count) {
    if (toDelete == nullptr) {
        return count;
    } else if (toDelete->next == nullptr){
        destroy(toDelete->value, true);
        return count + 1;
    } else {
        int result = recursiveDeleteChildren(toDelete->next, count);
        destroy(toDelete->value, true);
        return result + 1;
    }
}

void manager::removeChild(int toRemove, int parent) {
    if (PCB[parent]->children == nullptr) {
        cout << "-1 ";
    } else if (PCB[parent]->children->value == toRemove) {
        PCB[parent]->children = PCB[parent]->children->next;
    } else {
        childNode *cur, *nxt;
        for (cur = PCB[parent]->children, nxt = cur->next; nxt != nullptr && nxt->value != toRemove; cur = cur->next, nxt = cur->next) {}
        if (nxt && nxt->value == toRemove) {
            cur->next = nxt->next;
        } else {
            cout << "-1 ";
        }
    }
}

int manager::recursiveReleaseResources(resNode *toRelease) {
    if (toRelease == nullptr) {
        return 0;
    } else if (toRelease->next == nullptr){
        release(toRelease->value, toRelease->quantity, true);
        return 1;
    } else {
        int result = recursiveReleaseResources(toRelease->next);
        release(toRelease->value, toRelease->quantity, true);
        return result + 1;
    }
}

// for all k in children of j destroy(k)
// remove j from list of children of i
// remove j from RL or waiting list
// release all resources of j
// free PCB of j
// display: “n processes destroyed”
void manager::destroy(int j, bool recur) {
    if (PCB[j] != nullptr) {
        int result = recursiveDeleteChildren(PCB[j]->children, 0);
        if (PCB[j]->parent >= 0) {
            removeChild(j, PCB[j]->parent);
        }
        RL[PCB[j]->priority]->remove(j);
        recursiveReleaseResources(PCB[j]->resources);
        ProcCB *temp = PCB[j];
        PCB[j] = nullptr;
        delete temp;
        if (!recur) {
            // cout << result + 1 << " processes destroyed" << endl;
            scheduler();
        }
    } else {
        cout << "-1 ";
    }
}

void manager::insertIntoResources(int toInsert, int insertInto, int quantity) {
    if (PCB[insertInto]->resources == nullptr) {
        PCB[insertInto]->resources = new resNode(toInsert, quantity);
    } else {
        resNode *cur;
        for (cur = PCB[insertInto]->resources; cur->next != nullptr; cur = cur->next) {}
        cur->next = new resNode(toInsert, quantity);
    }
}

void manager::moveToWaitlist(int toMove, int moveTo, int request) {
    if (RCB[moveTo]->waitlist == nullptr) {
        RCB[moveTo]->waitlist = new waitNode(toMove, request);
    } else {
        waitNode *cur;
        for (cur = RCB[moveTo]->waitlist; cur->next != nullptr; cur = cur->next) {}
        cur->next = new waitNode(toMove, request);
    }
}

// if state of r is free
//      state of r = ALLOCATED
//      insert r into list of resources of proc i
//      display: “resource r allocated”
// else
//      state of i = BLOCKED
//      move i from RL to waitlist of r
//      display: “process i blocked”
//      scheduler()
void manager::request(int r, int quantity) {
    if (scheduled < 0) {
        cout << "-1 ";
        return;
    }
    if (RCB[r]->state >= quantity) {
        RCB[r]->state = RCB[r]->state - quantity;
        insertIntoResources(r, scheduled, quantity);
        // cout << "resource " << r << " allocated" << endl;
    } else {
        PCB[scheduled]->state = BLOCKED;
        RL[PCB[scheduled]->priority]->deq();
        moveToWaitlist(scheduled, r, quantity);
        // cout << "process " << scheduled << " blocked" << endl;
    }
    scheduler();
}

int manager::releaseResource(int toRelease, int proc, int quantity) {
    int result = -1;
    if (PCB[proc]->resources == nullptr) {
        return result;
    } else if (PCB[proc]->resources->value == toRelease) {
        resNode *temp = PCB[proc]->resources;
        result = PCB[proc]->resources->quantity;
        PCB[proc]->resources = PCB[proc]->resources->next;
        delete temp;
    } else {
        resNode *cur, *nxt;
        for (cur = PCB[proc]->resources, nxt = cur->next; nxt != nullptr && nxt->value != toRelease; cur = cur->next, nxt = cur->next) {}
        if (nxt && nxt->value == toRelease) {
            result = nxt->quantity;
            cur->next = nxt->next;
            delete nxt;
        }
    }
    return result;
}

int manager::removeFromWaitlist(int resource) {
    if (RCB[resource]->waitlist == nullptr) {
        cout << "-1 ";
        return -1;
    } else {
        int temp = RCB[resource]->waitlist->value;
        RCB[resource]->waitlist = RCB[resource]->waitlist->next;
        return temp;
    }
}

int manager::moveToReadylist(int moveFrom) {
    int result = removeFromWaitlist(moveFrom);
    if (result > 0) {
        RL[PCB[result]->priority]->enq(result);
    }
    return result;
}

// remove r from resources list of process i
// if waitlist of r is empty
//      state of r = FREE
// else
//      move process j from head of waitlist of r to RL
//      state of j = READY
//      insert r into resources list of process j
// display: "resource r released"
void manager::release(int r, int quantity, bool recur) {
    if (scheduled < 0 && !recur) {
        cout << "-1 ";
        return;
    }
    int quan = quantity;
    if (RCB[r]->inventory - RCB[r]->state >= quantity) {
        bool isChange = true;
        while (isChange) {
            int freed = releaseResource(r, scheduled, quan);
            quan -= freed;
            if (RCB[r]->waitlist == nullptr) {
                isChange = false;
                RCB[r]->state += freed;
            } else if (freed >= RCB[r]->waitlist->requested){
                int req = RCB[r]->waitlist->requested;
                int result = moveToReadylist(r);
                if (result > 0) {
                    PCB[result]->state = READY;
                    insertIntoResources(r, result, req);
                }
            } else {
                isChange = false;
            }
        }
        // cout << "resource " << r << " released" << endl;
        if (!recur) {
            scheduler();
        }
    } else if (!recur) {
        cout << "-1 ";
    }
}

// move process i from head of RL to end of RL
// scheduler()
void manager::timeout() {
    if (scheduled < 0) {
        cout << "-1 ";
        return;
    }
    RL[PCB[scheduled]->priority]->deq();
    RL[PCB[scheduled]->priority]->enq(scheduled);
    scheduler();
}

int manager::findScheduledProc() {
    for (int ndx = 2; ndx >= 0; ndx--) {
        if (!RL[ndx]->isEmpty()) {
            return RL[ndx]->Front();
        }
    }
    return -1;
}

// find process i currently at the head of RL
// display: "process i running"
void manager::scheduler() {
    scheduled = findScheduledProc();
    if (scheduled >= 0) {
        cout << scheduled << " ";
        // cout << "process " << scheduled << " running" << endl;
    }
}

void manager::recursiveDeleteList(void *node, int type) {
    if (node == nullptr) {
    } else if (type == CHILDNODE) {
        childNode *cn = (childNode *) node;
        if (cn->next == nullptr) {
            delete cn;
        } else {
            recursiveDeleteList(cn->next, CHILDNODE);
            delete cn;
        }
    } else if (type == RESNODE) {
        childNode *cn = (childNode *) node;
        if (cn->next == nullptr) {
            delete cn;
        } else {
            recursiveDeleteList(cn->next, RESNODE);
            delete cn;
        }
    } else {
        childNode *cn = (childNode *) node;
        if (cn->next == nullptr) {
            delete cn;
        } else {
            recursiveDeleteList(cn->next, WAITNODE);
            delete cn;
        }
    }
}

// destructor
manager::~manager() {
    for (int ndx = 0; ndx < 3; ndx++) {
        delete RL[ndx];
    }
    for (int ndx = 0; ndx < 4; ndx++) {
        recursiveDeleteList(RCB[ndx]->waitlist, WAITNODE);
        delete RCB[ndx];
    }
    for (int ndx = 0; ndx < 16; ndx++) {
        if (PCB[ndx] != nullptr) {
            recursiveDeleteList(PCB[ndx]->children, CHILDNODE);
            recursiveDeleteList(PCB[ndx]->resources, RESNODE);
            delete PCB[ndx];
        }
    }
}