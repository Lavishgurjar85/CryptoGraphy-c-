#include <iostream>
#include <cmath>
#include <unordered_set>

// Function to calculate the modular exponentiation (a^b mod m)
long long mod_exp(long long base, long long exponent, long long modulus) {
    long long result = 1;
    base = base % modulus;
    while (exponent > 0) {
        if (exponent % 2 == 1)
            result = (result * base) % modulus;
        exponent = exponent >> 1;
        base = (base * base) % modulus;
    }
    return result;
}

// Function to check if a number is a primitive root modulo a prime
bool is_primitive_root(long long candidate, long long prime) {
    std::unordered_set<long long> residues;
    for (long long i = 1; i < prime; ++i) {
        long long residue = mod_exp(candidate, i, prime);
        if (residues.find(residue) != residues.end()) {
            return false;  // Not a primitive root
        }
        residues.insert(residue);
    }
    return true;  // Found a primitive root
}

// Function to find the smallest primitive root modulo a prime
long long find_primitive_root(long long prime) {
    for (long long candidate = 2; candidate < prime; ++candidate) {
        if (is_primitive_root(candidate, prime)) {
            return candidate;
        }
    }
    return -1;  // No primitive root found
}

int main() {
    // Choose a large prime number
    long long prime = 23;  // Replace with a larger prime number for real-world scenarios

    // Find a primitive root modulo the prime
    long long generator = find_primitive_root(prime);

    if (generator != -1) {
        std::cout << "Prime: " << prime << std::endl;
        std::cout << "Generator: " << generator << std::endl;
    } else {
        std::cout << "No primitive root found for the given prime." << std::endl;
    }

    return 0;
}
