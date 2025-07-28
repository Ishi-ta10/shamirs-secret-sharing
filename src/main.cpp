#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <algorithm>
#include <regex>
#include <cmath>
#include <chrono>
#include "json.hpp"
#include "bigint.hpp"
#include "shamirs_solver.hpp"

using json = nlohmann::json;
using namespace std;

void printUsage(const string& programName) {
    cout << "Usage: " << programName << " [input_file.json] [options]\n";
    cout << "Options:\n";
    cout << "  -h, --help     Show this help message\n";
    cout << "  -v, --verbose  Enable verbose output\n";
    cout << "  -t, --time     Show execution time\n";
    cout << "\nExample:\n";
    cout << "  " << programName << " input.json\n";
    cout << "  " << programName << " examples/test1.json --verbose\n";
}

int main(int argc, char* argv[]) {
    try {
        string filename = "input.json";
        bool verbose = false;
        bool showTime = false;
        
        // Parse command line arguments
        for (int i = 1; i < argc; i++) {
            string arg = argv[i];
            if (arg == "-h" || arg == "--help") {
                printUsage(argv[0]);
                return 0;
            } else if (arg == "-v" || arg == "--verbose") {
                verbose = true;
            } else if (arg == "-t" || arg == "--time") {
                showTime = true;
            } else if (arg[0] != '-') {
                filename = arg;
            }
        }
        
        cout << "🔐 Shamir's Secret Sharing Algorithm" << endl;
        cout << "====================================" << endl;
        cout << "Reading from: " << filename << endl;
        if (verbose) cout << "Verbose mode: ON" << endl;
        cout << endl;
        
        auto startTime = chrono::high_resolution_clock::now();
        
        ShamirsSecretSharing solver(verbose);
        solver.solve(filename);
        
        auto endTime = chrono::high_resolution_clock::now();
        
        if (showTime) {
            auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);
            cout << "\n⏱️  Execution time: " << duration.count() << " ms" << endl;
        }
        
    } catch (const exception& e) {
        cerr << "❌ Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}
