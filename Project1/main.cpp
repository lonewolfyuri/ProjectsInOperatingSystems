#include <iostream>
#include <sstream>
#include <string>
#include "manager.hpp"
using namespace std;


int main() {
    manager* testManager = nullptr;
    string rawInput;
    while(getline(cin, rawInput)) {
        if (rawInput.length() == 0 || rawInput.length() == 1 || rawInput.compare("\n") == 0 || rawInput.compare("\r\n") == 0) {
            continue;
        }
        cin.clear();
        string opType, opVal1, opVal2, result;

        /*
        if (cin.eof()) {
            cin.clear();
            break;
        }
         */

        stringstream command(rawInput);
        command >> opType;
        int opInt1, opInt2;

        if (opType == "exit") {
            exit(EXIT_SUCCESS);
        } else if (opType == "cr") {
            try {
                command >> opVal1;
                stringstream convert(opVal1);
                convert >> opInt1;
                if (opInt1 > 0 && opInt1 < 3) {
                    testManager->create(opInt1);
                } else {
                    cout << "-1 ";
                }
            } catch (...) {
                cout << "-1 ";
            }
        } else if (opType == "de") {
            try {
                command >> opVal1;
                stringstream convert(opVal1);
                convert >> opInt1;
                if (opInt1 > 0 && opInt1 < 16) {
                    if (testManager->PCB[opInt1] != nullptr) {
                        testManager->destroy(opInt1, false);
                    } else {
                        cout << "-1 ";
                    }
                } else {
                    cout << "-1 ";
                }
            } catch (...) {
                cout << "-1 ";
            }
        } else if (opType == "in") {
            if (testManager != nullptr) {
                cout << endl;
                delete testManager;
            }
            testManager = new manager();
        } else if (opType == "rl") {
            try {
                command >> opVal1;
                command >> opVal2;
                stringstream convert1(opVal1);
                stringstream convert2(opVal2);
                convert1 >> opInt1;
                convert2 >> opInt2;
                if ((opInt1 >= 0 && opInt1 < 4) && (opInt2 > 0 && opInt2 <= testManager->RCB[opInt1]->inventory)) {
                    testManager->release(opInt1, opInt2, false);
                } else {
                    cout << "-1 ";
                }
            } catch (...) {
                cout << "-1 ";
            }
        } else if (opType == "rq") {
            try {
                command >> opVal1;
                command >> opVal2;
                stringstream convert1(opVal1);
                stringstream convert2(opVal2);
                convert1 >> opInt1;
                convert2 >> opInt2;
                if ((opInt1 >= 0 && opInt1 < 4) && (opInt2 > 0 && opInt2 <= testManager->RCB[opInt1]->inventory)) {
                    testManager->request(opInt1, opInt2);
                } else {
                    cout << "-1 ";
                }
            } catch (...) {
                cout << "-1 ";
            }
        } else if (opType == "to") {
            testManager->timeout();
        } else {
            cout << "-1 ";
        }
        cin.clear();
    }
}