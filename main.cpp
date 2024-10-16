#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>

using namespace std;

// Base class: Password
class Password {
protected:
    string username;
    string password;

public:
    Password(const string& u, const string& p) : username(u), password(p) {}
    virtual ~Password() {}

    virtual void display() const = 0;

    string getPassword() const {
        return password;
    }
    
    string getUsername() const {
        return username;
    }

    virtual string toCSV() const = 0;

    bool isUsername(const string& u) const {
        return username == u;
    }
};

class WebsitePassword : public Password {
private:
    string url;

public:
    WebsitePassword(const string& u, const string& p, const string& link) : Password(u, p), url(link) {}

    void display() const override {
        cout << "Website: " << url << "\nUsername: " << username << "\nPassword: " << password << endl;
    }

    string toCSV() const override {
        return "Website," + username + "," + password + "," + url + "\n";
    }

    bool isUrl(const string& link) const {
        return url == link;
    }

    string getUrl() const {
        return url;
    }
};

class AppPassword : public Password {
private:
    string appName;

public:
    AppPassword(const string& u, const string& p, const string& app) : Password(u, p), appName(app) {}

    void display() const override {
        cout << "App: " << appName << "\nUsername: " << username << "\nPassword: " << password << endl;
    }

    string toCSV() const override {
        return "App," + username + "," + password + "," + appName + "\n";
    }

    bool isAppName(const string& app) const {
        return appName == app;
    }

    string getAppName() const {
        return appName;
    }
};

class PasswordManager {
private:
    vector<shared_ptr<Password>> passwordList;
    const string filename = "passwords.csv";

public:
    PasswordManager() {
        loadFromFile();
    }

    void addPassword(shared_ptr<Password> pass) {
        passwordList.push_back(pass);
    }

    void saveToFile() const {
        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "Error: Could not open file for writing." << endl;
            return;
        }
        for (const auto& pass : passwordList) {
            file << pass->toCSV();
        }
        file.close();
        cout << "Passwords saved to " << filename << endl;
    }

    void loadFromFile() {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "No password file found, starting fresh." << endl;
            return;
        }

        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string type, user, pass, extra;

            getline(ss, type, ',');
            getline(ss, user, ',');
            getline(ss, pass, ',');
            getline(ss, extra, ',');

            if (type == "Website") {
                passwordList.push_back(make_shared<WebsitePassword>(user, pass, extra));
            } else if (type == "App") {
                passwordList.push_back(make_shared<AppPassword>(user, pass, extra));
            }
        }
        file.close();
        cout << "Passwords loaded from " << filename << endl;
    }

    void searchPassword() const {
        string username, identifier;

        cout << "Enter username: ";
        cin >> username;
        cout << "Enter app name or URL: ";
        cin >> identifier;

        bool found = false;
        for (const auto& pass : passwordList) {
            if (pass->isUsername(username)) {
                if (auto webPass = dynamic_cast<WebsitePassword*>(pass.get())) {
                    if (webPass->isUrl(identifier)) {
                        webPass->display();
                        found = true;
                    }
                } else if (auto appPass = dynamic_cast<AppPassword*>(pass.get())) {
                    if (appPass->isAppName(identifier)) {
                        appPass->display();
                        found = true;
                    }
                }
            }
        }

        if (!found) {
            cout << "No matching password found." << endl;
        }
    }
};

int main() {
    PasswordManager manager;
    

    return 0;
}
