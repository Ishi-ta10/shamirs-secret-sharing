#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <algorithm>
#include <regex>
#include <cmath>
#include "json.hpp"
#include "bigint.hpp"

using json = nlohmann::json;
using namespace std;

class ShamirsSecretSharing {
private:
    int n, k;
    vector<pair<BigInt, BigInt>> shares;
    BigInt PRIME = BigInt("170141183460469231731687303715884105727"); // 2^127 - 1
    
public:
    // Evaluate mathematical expressions from JSON
    BigInt evaluateExpression(const string& expr) {
        // Remove spaces
        string cleanExpr = expr;
        cleanExpr.erase(remove(cleanExpr.begin(), cleanExpr.end(), ' '), cleanExpr.end());
        
        // Check for sum function
        regex sumPattern(R"(sum$$(\d+),(\d+)$$)");
        smatch match;
        if (regex_search(cleanExpr, match, sumPattern)) {
            BigInt a(match[1].str());
            BigInt b(match[2].str());
            return a + b;
        }
        
        // Check for multiply function
        regex multiplyPattern(R"(multiply$$(\d+),(\d+)$$)");
        if (regex_search(cleanExpr, match, multiplyPattern)) {
            BigInt a(match[1].str());
            BigInt b(match[2].str());
            return a * b;
        }
        
        // Check for divide function
        regex dividePattern(R"(divide$$(\d+),(\d+)$$)");
        if (regex_search(cleanExpr, match, dividePattern)) {
            BigInt a(match[1].str());
            BigInt b(match[2].str());
            return a / b;
        }
        
        // Check for lcm function
        regex lcmPattern(R"(lcm$$(\d+),(\d+)$$)");
        if (regex_search(cleanExpr, match, lcmPattern)) {
            BigInt a(match[1].str());
            BigInt b(match[2].str());
            return lcm(a, b);
        }
        
        // Check for hcf/gcd function
        regex hcfPattern(R"((?:hcf|gcd)$$(\d+),(\d+)$$)");
        if (regex_search(cleanExpr, match, hcfPattern)) {
            BigInt a(match[1].str());
            BigInt b(match[2].str());
            return gcd(a, b);
        }
        
        // If it's just a number
        if (regex_match(cleanExpr, regex(R"(\d+)"))) {
            return BigInt(cleanExpr);
        }
        
        throw runtime_error("Unknown expression format: " + expr);
    }
    
    // Greatest Common Divisor
    BigInt gcd(BigInt a, BigInt b) {
        while (b != BigInt("0")) {
            BigInt temp = b;
            b = a % b;
            a = temp;
        }
        return a;
    }
    
    // Least Common Multiple
    BigInt lcm(BigInt a, BigInt b) {
        return (a * b) / gcd(a, b);
    }
    
    // Modular exponentiation
    BigInt modPow(BigInt base, BigInt exp, BigInt mod) {
        BigInt result("1");
        base = base % mod;
        while (exp > BigInt("0")) {
            if (exp % BigInt("2") == BigInt("1")) {
                result = (result * base) % mod;
            }
            exp = exp / BigInt("2");
            base = (base * base) % mod;
        }
        return result;
    }
    
    // Modular inverse using Fermat's little theorem
    BigInt modInverse(BigInt a, BigInt p) {
        return modPow(a, p - BigInt("2"), p);
    }
    
    // Lagrange interpolation to find constant term
    BigInt lagrangeInterpolation(const vector<pair<BigInt, BigInt>>& points) {
        BigInt result("0");
        int n = points.size();
        
        for (int i = 0; i < n; i++) {
            BigInt term = points[i].second;
            BigInt numerator("1");
            BigInt denominator("1");
            
            for (int j = 0; j < n; j++) {
                if (i != j) {
                    numerator = (numerator * (BigInt("0") - points[j].first)) % PRIME;
                    denominator = (denominator * (points[i].first - points[j].first)) % PRIME;
                }
            }
            
            // Handle negative numbers
            if (numerator < BigInt("0")) {
                numerator = numerator + PRIME;
            }
            if (denominator < BigInt("0")) {
                denominator = denominator + PRIME;
            }
            
            BigInt inv = modInverse(denominator, PRIME);
            term = ((term * numerator) % PRIME * inv) % PRIME;
            result = (result + term) % PRIME;
        }
        
        return result;
    }
    
    // Generate all combinations of k elements from n elements
    void generateCombinations(vector<int>& arr, int k, int start, vector<int>& current, 
                            vector<vector<int>>& result) {
        if (current.size() == k) {
            result.push_back(current);
            return;
        }
        
        for (int i = start; i <= arr.size() - (k - current.size()); i++) {
            current.push_back(arr[i]);
            generateCombinations(arr, k, i + 1, current, result);
            current.pop_back();
        }
    }
    
    vector<vector<int>> getCombinations(int n, int k) {
        vector<int> indices(n);
        for (int i = 0; i < n; i++) {
            indices[i] = i;
        }
        
        vector<vector<int>> combinations;
        vector<int> current;
        generateCombinations(indices, k, 0, current, combinations);
        return combinations;
    }
    
public:
    void solve(const string& filename) {
        // Read JSON file
        ifstream file(filename);
        if (!file.is_open()) {
            throw runtime_error("Could not open file: " + filename);
        }
        
        json data;
        file >> data;
        file.close();
        
        n = data["n"];
        k = data["k"];
        
        cout << "N (total shares): " << n << endl;
        cout << "K (minimum required): " << k << endl;
        cout << "Polynomial degree: " << (k - 1) << endl << endl;
        
        // Parse and evaluate shares
        shares.clear();
        auto jsonShares = data["shares"];
        
        cout << "Evaluating shares:" << endl;
        for (int i = 0; i < n; i++) {
            int id = jsonShares[i]["id"];
            string valueExpr = jsonShares[i]["value"];
            BigInt value = evaluateExpression(valueExpr);
            
            shares.push_back({BigInt(to_string(id)), value});
            cout << "Share " << id << ": " << valueExpr << " = " << value.toString() << endl;
        }
        cout << endl;
        
        // Generate all combinations
        vector<vector<int>> combinations = getCombinations(n, k);
        map<string, int> secretCounts;
        map<string, vector<int>> secretToCombination;
        
        cout << "Trying " << combinations.size() << " combinations..." << endl;
        
        // Try each combination
        for (const auto& combo : combinations) {
            try {
                vector<pair<BigInt, BigInt>> points;
                for (int idx : combo) {
                    points.push_back(shares[idx]);
                }
                
                BigInt secret = lagrangeInterpolation(points);
                string secretStr = secret.toString();
                
                secretCounts[secretStr]++;
                if (secretToCombination.find(secretStr) == secretToCombination.end()) {
                    secretToCombination[secretStr] = combo;
                }
            } catch (const exception& e) {
                // Skip invalid combinations
                continue;
            }
        }
        
        // Find the most common secret
        string correctSecret;
        int maxCount = 0;
        
        for (const auto& pair : secretCounts) {
            if (pair.second > maxCount) {
                maxCount = pair.second;
                correctSecret = pair.first;
            }
        }
        
        if (correctSecret.empty()) {
            throw runtime_error("Could not determine the correct secret");
        }
        
        cout << "Secret found: " << correctSecret << endl;
        cout << "Appears in " << maxCount << " out of " << combinations.size() << " combinations" << endl;
        
        // Find wrong shares
        set<int> validShareIndices;
        
        for (const auto& combo : combinations) {
            try {
                vector<pair<BigInt, BigInt>> points;
                for (int idx : combo) {
                    points.push_back(shares[idx]);
                }
                
                BigInt secret = lagrangeInterpolation(points);
                if (secret.toString() == correctSecret) {
                    for (int idx : combo) {
                        validShareIndices.insert(idx);
                    }
                }
            } catch (const exception& e) {
                continue;
            }
        }
        
        // Identify wrong shares
        vector<int> wrongShares;
        for (int i = 0; i < n; i++) {
            if (validShareIndices.find(i) == validShareIndices.end()) {
                wrongShares.push_back(i + 1); // Convert to 1-based indexing for share IDs
            }
        }
        
        cout << endl << "=== RESULTS ===" << endl;
        cout << "SECRET: " << correctSecret << endl;
        
        if (!wrongShares.empty()) {
            cout << "WRONG SHARES DETECTED: ";
            for (size_t i = 0; i < wrongShares.size(); i++) {
                cout << "Share " << wrongShares[i];
                if (i < wrongShares.size() - 1) cout << ", ";
            }
            cout << endl;
        } else {
            cout << "NO WRONG SHARES DETECTED" << endl;
        }
        
        cout << "VALID COMBINATIONS: " << maxCount << "/" << combinations.size() << endl;
    }
};

int main(int argc, char* argv[]) {
    try {
        string filename = "input.json";
        
        // Allow command line argument for filename
        if (argc > 1) {
            filename = argv[1];
        }
        
        cout << "Shamir's Secret Sharing Algorithm" << endl;
        cout << "=================================" << endl;
        cout << "Reading from: " << filename << endl << endl;
        
        ShamirsSecretSharing solver;
        solver.solve(filename);
        
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}
