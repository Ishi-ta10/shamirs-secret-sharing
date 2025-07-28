#ifndef POLYNOMIAL_SOLVER_HPP
#define POLYNOMIAL_SOLVER_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <cctype>
#include "json.hpp"
#include "bigint.hpp"

using json = nlohmann::json;
using namespace std;

class PolynomialSolver {
private:
    struct Point {
        BigInt x;
        BigInt y;
        
        Point(const BigInt& x_val, const BigInt& y_val) : x(x_val), y(y_val) {}
    };
    
    vector<Point> points;
    int n, k;
    
public:
    // Convert a character to its numeric value in given base
    int charToDigit(char c) {
        if (c >= '0' && c <= '9') {
            return c - '0';
        } else if (c >= 'a' && c <= 'z') {
            return c - 'a' + 10;
        } else if (c >= 'A' && c <= 'Z') {
            return c - 'A' + 10;
        }
        throw runtime_error("Invalid character in number: " + string(1, c));
    }
    
    // Decode a value from given base to decimal
    BigInt decodeFromBase(const string& value, int base) {
        if (base < 2 || base > 36) {
            throw runtime_error("Invalid base: " + to_string(base));
        }
        
        BigInt result("0");
        BigInt baseValue(to_string(base));
        
        for (char c : value) {
            int digit = charToDigit(c);
            if (digit >= base) {
                throw runtime_error("Invalid digit '" + string(1, c) + "' for base " + to_string(base));
            }
            
            result = result * baseValue + BigInt(to_string(digit));
        }
        
        return result;
    }
    
    // Lagrange interpolation to find polynomial value at x=0 (constant term)
    BigInt lagrangeInterpolation(const vector<Point>& points) {
        BigInt result("0");
        int n = points.size();
        
        cout << "   Using Lagrange interpolation with " << n << " points:" << endl;
        
        for (int i = 0; i < n; i++) {
            cout << "     Point " << (i+1) << ": (" << points[i].x.toString() 
                 << ", " << points[i].y.toString() << ")" << endl;
            
            BigInt term = points[i].y;
            BigInt numerator("1");
            BigInt denominator("1");
            
            // Calculate Lagrange basis polynomial L_i(0)
            for (int j = 0; j < n; j++) {
                if (i != j) {
                    // For x = 0: (0 - x_j) / (x_i - x_j) = -x_j / (x_i - x_j)
                    numerator = numerator * (BigInt("0") - points[j].x);
                    denominator = denominator * (points[i].x - points[j].x);
                }
            }
            
            // Handle division for the term
            // Since we're working with integers, we need to be careful with division
            // In practice, for Shamir's secret sharing, the result should be an integer
            if (denominator != BigInt("0")) {
                term = term * numerator / denominator;
                result = result + term;
            }
        }
        
        return result;
    }
    
    // Alternative method using matrix approach (Gaussian elimination)
    BigInt matrixMethod(const vector<Point>& points) {
        int n = points.size();
        cout << "   Using Matrix method with " << n << " points" << endl;
        
        // For simplicity, we'll use Lagrange interpolation
        // Matrix method would require more complex BigInt matrix operations
        return lagrangeInterpolation(points);
    }
    
    BigInt solve(const string& filename) {
        // Read JSON file
        ifstream file(filename);
        if (!file.is_open()) {
            throw runtime_error("Could not open file: " + filename);
        }
        
        json data;
        file >> data;
        file.close();
        
        // Parse keys
        n = data["keys"]["n"];
        k = data["keys"]["k"];
        
        cout << "   Configuration:" << endl;
        cout << "     n (total roots): " << n << endl;
        cout << "     k (minimum required): " << k << endl;
        cout << "     Polynomial degree (m): " << (k-1) << endl;
        cout << endl;
        
        // Parse and decode points
        points.clear();
        cout << "   Decoding points:" << endl;
        
        for (auto& item : data.items()) {
            string key = item.key();
            
            // Skip the "keys" entry
            if (key == "keys") continue;
            
            // Parse x coordinate (the key)
            BigInt x(key);
            
            // Parse base and value
            int base = stoi(item.value()["base"].get<string>());
            string encodedValue = item.value()["value"].get<string>();
            
            // Decode y coordinate
            BigInt y = decodeFromBase(encodedValue, base);
            
            points.emplace_back(x, y);
            
            cout << "     x=" << x.toString() << ", base=" << base 
                 << ", encoded='" << encodedValue << "' -> y=" << y.toString() << endl;
        }
        
        cout << endl;
        
        // Sort points by x coordinate for consistency
        sort(points.begin(), points.end(), [](const Point& a, const Point& b) {
            return a.x < b.x;
        });
        
        // We need exactly k points to solve the polynomial
        if (points.size() < k) {
            throw runtime_error("Not enough points to solve polynomial. Need " + 
                              to_string(k) + ", got " + to_string(points.size()));
        }
        
        // Use first k points (we could use any k points)
        vector<Point> selectedPoints(points.begin(), points.begin() + k);
        
        cout << "   Selected " << k << " points for interpolation:" << endl;
        for (const auto& point : selectedPoints) {
            cout << "     (" << point.x.toString() << ", " << point.y.toString() << ")" << endl;
        }
        cout << endl;
        
        // Find the constant term using Lagrange interpolation
        cout << "   Computing polynomial constant term..." << endl;
        BigInt secret = lagrangeInterpolation(selectedPoints);
        
        return secret;
    }
};

#endif
