#pragma once

#include "Term.hpp"
#include "Function.hpp"
#include <vector>
#include <unordered_map>

using namespace std;

class cokernelMatrix {
private:
    Function func;
    vector<vector<char>> data; // data[row][col]
    set<string> cokernels;
    unordered_map<string, vector<string>> kernels; // cokernel_name kernel_name 
    unordered_map<string, set<string>> kernels_literal_set; // kernel_name kernel_literal
    unordered_map<string, set<string>> cokernels_literal_set; // cokernel_name cokernel_literal
    size_t rowSize;
    size_t colSize;
    unordered_map<int, string> rowTable;
    unordered_map<int, string> colTable_i;
    unordered_map<string, int> colTable_s;
public:
    void addFunction(Function& function) {
        func = function;
        const auto& literals = func.getLiterals(); 
        const auto& terms = func.getTerms();
        colSize = literals.size();
        rowSize = terms.size();

        data.resize(rowSize, vector<char>(colSize, '0'));

        size_t i = 0;
        for (const auto& literal : literals) {
            colTable_i[i] = literal;
            colTable_s[literal] = i++;
        }

        i = 0;
        for (const auto& term : terms) {
            rowTable[i] = term.getExpr();
            const auto& term_literals = term.getLiterals();
            for (const auto& literal : term_literals) {
                data[i][colTable_s[literal]] = '1';
            }
            i++;
        }
    }
    void PrimeRectangle() {
        for(size_t i = 0; i < rowSize; ++i) {
            for(size_t j = 0; j < colSize; ++j) {
                if(data[i][j] == '1' || data[i][j] == '*') {
                    processRC(i, j, func.getTerms());
                    processCR(i, j, func.getTerms());
                }
            }
        }
    }
    void printMatrix() {
        cout << "\nPrint out Matrix" << endl;
        for(size_t i=0; i<colSize; ++i) {
            if(i % 11 == 0 && i != 0) cout << endl;
            cout << i << colTable_i[i] << " ";
        } cout << endl;
        for(size_t i=0; i<rowSize; ++i) {
            if(i % 11 == 0 && i != 0) cout << endl;
            cout << i << rowTable[i] << " ";
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
    set<string> getCokernels() {
        if(cokernels.size() == 0) return {};
        return cokernels;
    }
    unordered_map<string, vector<string>> getKernels() {
        if(kernels.size() == 0) return {};
        return kernels;
    }
    unordered_map<string, set<string>> getKernelsLiteralSet() {
        return kernels_literal_set;
    }
    unordered_map<string, set<string>> getCokernelsLiteralSet() {
        return cokernels_literal_set;
    }
private:
    void processRC(size_t row, size_t col, const vector<Term>& terms) {
        vector<int> row_result = growingRowRC(col);
        if(row_result.size() < 2) return;
        vector<int> col_result = growingColRC(row_result);
        compute(col_result, row_result, terms);
    }
    void processCR(size_t row, size_t col, const vector<Term>& terms) {
        vector<int> col_result = growingColCR(row);
        if(col_result.size() < 2) return;
        vector<int> row_result = growingRowCR(col_result);
        compute(col_result, row_result, terms);
    }
    void compute(const vector<int>& col_result, const vector<int>& row_result, const vector<Term>& terms) {
        vector<string> kernel;
        string cokernel_name;
        set<string> cokernelLiteralSet;
        for(const auto& col : col_result) {
            cokernel_name += colTable_i[col];
            cokernelLiteralSet.insert(colTable_i[col]);
        }
        if(cokernels.find(cokernel_name) == cokernels.end()) {
            cokernels_literal_set.emplace(cokernel_name, cokernelLiteralSet);
            for(const auto& row : row_result) {
                string kernel_name = rowTable[row];
                set<string> kernelLiteralSet;
                for(const auto& term : terms) {
                    if(kernel_name == term.getExpr()) {
                        kernelLiteralSet = term.getLiterals();
                        break;
                    }
                }
                for(const auto& col : col_result) {
                    kernelLiteralSet.erase(colTable_i[col]);
                    kernel_name = divider(kernel_name, colTable_i[col]);
                }
                kernels_literal_set.emplace(kernel_name, kernelLiteralSet);
                kernel.push_back(kernel_name);
            }
            kernels.insert({cokernel_name, kernel});
            cokernels.insert(cokernel_name);       
        }
    }
    vector<int> growingRowRC(int col) {
        vector<int> result;
        for(size_t i=0; i<rowSize; ++i) {
            if(data[i][col] == '1' || data[i][col] == '*') {
                result.push_back(i);
            }
            if(result.size() >= 2) {
                for(auto num : result) {
                    data[num][col] = '*';
                }
            }
        }
        return result;
    }
    vector<int> growingColCR(int row) {
        vector<int> result;
        for(size_t i=0; i<colSize; ++i) {
            if(data[row][i] == '1' || data[row][i] == '*') {
                result.push_back(i);
            }
            if(result.size() >= 2) {
                for(auto num : result) {
                    data[row][num] = '*';
                }
            }
        }
        return result;
    }
    vector<int> growingColRC(vector<int> row) {
        vector<int> result;
        size_t size = row.size();
        for(size_t i=0; i<colSize; ++i) {
            bool flag = true;
            for(size_t j=0; j<size; ++j) {
                if(data[row[j]][i] == '0') {
                    flag = false;
                    break;
                }
            }
            if(flag) {
                for(size_t j=0; j<size; ++j) {
                    data[row[j]][i] = '*';
                }
                result.push_back(i);
            }
        }
        return result;
    }
    vector<int> growingRowCR(vector<int> col) {
        vector<int> result;
        size_t size = col.size();
        for(size_t i=0; i<rowSize; ++i) {
            bool flag = true;
            for(size_t j=0; j<size; ++j) {
                if(data[i][col[j]] == '0') {
                    flag = false;
                    break;
                }
            }
            if(flag) {
                for(size_t j=0; j<size; ++j) {
                    data[i][col[j]] = '*';
                }
                result.push_back(i);
            }
        }
        return result;
    }
    string divider(string term, string literal) {
        size_t pos = term.find(literal);
        term.erase(pos, literal.length());
        return term;        
    }
};