#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <limits>
#include <algorithm>

using namespace std;

// XOR-based simple encryption function
string encryptDecrypt(string text, char key = 'K') {
    string result = text;
    for (char &c : result) {
        c ^= key;
        c &= key;  // XOR each character with the key
    }
    return result;
}

// Class to store password details
class Password {
private:
    string account;
    string username;
    string encryptedPassword;

public:
    Password(string acc, string user, string pass) {
        account = acc;
        username = user;
        encryptedPassword = encryptDecrypt(pass); // Encrypt password before storing
    }

    // Constructor for loading from file
    Password(string acc, string user, string encPass, bool isEncrypted) {
        account = acc;
        username = user;
        encryptedPassword = isEncrypted ? encPass : encryptDecrypt(encPass);
    }

    string getAccount() const { return account; }
    string getUsername() const { return username; }
    string getEncryptedPassword() const { return encryptedPassword; }
    string getDecryptedPassword() const { return encryptDecrypt(encryptedPassword); } // Decrypt when needed
};

// Class to manage all passwords
class PasswordManager {
private:
    vector<Password> passwordList;
    string masterPassword;
    string filename = "passwords.txt";

public:
    PasswordManager(string masterPass) {
        masterPassword = masterPass;
        loadFromFile(); // Load saved passwords on startup
    }

    // Function to add a new password
    void addPassword(string account, string username, string password) {
        Password newPass(account, username, password);
        passwordList.push_back(newPass);
        saveToFile(); // Save the updated list
        cout << "Password saved successfully!\n";
    }

    // Function to display saved passwords
    void viewPasswords() {
        if (passwordList.empty()) {
            cout << "No passwords stored yet.\n";
            return;
        }

        cout << "\nStored Passwords:\n";
        for (const auto &pass : passwordList) {
            cout << "Account: " << pass.getAccount()
                 << " | Username: " << pass.getUsername()
                 << " | Password: " << pass.getDecryptedPassword() << "\n";
        }
    }

    // Save passwords to a file
    void saveToFile() {
        ofstream file(filename);
        if (!file) {
            cout << "Error saving passwords!\n";
            return;
        }

        for (const auto &pass : passwordList) {
            file << pass.getAccount() << " " << pass.getUsername() << " " << pass.getEncryptedPassword() << "\n";
        }
        file.close();
    }

    // Load passwords from a file
    void loadFromFile() {
        ifstream file(filename);
        if (!file) return; // No file exists yet

        string acc, user, encPass;
        passwordList.clear(); // Clear existing passwords before loading
        
        while (file >> acc >> user >> encPass) {
            passwordList.push_back(Password(acc, user, encPass, true)); // Already encrypted in file
        }
        file.close();
    }
};

struct PasswordStrength {
    int score;
    string evaluation;
    vector<string> tips;
};

bool hasCommonPatterns(const string& password) {
    string lower = password;
    transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    // Common patterns to check for
    vector<string> patterns = {
        "password", "admin", "letmein", "welcome",
        "qwerty", "12345", "54321", "11111", "00000", "12345678", "01234567"
    };
    
    // Check if any pattern exists in the password
    for (const auto& pattern : patterns) {
        if (lower.find(pattern) != string::npos) {
            return true;
        }
    }
    
    // Check for 3+ repeated characters (like "aaa")
    for (size_t i = 0; i < password.length() - 2; i++) {
        if (password[i] == password[i+1] && password[i] == password[i+2]) {
            return true;
        }
    }
    return false;
}
PasswordStrength evaluatePasswordStrength(const string &password) {
    PasswordStrength strength;
    strength.score = 0;

    if(password.length() >= 12) strength.score += 12;
    else if(password.length() >= 8) strength.score += 8;
    else strength.tips.push_back("Use at least 8 characters");

    if (hasCommonPatterns(password)){
        strength.score -= 10;
        strength.tips.push_back("Avoid common patterns like '12345678', 'abcdefgh', 'qwertyui', etc.");
    }

    if (any_of(password.begin(), password.end(), ::isupper)) strength.score += 15;
    else strength.tips.push_back("Use upper case letters");

    if (any_of(password.begin(), password.end(), ::isdigit)) strength.score += 15;
    else strength.tips.push_back("Include at least one number");

    strength.score = max(0, min(100, strength.score));
    
    if (strength.score >= 70) strength.evaluation = "Strong";
    else if (strength.score >= 40) strength.evaluation = "Medium";
    else strength.evaluation = "Weak";
    
    return strength;
}
// Main function with authentication
int main() {
    string masterPass;
    cout << "Enter Master Password: ";
    cin >> masterPass;

    PasswordManager manager(masterPass);

    int choice;
    while (true) {
        cout << "\n===== PASSWORD MANAGER MENU =====\n";
        cout << "1 Add New Password\n";
        cout << "2 View Stored Passwords\n";
        cout << "3 Exit\n";
        cout << "Enter choice: ";
        cin >> choice;
        
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer

        if (choice == 1) {
            string acc, user, pass;
            cout << "Enter Account Name: ";
            getline(cin, acc);
            cout << "Enter Username: ";
            getline(cin, user);
            cout << "Enter Password: ";
            getline(cin, pass);
            PasswordStrength strength = evaluatePasswordStrength(pass);
            if (strength.evaluation == "Weak") {
                cout << "Weak password! Please improve it based on the following tips:\n";
                for (const auto& tip : strength.tips) {
                    cout << "- " << tip << "\n";
                }
            } 
            else {
                manager.addPassword(acc, user, pass);
            }
        } 
        else if (choice == 2) {
            manager.viewPasswords();
        } 
        else if (choice == 3) {
            cout << "Exiting Password Manager...\n";
            break;
        } 
        else {
            cout << "Invalid choice! Try again.\n";
        }
    }
    return 0;
}