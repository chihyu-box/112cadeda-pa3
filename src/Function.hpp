#pragma once

#include "Term.hpp"
#include <vector>
#include <iostream>
#include <string>
#include <set>
#include <unordered_map>
using namespace std;

class Function {
private:
    vector<Term> terms;
    set<string> terms_name;
    set<string> literals;
    set<string> cokernels;
    unordered_map<string, vector<string>> kernels; // cokernel_name, kernel_name
    unordered_map<string, set<string>> kernels_literal_set; // kernel_name kernel_literal
    unordered_map<string, set<string>> cokernels_literal_set; // cokernel_name cokernel_literal
    string name;
public:
    void insertTerm(Term term) {
        if(term.getLiterals().size() != 0 && terms_name.find(term.getExpr()) == terms_name.end()) {
            terms_name.insert(term.getExpr());
            terms.push_back(term);
            insertLiterals(term);
        }
    }
    void setName(string fname) {
        name = fname;
    }
    void setCokernels(set<string>&& ck) {
        cokernels = move(ck);
    }
    void setkernels(unordered_map<string, vector<string>>&& ker) {
        kernels = move(ker);
    }
    void setKernelsLiteralSet(unordered_map<string, set<string>>&& kls) {
        kernels_literal_set = move(kls);
    }
    void setCokernelsLiteralSet(unordered_map<string, set<string>>&& cls) {
        cokernels_literal_set = move(cls);
    }
    set<string> getLiterals() const {
        return literals;
    }
    vector<Term> getTerms() const {
        return terms;
    }
    set<string> getCokernels() const {
        return cokernels;
    }
    unordered_map<string, vector<string>> getKernels() const {
        return kernels;
    }
    unordered_map<string, set<string>> getKernelsLiteralSet() const {
        return kernels_literal_set;
    }
    unordered_map<string, set<string>> getCokernelsLiteralSet() const {
        return cokernels_literal_set;
    }
    string getName() const {
        return name;
    }
    string getExpr() const {
        string expr;
        for(auto& term : terms) {
            expr += "+" + term.getExpr();
        }
        return expr;
    }
    void printExpr() const {
        string expr;
        cout << name << " : ";
        for(auto term : terms) {
            expr += "+" + term.getExpr();
        }
        cout << expr << endl;
    }
    void printLiterals() const {
        for(auto lit : literals) {
            cout << lit << " " << flush;
        } cout << endl;
    }
private:
    void insertLiterals(Term &term) {
        auto liter = term.getLiterals();
        for(auto lit : liter) {
            literals.insert(lit);
        }
    }
};