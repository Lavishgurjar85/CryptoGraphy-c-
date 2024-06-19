#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cstdlib>
#include <ctime>
#include <sstream>
using namespace std;

string generateSalt() {
    srand(time(nullptr));
    int salt = rand() % 10000;
    stringstream ss;
    ss << salt;
    return ss.str();
}


map<string, pair<string, string>> readUserPasswordsFromFile(const string &filename) {
    map<string, pair<string, string>> userPasswords;
    ifstream file(filename);

    if (file.is_open()) {
        string username, salt, hashedPassword;
        while (file >> username >> salt >> hashedPassword) {
            userPasswords[username] = make_pair(salt, hashedPassword);
        }
        file.close();
    }

    return userPasswords;
}

void storeUserPassword(const string &filename, const string &username, const string &salt, const string &hashedPassword) {
    ofstream file(filename, ios::app);
    if (file.is_open()) {
        file << username << " " << salt << " " << hashedPassword << endl;
        file.close();
    } else {
        cerr << "Error opening file for writing." << endl;
    }
}

bool authenticate(const string &username,  string &password, const map<string, pair<string, string>> &userPasswords) {
    auto it = userPasswords.find(username);
    if (it != userPasswords.end()) {
        string salt = it->second.first;
        string storedHashedPassword = it->second.second;
        std::hash<string> hash_obj;
        password=password+salt;
        password =hash_obj(password);
        if (storedHashedPassword == password) {
            return true;
        }
    }
    return false;
}

int main() {
    std::hash<string> hash_obj;
    string filename = "salting.txt";
    string dd;

    cout << "Welcome to the Registration System" << endl << endl;
    cout << "Want to Register? (yes/no) ";
    cin >> dd;

    if (dd == "yes") {
        string username, password;
        cout << "Enter the data of the Users " << endl << endl;
        cout << "Enter a new username: ";
        cin >> username;
        cout << "Enter a new password: ";
        cin >> password;
       
        string salt = generateSalt();
         password=password+salt;
        password=hash_obj(password);

        storeUserPassword(filename, username, salt, password);

        cout << "User successfully registered" << endl;
    }

    cout << endl;
    cout << "Welcome to the Login System" << endl << endl;
    string username, password;

    cout << "Enter your username: ";
    cin >> username;
    cout << "Enter your password: ";
    cin >> password;

    map<string, pair<string, string>> userPasswords = readUserPasswordsFromFile(filename);

    if (authenticate(username, password, userPasswords)) {
        cout << "Login successful. Access granted." << endl;
    } else {
        cout << "Login failed. Please register." << endl;
    }

    return 0;
}
