#pragma once

#include <vector>
#include <string>
#include "Function.hpp"

using namespace std;

struct GraphInfo {
    vector<Function> sets;
    vector<string> input;
    vector<string> output;
public:
    GraphInfo() {}
    void insertPI(string name) {
        input.push_back(name);
    }
    void insertPO(string name) {
        output.push_back(name);
    }
    void insertFunction(Function function) {
        sets.push_back(function);
    }
    void printFunctionSets() {
        for(auto &func : sets) {
            func.printExpr();
        }
    }
};