#ifndef BIGINT_HPP
#define BIGINT_HPP

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <stdexcept>

class BigInt {
private:
    std::vector<int> digits;
    bool negative;
    
    void removeLeadingZeros() {
        while (digits.size() > 1 && digits.back() == 0) {
            digits.pop_back();
        }
        if (digits.size() == 1 && digits[0] == 0) {
            negative = false;
        }
    }
    
public:
    BigInt() : negative(false) {
        digits.push_back(0);
    }
    
    BigInt(const std::string& str) : negative(false) {
        if (str.empty()) {
            digits.push_back(0);
            return;
        }
        
        size_t start = 0;
        if (str[0] == '-') {
            negative = true;
            start = 1;
        } else if (str[0] == '+') {
            start = 1;
        }
        
        if (start >= str.length()) {
            digits.push_back(0);
            return;
        }
        
        for (int i = str.length() - 1; i >= start; i--) {
            if (str[i] < '0' || str[i] > '9') {
                throw std::runtime_error("Invalid character in number: " + std::string(1, str[i]));
            }
            digits.push_back(str[i] - '0');
        }
        
        removeLeadingZeros();
    }
    
    BigInt(long long num) {
        negative = num < 0;
        if (negative) num = -num;
        
        if (num == 0) {
            digits.push_back(0);
        } else {
            while (num > 0) {
                digits.push_back(num % 10);
                num /= 10;
            }
        }
    }
    
    std::string toString() const {
        std::string result;
        if (negative) result += "-";
        
        for (int i = digits.size() - 1; i >= 0; i--) {
            result += (digits[i] + '0');
        }
        
        return result;
    }
    
    bool operator<(const BigInt& other) const {
        if (negative != other.negative) {
            return negative;
        }
        
        if (digits.size() != other.digits.size()) {
            return negative ? digits.size() > other.digits.size() : digits.size() < other.digits.size();
        }
        
        for (int i = digits.size() - 1; i >= 0; i--) {
            if (digits[i] != other.digits[i]) {
                return negative ? digits[i] > other.digits[i] : digits[i] < other.digits[i];
            }
        }
        
        return false;
    }
    
    bool operator>(const BigInt& other) const {
        return other < *this;
    }
    
    bool operator==(const BigInt& other) const {
        return !(*this < other) && !(other < *this);
    }
    
    bool operator!=(const BigInt& other) const {
        return !(*this == other);
    }
    
    BigInt operator+(const BigInt& other) const {
        if (negative != other.negative) {
            if (negative) {
                BigInt temp = *this;
                temp.negative = false;
                return other - temp;
            } else {
                BigInt temp = other;
                temp.negative = false;
                return *this - temp;
            }
        }
        
        BigInt result;
        result.digits.clear();
        result.negative = negative;
        
        int carry = 0;
        size_t maxSize = std::max(digits.size(), other.digits.size());
        
        for (size_t i = 0; i < maxSize || carry; i++) {
            int sum = carry;
            if (i < digits.size()) sum += digits[i];
            if (i < other.digits.size()) sum += other.digits[i];
            
            result.digits.push_back(sum % 10);
            carry = sum / 10;
        }
        
        result.removeLeadingZeros();
        return result;
    }
    
    BigInt operator-(const BigInt& other) const {
        if (negative != other.negative) {
            BigInt result = *this;
            BigInt temp = other;
            temp.negative = !temp.negative;
            return result + temp;
        }
        
        if (negative) {
            BigInt temp1 = *this, temp2 = other;
            temp1.negative = temp2.negative = false;
            return temp2 - temp1;
        }
        
        if (*this < other) {
            BigInt result = other - *this;
            result.negative = true;
            return result;
        }
        
        BigInt result;
        result.digits.clear();
        
        int borrow = 0;
        for (size_t i = 0; i < digits.size(); i++) {
            int sub = digits[i] - borrow;
            if (i < other.digits.size()) sub -= other.digits[i];
            
            if (sub < 0) {
                sub += 10;
                borrow = 1;
            } else {
                borrow = 0;
            }
            
            result.digits.push_back(sub);
        }
        
        result.removeLeadingZeros();
        return result;
    }
    
    BigInt operator*(const BigInt& other) const {
        BigInt result;
        result.digits.assign(digits.size() + other.digits.size(), 0);
        result.negative = negative != other.negative;
        
        for (size_t i = 0; i < digits.size(); i++) {
            for (size_t j = 0; j < other.digits.size(); j++) {
                result.digits[i + j] += digits[i] * other.digits[j];
                result.digits[i + j + 1] += result.digits[i + j] / 10;
                result.digits[i + j] %= 10;
            }
        }
        
        result.removeLeadingZeros();
        return result;
    }
    
    BigInt operator/(const BigInt& other) const {
        if (other == BigInt("0")) {
            throw std::runtime_error("Division by zero");
        }
        
        BigInt dividend = *this;
        BigInt divisor = other;
        dividend.negative = divisor.negative = false;
        
        if (dividend < divisor) {
            return BigInt("0");
        }
        
        BigInt result;
        result.digits.clear();
        
        BigInt current("0");
        for (int i = digits.size() - 1; i >= 0; i--) {
            current = current * BigInt("10") + BigInt(std::to_string(digits[i]));
            
            int count = 0;
            while (!(current < divisor)) {
                current = current - divisor;
                count++;
            }
            
            result.digits.push_back(count);
        }
        
        std::reverse(result.digits.begin(), result.digits.end());
        result.negative = negative != other.negative;
        result.removeLeadingZeros();
        
        return result;
    }
    
    BigInt operator%(const BigInt& other) const {
        BigInt quotient = *this / other;
        return *this - (quotient * other);
    }
};

#endif
