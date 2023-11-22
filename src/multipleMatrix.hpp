#pragma once

#include "Term.hpp"
#include "Function.hpp"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm> 

using namespace std;

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1, T2> &pair) const {
        auto hash1 = std::hash<T1>{}(pair.first);
        auto hash2 = std::hash<T2>{}(pair.second);
        return hash1 ^ hash2;
    }
};

class multipleMatrix {
private:
    vector<vector<char>> data; // data[row][col]
    vector<Function> functions;
    vector<Function> result_original;
    vector<Function> result_new;
    set<string> set_kernels;
    unordered_map<string, vector<size_t>> FuncRange;
    unordered_map<string, vector<Term>> function_remaining_term;
    unordered_map<string, set<string>> kernels_literal_set; // kernel_name kernel_literal
    unordered_map<string, set<string>> cokernels_literal_set; // cokernel_name cokernel_literal
    unordered_map<int, pair<string, string>> rowTable_i;
    unordered_map<pair<string, string>, int, pair_hash> rowTable_s;
    unordered_map<int, string> colTable_i;
    unordered_map<string, int> colTable_s;
    size_t rowSize;
    size_t colSize;
    size_t pickedRow;
public:
    static int num;
    multipleMatrix(vector<Function>& sets) : functions(sets), rowSize(0), colSize(0), pickedRow(0) {
        makeOriginalFunction();
    }
    void initializeMatrix() {
        for(const auto& func : functions) {
            rowSize += func.getCokernels().size();
            function_remaining_term.insert({func.getName(), func.getTerms()});
            for (const auto& [key, value] : func.getKernels()) {
                set_kernels.insert(value.begin(), value.end());
            }
        }
        colSize = set_kernels.size();
        data.resize(rowSize, vector<char>(colSize, '0'));
        // Add collumn
        size_t i = 0; 
        for (const auto& kernel : set_kernels) {
            colTable_i[i] = kernel;
            colTable_s[kernel] = i++;
        }
        size_t row_count = 0;
        for(const auto& func : functions) {
            addFunction(func, row_count);
        }
    }
    void PrimeRectangle() {
        // Pick row
        pickRow();
        for(size_t j = 0; j < colSize; ++j) {
            if(data[pickedRow][j] == '1') {
                const auto& row_result = growingRow(j);
                const auto& col_result = growingCol(row_result);
                if(isSaved(row_result, col_result)) {
                    string node_name = "[mtMat" + to_string(num++) + "]";
                    makeNewFunction(node_name, col_result);
                    buildOriginalFunction(node_name, row_result, col_result);
                }
            }
        }
    }
    void FinalRefinement() {
        for(auto& func : result_original) {
            const auto& remaining_term = function_remaining_term[func.getName()];
            for(const auto& term : remaining_term) {
                func.insertTerm(move(term));
            }
        }
    }
    vector<Function> getResultNew() const {
        return result_new;
    }
    vector<Function> getResultOriginal() const {
        return result_original;
    }
    bool isSimplify() {
        return result_new.size();
    }
    void printMatrix() {
        cout << "Print out Matrix" << endl;
        for(size_t i=0; i<colSize; ++i) {
            if(i % 11 == 0 && i != 0) cout << endl;
            cout << i << colTable_i[i] << " ";
        } cout << endl;
        for(size_t i=0; i<rowSize; ++i) {
            if(i % 11 == 0 && i != 0) cout << endl;
            cout << i << rowTable_i[i].second << " ";
        } cout << "\n   ";
        for(size_t i=0; i<colSize; ++i) {
            cout << i;
        } cout << endl;
        for(size_t i=0; i<rowSize; ++i) {
            cout << i << "  ";
            for(size_t j=0; j<colSize; ++j) {
                cout << data[i][j];
            }
            cout << endl;
        }
        cout << endl;
    }
private:
    void addFunction(const Function &func, size_t &row_count) {
        const auto& cokernels = func.getCokernels(); // {a, ab}
        const auto& kernels = func.getKernels();
        string func_name = func.getName();
        // Add literal set
        for(const auto& [key, value] : func.getCokernelsLiteralSet()) {
            cokernels_literal_set.insert({key, value});
        }
        for(const auto& [key, value] : func.getKernelsLiteralSet()) {
            kernels_literal_set.insert({key, value});
        }

        // Add function range
        FuncRange[func_name].push_back(row_count);
        FuncRange[func_name].push_back(row_count + cokernels.size());
        // Add row
        for (const auto& cokernel : cokernels) {
            rowTable_i.emplace(row_count, make_pair(func_name, cokernel));
            rowTable_s.emplace(make_pair(func_name, cokernel), row_count++);
        }   
        // Build Matrix
        for(const auto& cokernel_name : cokernels) {
            for(const auto& kernel_name : kernels.at(cokernel_name)) {
                data[rowTable_s[{func_name, cokernel_name}]][colTable_s[kernel_name]] = '1';
            }
        }
    }
    vector<int> growingRow(int col) {
        vector<int> result;
        for(size_t i=0; i<rowSize; ++i) {
            if(data[i][col] == '1') {
                result.push_back(i);
            }
        }
        return result;
    }
    vector<int> growingCol(const vector<int>& row) {
        vector<int> result;
        result.reserve(colSize);
        for(size_t colIndex=0; colIndex<colSize; ++colIndex) {
            bool flag = true;
            for(const auto& rowIndex : row) {
                if(data[rowIndex][colIndex] == '0') {
                    flag = false;
                    break;
                }
            }
            if(flag) {
                result.push_back(colIndex);
            }
        }
        return result;
    }
    bool isSaved(const vector<int>& row, const vector<int> &col) {
        if(row.empty() || col.empty()) return false;

        int saved = 0;
        int row_weight = 0;
        int col_weight = 0;
        
        // Check selected rectangle is good 
        for(auto& row_num : row) {
            row_weight += cokernels_literal_set[rowTable_i[row_num].second].size() + 1;
            for(auto& col_num : col) {
                if(data[row_num][col_num] == '1') {
                    saved += cokernels_literal_set[rowTable_i[row_num].second].size() + kernels_literal_set[colTable_i[col_num]].size();
                }
            }
        }
        for(auto& col_num : col) {
            col_weight += kernels_literal_set[colTable_i[col_num]].size();
        }
        // Update Matrix
        if(saved > row_weight + col_weight) {
            for (auto& row_num : row) {
                // Extract kernel
                for (auto& col_num : col) {
                    Term original_term;
                    for(auto& s : cokernels_literal_set[rowTable_i[row_num].second]) {
                        original_term.insert(s);
                    }
                    for(auto& s : kernels_literal_set[colTable_i[col_num]]) {
                        original_term.insert(s);
                    }
                    auto& remaining_terms = function_remaining_term[findName(row_num)];
                    for(auto it = remaining_terms.begin(); it != remaining_terms.end(); ++it) {
                        if(original_term.getExpr() == it->getExpr()) {
                            it = remaining_terms.erase(it);
                            break;
                        }
                    }
                    // 假設kernel為a+b+c、cokernel為de，這邊是把ade、bde、cde，變成*
                    data[row_num][col_num] = '*';
                    auto range = FuncRange[rowTable_i[row_num].first];
                }
            }
            return true;
        }
        return false;
    }
    bool checkElements(const set<string>& set1, const set<string>& kernel, const set<string>& cokernel) {
        set<string> kernelCokernelUnion;
        std::set_union(kernel.begin(), kernel.end(), cokernel.begin(), cokernel.end(), inserter(kernelCokernelUnion, kernelCokernelUnion.begin()));
        return std::includes(kernelCokernelUnion.begin(), kernelCokernelUnion.end(), set1.begin(), set1.end());
    }
    void pickRow() {
        int best_count = 0;
        size_t original_picked_row = pickedRow;
        for (size_t i = 0; i < rowSize; ++i) {
            int count = std::count(begin(data[i]), end(data[i]), '1');
            if (count > best_count || (count == best_count && i > original_picked_row)) {
                best_count = count;
                pickedRow = i;
            }
        }
    }
    void makeNewFunction(const string& node_name, const vector<int>& col_result) {
        Function temp;
        temp.setName(node_name);
        for(const auto& col : col_result) {
            temp.insertTerm(makeTerm(kernels_literal_set[colTable_i[col]]));
        }
        result_new.emplace_back(move(temp));
    }
    void makeOriginalFunction() {
        for(const auto& function : functions) {
            Function temp;
            temp.setName(function.getName());
            result_original.emplace_back(move(temp));
        }
    }
    void buildOriginalFunction(const string& node_name, const vector<int>& row_result, const vector<int>& col_result) {
        vector<string> functions_name = findIntervals(row_result);
        for(auto& original : result_original) {
            for(const auto& name : functions_name) {
                if(original.getName() != name) continue;

                for(const auto& row : row_result) {
                    if(FuncRange[name][0] <= size_t(row) && size_t(row) < FuncRange[name][1]) {
                        Term temp;
                        for(const auto& col : col_result) {
                            temp.insert(node_name);
                            for(auto& lit : cokernels_literal_set[rowTable_i[row].second]) {
                                temp.insert(lit);
                            }
                        }
                        original.insertTerm(move(temp));
                    }
                }
            }
        }
    }
    Term makeTerm(const set<string>& literals) {
        Term temp;
        for(const auto& literal : literals) {
            temp.insert(literal);
        }
        return temp;
    }
    vector<string> findIntervals(const vector<int>& index) {
        vector<string> intervals;
        for (const auto& pair : FuncRange) {
            const string& Function_name = pair.first;
            const vector<size_t>& range = pair.second;
            
            for (size_t idx : index) {
                if (idx >= range[0] && idx < range[1]) {
                    intervals.push_back(Function_name);
                    break;
                }
            }
        }
        return intervals;
    }
    string findName(size_t index) {
        string name;
        for (const auto& pair : FuncRange) {
            const string& Function_name = pair.first;
            const vector<size_t>& range = pair.second;
            
            if (index >= range[0] && index < range[1]) {
                name = Function_name;
                break;
            }
        }
        return name;
    }    
};

int multipleMatrix::num = 0;
