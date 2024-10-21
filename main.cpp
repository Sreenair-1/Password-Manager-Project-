#include <iostream>
#include <conio.h>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>  
#include <algorithm>    

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
        password = newPass;
        return password;
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
        cout << "Website: " << url << "\nemail: " << email << "\nPassword: " << password << "\n";
    }

    string toCSV() const override
    {
        return "Website," + email + "," + url + "," + password + "\n";
    }

    bool isUrl(const string &link) const
    {
        return url == link;
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
        cout << "\n\tApp: " << appName << "\n\temail: " << email << "\n\tPassword: " << password << "\n";
    }

    string toCSV() const override
    {
        return "App," + email + "," + appName + "," + password + "\n";
    }

    bool isAppName(const string &app) const
    {
        return appName == app;
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
            cout << "Error: Could not open file for writing." << "\n";
            return;
        }
        for (const auto &pass : passwordList)
        {
            file << pass->toCSV();
        }
        file.close();
        cout << "Passwords have been saved." << "\n";
    }

    void loadFromFile()
    {
        ifstream file(filename);
        if (!(file.is_open()))
        {
            cout << "No password file found, starting fresh." << "\n";
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
        cout << "Passwords loaded from " << filename << "\n";
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
        cout << "No matching password found." << "\n";
    }

    void update()
    {
        string email, identifier;

        cout << "Enter email: ";
        cin >> email;
        cout << "Enter app name or URL: ";
        cin >> identifier;

        auto pass = findPassword(email, identifier);
        if (pass)
        {
            string newPass;
            cout << "Enter new password(NO COMMAS): ";
            cin >> newPass;
            pass->setPassword(newPass);
            saveToFile();
            cout << "Password was updated successfully." << "\n";
            return;
        }

        cout << "No matching password was found." << "\n";
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
            cout << "\nPassword was deleted successfully." << "\n";
            return;
        }
        cout << "\nNo matching password found." << "\n";
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
            cout << "Enter type (Website/App): ";
            cin >> type;
            cout << "Enter URL/App name: ";
            cin >> extra;
            cout << "Enter password: ";
            cin >> password;

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
                cout << "Invalid type." << "\n";
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
            return 0;
        }
        getch();
    }
    return 0;
}
