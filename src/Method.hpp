#pragma once

#include "multipleMatrix.hpp"
#include "cokernelMatrix.hpp"
#include "GraphInfo.hpp"
#include <fstream>
#include <climits>

using namespace std;

extern bool Method_debug_mode;
extern int num;

class Method {
private:
    GraphInfo graphInfo;
    vector<Function> function_sets;
    vector<Function> result;
    int original_literals_count;
    int optimized_literals_count;
public:
    Method(GraphInfo graphInfo) : graphInfo(graphInfo) {
        function_sets = graphInfo.sets;
        original_literals_count = getLiteralCount(function_sets);
        optimized_literals_count = INT_MAX;
    }
    void setFunctionSet(vector<Function>&& sets) {
        function_sets = move(sets);
    }
    bool multiple_cube_extraction() {
        computeCokernel();
        multipleMatrix mtMat(function_sets);
        mtMat.initializeMatrix();
        mtMat.PrimeRectangle();
        mtMat.FinalRefinement();
        const auto& result_original = mtMat.getResultOriginal();
        result.insert(result.end(), result_original.begin(), result_original.end());
        const auto& result_new = mtMat.getResultNew();
        result.insert(result.end(), result_new.begin(), result_new.end());
        int result_literral_count = getLiteralCount(result);
        if(result_literral_count >= optimized_literals_count) return false;
        optimized_literals_count = result_literral_count;
        return true;
    }
    void computeCokernel() {
        for(auto& func : function_sets) {
            cokernelMatrix ckMat;
            ckMat.addFunction(func);
            ckMat.PrimeRectangle();
            func.setCokernels(ckMat.getCokernels());
            func.setkernels(ckMat.getKernels());
            func.setKernelsLiteralSet(ckMat.getKernelsLiteralSet());
            func.setCokernelsLiteralSet(ckMat.getCokernelsLiteralSet());
        }
    }
    vector<Function> getResult() {
        return result;
    }
    int getLiteralCount(vector<Function>& function_set) {
        int count = 0;
        for(auto& func : function_set) {
            for(auto& term : func.getTerms()) {
                count += term.getLiterals().size();
            }
        }
        return count;
    }
    void writeOut() {
        cout << "Original literal count: " << original_literals_count << endl;
        cout << "Optimized literal count: " << optimized_literals_count << endl;
        fstream fo("out.blif", ios::out);
        fo << ".model out" << endl;
        fo << ".inputs ";
        for(auto& in : graphInfo.input) {
            fo << in << " ";
        } 
        fo << "\n.outputs ";
        for(auto& out : graphInfo.output) {
            fo << out << " ";
        } 
        fo << endl;
        for(auto& func : result) {
            fo << ".names ";
            unordered_map<string, int> map; // a 
            int num = 0;
            for(auto& liter : func.getLiterals()) {
                string liter_name;
                if(liter.back() == '\'') {
                    liter_name = liter.substr(0, liter.size()-1);
                } 
                else {
                    liter_name = liter;
                }
                if(map.find(liter_name) == map.end()) {
                    map.emplace(liter_name, num++);
                    fo << liter_name << " ";
                }
            }
            int size = map.size();
            fo << func.getName() << endl;
            for(auto& term : func.getTerms()) {
                vector<string> vec(size, "-");
                for(auto& liter : term.getLiterals()) {
                    if(liter.back() == '\'') vec[map[liter.substr(0, liter.size()-1)]] = "0";
                    else vec[map[liter]] = "1";
                }
                for(auto& v : vec) {
                    fo << v;
                }
                fo << " 1" << endl;
            }
        }
        fo << ".end" << endl;
    }
    void reset() {
        result.clear();
    }
};