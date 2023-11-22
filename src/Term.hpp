#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <set>
using namespace std;

class Term {
private:
    set<string> literals;
    string expr;
public:
    Term() {}
    void insert(string name) {
        if(!name.empty() && literals.find(name) == literals.end()) {
            literals.insert(name);
        }
    }
    string getExpr() const {
        string expr;
        for(auto& literal : literals) {
            expr += literal;
        } 
        return expr;
    }
    set<string> getLiterals() const {
        return literals;
    }
    void printExpr() const {
        string expr;
        for(auto& literal : literals) {
            expr += literal;
        }
        cout << expr;
    }    
};