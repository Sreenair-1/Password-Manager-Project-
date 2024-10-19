#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>

using namespace std;

class Password
{
protected:
    string email;
    string password;

public:
    Password(const string &e, const string &p) : email(e), password(p) {}
    virtual ~Password() {}

    virtual void display() const = 0;

    string setPassword(const string &newPass)
    {
        return newPass;
    }

    string getPassword() const
    {
        return password;
    }

    string getemail() const
    {
        return email;
    }

    virtual string toCSV() const = 0;

    bool isemail(const string &e) const
    {
        return email == e;
    }
};

class WebsitePassword : public Password
{
private:
    string url;

public:
    WebsitePassword(const string &e, const string &p, const string &link) : Password(e, p), url(link) {}

    void display() const override
    {
        cout << "Website: " << url << "\nemail: " << email << "\nPassword: " << password << endl;
    }

    string toCSV() const override
    {
        return "Website," + email + "," + password + "," + url + "\n";
    }

    bool isUrl(const string &link) const
    {
        return url == link;
    }

    string getUrl() const
    {
        return url;
    }
};

class AppPassword : public Password
{
private:
    string appName;

public:
    AppPassword(const string &e, const string &p, const string &app) : Password(e, p), appName(app) {}

    void display() const override
    {
        cout << "App: " << appName << "\nemail: " << email << "\nPassword: " << password << endl;
    }

    string toCSV() const override
    {
        return "App," + email + "," + password + "," + appName + "\n";
    }

    bool isAppName(const string &app) const
    {
        return appName == app;
    }

    string getAppName() const
    {
        return appName;
    }
};

class PasswordManager
{
private:
    vector<shared_ptr<Password>> passwordList;
    const string filename = "passwords.csv";

    shared_ptr<Password> findPassword(const string &email, const string &identifier) const
    {
        for (const auto &pass : passwordList)
        {
            if (pass->isemail(email))
            {
                if (auto webPass = dynamic_cast<WebsitePassword *>(pass.get()))
                {
                    if (webPass->isUrl(identifier))
                    {
                        return pass;
                    }
                }
                else if (auto appPass = dynamic_cast<AppPassword *>(pass.get()))
                {
                    if (appPass->isAppName(identifier))
                    {
                        return pass;
                    }
                }
            }
        }
        return nullptr;
    }

public:
    PasswordManager()
    {
        loadFromFile();
    }

    void addPassword(shared_ptr<Password> pass)
    {
        passwordList.push_back(pass);
        saveToFile();
    }

    void saveToFile() const
    {
        ofstream file(filename);
        if (!file.is_open())
        {
            cout << "Error: Could not open file for writing." << endl;
            return;
        }
        for (const auto &pass : passwordList)
        {
            file << pass->toCSV();
        }
        file.close();
        cout << "Passwords have been saved." << endl;
    }

    void loadFromFile()
    {
        ifstream file(filename);
        if (!(file.is_open()))
        {
            cout << "No password file found, starting fresh." << endl;
            return;
        }

        string line;
        while (getline(file, line))
        {
            stringstream ss(line);
            string type, user, pass, extra;

            getline(ss, type, ',');
            getline(ss, user, ',');
            getline(ss, pass, ',');
            getline(ss, extra, ',');

            if (type == "Website")
            {
                passwordList.push_back(make_shared<WebsitePassword>(user, pass, extra));
            }
            else if (type == "App")
            {
                passwordList.push_back(make_shared<AppPassword>(user, pass, extra));
            }
        }
        file.close();
        cout << "Passwords loaded from " << filename << endl;
    }

    void searchPassword() const
    {
        string email, identifier;

        cout << "Enter email: ";
        cin >> email;
        cout << "Enter app name or URL: ";
        cin >> identifier;

        auto pass = findPassword(email, identifier);
        if (pass)
        {
            pass->display();
            return;
        }
        cout << "No matching password found." << endl;
    }

    void update()
    {
        string email, identifier, newPass;

        cout << "Enter email: ";
        cin >> email;
        cout << "Enter app name or URL: ";
        cin >> identifier;
        cout << "Enter new password: ";
        cin >> newPass;

        auto pass = findPassword(email, identifier);
        if (pass)
        {
            pass->setPassword(newPass);
            cout << "Password was updated successfully." << endl;
            return;
        }

        cout << "No matching password was found." << endl;
    }

    void deletePassword()
    {
        string email, identifier;

        cout << "Enter email: ";
        cin >> email;
        cout << "Enter app name or URL: ";
        cin >> identifier;
        auto pass = findPassword(email, identifier);
        if (pass)
        {
            passwordList.erase(remove(passwordList.begin(), passwordList.end(), pass), passwordList.end());
            saveToFile();
            cout << "Password was deleted successfully." << endl;
            return;
        }
        cout << "No matching password found." << endl;
    }
};

int main()
{
    PasswordManager manager;

    while (true)
    {
        int choice;
        cout << "\nEnter choice\n\n";
        cout << "1. Enter Password\n2. Search Password\n3. Update Password\n4. Delete Password\nPress any other key to exit\n";
        cin >> choice;
        string email, password, type, extra;
        switch (choice)
        {
        case 1:
            cout << "Enter email: ";
            cin >> email;
            cout << "Enter password: ";
            cin >> password;
            cout << "Enter type (Website/App): ";
            cin >> type;
            cout << "Enter URL/App name: ";
            cin >> extra;

            if (type == "Website" || type == "website")
            {
                manager.addPassword(make_shared<WebsitePassword>(email, password, extra));
            }
            else if (type == "App" || type == "app")
            {
                manager.addPassword(make_shared<AppPassword>(email, password, extra));
            }
            else
            {
                cout << "Invalid type." << endl;
            }
            break;
        case 2:
            manager.searchPassword();
            break;
        case 3:
            manager.update();
            break;
        case 4:
            manager.deletePassword();
            break;
        default:
            cout << "Program is being terminated....";
            exit(0);
        }
    }

    return 0;
}
