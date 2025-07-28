#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include "json.hpp"
#include "bigint.hpp"
#include "polynomial_solver.hpp"

using json = nlohmann::json;
using namespace std;

int main() {
    try {
        cout << "🔐 Catalog Placements Assignment - Shamir's Secret Sharing" << endl;
        cout << "==========================================================" << endl;
        cout << endl;
        
        // Process Test Case 1
        cout << "📋 Processing Test Case 1..." << endl;
        cout << "----------------------------" << endl;
        
        PolynomialSolver solver1;
        BigInt secret1 = solver1.solve("testcases/testcase1.json");
        
        cout << "🎯 Test Case 1 Result:" << endl;
        cout << "   Secret (c): " << secret1.toString() << endl;
        cout << endl;
        
        // Process Test Case 2
        cout << "📋 Processing Test Case 2..." << endl;
        cout << "----------------------------" << endl;
        
        PolynomialSolver solver2;
        BigInt secret2 = solver2.solve("testcases/testcase2.json");
        
        cout << "🎯 Test Case 2 Result:" << endl;
        cout << "   Secret (c): " << secret2.toString() << endl;
        cout << endl;
        
        // Final Output
        cout << "🏆 FINAL RESULTS" << endl;
        cout << "================" << endl;
        cout << "Test Case 1 Secret: " << secret1.toString() << endl;
        cout << "Test Case 2 Secret: " << secret2.toString() << endl;
        
    } catch (const exception& e) {
        cerr << "❌ Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}
