#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include "GraphInfo.hpp"
#include "parser.hpp"
#include "cokernelMatrix.hpp"
#include "Method.hpp"
using namespace std;

extern bool main_debug_mode;

int main(int argc, char** argv) {

    // Set timer
	auto start = chrono::high_resolution_clock::now();
	
	// ========== Program start ==========
	// Is the input correct?
	if(argc != 2) {
		cerr << "Usage : ./main <BLIF_FILE>" << endl;
		return(1);
	}
	// DEFINE VARIABLE
	fstream fi(argv[1], ios::in);
    GraphInfo graphInfo = parser(fi);
    fi.close();
	
	Method method(graphInfo);
	vector<Function> &result = graphInfo.sets;
	while(method.multiple_cube_extraction()) {
		result = method.getResult();		
		method.setFunctionSet(move(result));
		method.reset();
	}
	method.writeOut();

    // ========== Program end ==========

	// Stop timer 
	auto stop = chrono::high_resolution_clock::now();

	// Run time
	auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
	cout << "Program end " << duration.count() << " ms." << endl;

	return 0;
}