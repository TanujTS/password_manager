#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <algorithm>

using namespace std;

#define MAX_PASSWORDS 100
#define MAX_TIPS 5

// XOR-based simple encryption function
string encryptDecrypt(string text, char key = 'K')
{
    string result = text;
    for (int i = 0; i < result.length(); ++i)
    {
        result[i] ^= key;
    }
    return result;
}

// Class to store password details
class Password
{
private:
    string account;
    string username;
    string encryptedPassword;

public:
    Password() {}

    Password(string acc, string user, string pass)
    {
        account = acc;
        username = user;
        encryptedPassword = encryptDecrypt(pass);
    }

    Password(string acc, string user, string encPass, bool isEncrypted)
    {
        account = acc;
        username = user;
        encryptedPassword = isEncrypted ? encPass : encryptDecrypt(encPass);
    }

    string getAccount() const { return account; }
    string getUsername() const { return username; }
    string getEncryptedPassword() const { return encryptedPassword; }
    string getDecryptedPassword() const { return encryptDecrypt(encryptedPassword); }
};

// Struct to store password strength details
struct PasswordStrength
{
    int score;
    string evaluation;
    string tips[MAX_TIPS];
    int tipCount = 0;
};

// Check for common patterns in passwords
bool hasCommonPatterns(const string &password)
{
    string lower = password;
    transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    const string patterns[] = {
        "password", "admin", "letmein", "welcome",
        "qwerty", "12345", "54321", "11111", "00000", "12345678", "01234567"};

    for (const string &pattern : patterns)
    {
        if (lower.find(pattern) != string::npos)
        {
            return true;
        }
    }

    for (size_t i = 0; i < password.length() - 2; i++)
    {
        if (password[i] == password[i + 1] && password[i] == password[i + 2])
        {
            return true;
        }
    }

    return false;
}

// Evaluate password strength
PasswordStrength evaluatePasswordStrength(const string &password)
{
    // todo: add check for special symbols: @#$
    PasswordStrength strength;
    strength.score = 0;

    if (password.length() >= 12)
        strength.score += 12;
    else if (password.length() >= 8)
        strength.score += 8;
    else
        strength.tips[strength.tipCount++] = "Use at least 8 characters";

    if (hasCommonPatterns(password))
    {
        strength.score -= 10;
        strength.tips[strength.tipCount++] = "Avoid common patterns like '12345678', 'qwerty', etc.";
    }

    if (any_of(password.begin(), password.end(), ::isupper))
        strength.score += 15;
    else
        strength.tips[strength.tipCount++] = "Use upper case letters";

    if (any_of(password.begin(), password.end(), ::isdigit))
        strength.score += 15;
    else
        strength.tips[strength.tipCount++] = "Include at least one number";

    strength.score = max(0, min(100, strength.score));

    if (strength.score >= 70)
        strength.evaluation = "Strong";
    else if (strength.score >= 40)
        strength.evaluation = "Medium";
    else
        strength.evaluation = "Weak";

    return strength;
}

// Class to manage all passwords
class PasswordManager
{
private:
    Password passwordList[MAX_PASSWORDS];
    int passwordCount = 0;
    string masterPassword = "tanuj123";
    string filename = "passwords.txt";

public:
    PasswordManager(string masterPass)
    {
        masterPassword = masterPass;
        loadFromFile();
    }

    void addPassword(string account, string username, string password)
    {
        if (passwordCount >= MAX_PASSWORDS)
        {
            cout << "Password limit reached!\n";
            return;
        }

        Password newPass(account, username, password);
        passwordList[passwordCount++] = newPass;
        saveToFile();
        cout << "Password saved successfully!\n";
    }

    void viewPasswords()
    {
        if (passwordCount == 0)
        {
            cout << "No passwords stored yet.\n";
            return;
        }

        cout << "\nStored Passwords:\n";
        for (int i = 0; i < passwordCount; ++i)
        {
            cout << "Account: " << passwordList[i].getAccount()
                 << " | Username: " << passwordList[i].getUsername()
                 << " | Password: " << passwordList[i].getDecryptedPassword() << "\n";
        }
    }

    void saveToFile()
    {
        ofstream file(filename);
        if (!file)
        {
            cout << "Error saving passwords!\n";
            return;
        }

        for (int i = 0; i < passwordCount; ++i)
        {
            file << passwordList[i].getAccount() << "|"
                 << passwordList[i].getUsername() << "|"
                 << passwordList[i].getEncryptedPassword() << "\n";
        }
        file.close();
    }

    void loadFromFile()
    {
        ifstream file(filename);
        if (!file)
            return;

        string line;
        passwordCount = 0;

        while (getline(file, line) && passwordCount < MAX_PASSWORDS)
        {
            size_t pos1 = line.find("|");
            size_t pos2 = line.find("|", pos1 + 1);
            if (pos1 == string::npos || pos2 == string::npos)
                continue;
            string acc = line.substr(0, pos1);
            string user = line.substr(pos1 + 1, pos2 - pos1 - 1);
            string encPass = line.substr(pos2 + 1);
            passwordList[passwordCount++] = Password(acc, user, encPass, true);
        }
        file.close();
    }
};

// Main function
int main()
{
    string masterPass;
    cout << "Enter Master Password: ";
    cin >> masterPass;

    PasswordManager manager(masterPass);

    int choice;
    while (true)
    {
        cout << "\n===== PASSWORD MANAGER MENU =====\n";
        cout << "1 Add New Password\n";
        cout << "2 View Stored Passwords\n";
        cout << "3 Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice == 1)
        {
            string acc, user, pass;
            cout << "Enter Account Name: ";
            getline(cin, acc);
            cout << "Enter Username: ";
            getline(cin, user);
            cout << "Enter Password: ";
            getline(cin, pass);

            PasswordStrength strength = evaluatePasswordStrength(pass);
            if (strength.evaluation == "Weak")
            {
                cout << "Weak password! Please improve it based on the following tips:\n";
                for (int i = 0; i < strength.tipCount; ++i)
                {
                    cout << "- " << strength.tips[i] << "\n";
                }
            }
            else
            {
                manager.addPassword(acc, user, pass);
            }
        }
        else if (choice == 2)
        {
            manager.viewPasswords();
        }
        else if (choice == 3)
        {
            cout << "Exiting Password Manager...\n";
            break;
        }
        else
        {
            cout << "Invalid choice! Try again.\n";
        }
    }

    return 0;
}
