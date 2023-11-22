#pragma once

#include <fstream>
#include <sstream>
#include <deque>
#include "GraphInfo.hpp"
#include "Function.hpp"
#include "Term.hpp"

using namespace std;

GraphInfo parser(fstream &fi) {

	GraphInfo graphInfo;
    string buffer;

	if (!fi.is_open()) {
        cerr << "Failed to open the file." << endl;
        exit(1);
    }
    // ENTER CASE: inputs
    while(fi >> buffer) {
        if(buffer == ".inputs")
            break;
    }  
    while(fi >> buffer) {
        if(buffer == "\\") {
            continue;
        } else if(buffer[0] == '.') {
            break;
        } else {
            graphInfo.insertPI(buffer);
        }
    }
    // ENTER CASE: outputs
    while(buffer != ".outputs") {
        fi >> buffer;
    }
    while(fi >> buffer) {
        if(buffer == "\\") {
            continue;
        } else if(buffer[0] == '.') {
            break;
        } else {
            graphInfo.insertPO(buffer);
        }
    }
    // ENTER CASE: .names
    while(buffer != ".names") {
        fi >> buffer;
    }
    while(buffer != ".end") {
    	string line;
    	vector<string> container;
        getline(fi, line);
        stringstream ss(line);
        while(ss >> buffer) {
            if(buffer == "\\") {
                getline(fi, line);
                ss.clear();
                ss.str(line);
                continue;
            }
            container.push_back(buffer);
        }
        Function function;
        function.setName(container.back());
        container.pop_back();

        while(fi >> buffer) {
            if(buffer == ".names" || buffer == ".end") {
                break;
            }
            else {
                int size = buffer.size();
                int num = 0;
                Term term;
                while(num < size) {
                    string literalName = container[num];
                    if(buffer[num] == '1') {
                        term.insert(literalName);
                    }
                    else if(buffer[num] == '0') {
                        term.insert(literalName+'\'');
                    }
                    num++;
                }
                if(term.getLiterals().size() != 0) {
                    function.insertTerm(term);
                }
            }
            fi >> buffer;
        }
        graphInfo.insertFunction(function);
    }    
    fi.close();
    return graphInfo;
}