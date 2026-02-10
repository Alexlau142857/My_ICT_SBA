// Compile with C++14 standard
// g++ main.cpp -std=c++14 -o app
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <limits>
#include <algorithm>
#include <cctype>
#include <ctime>
#include <iomanip>
#include <cstdlib>
#include <cmath>
#include <cstdio>
using ll = long long;

#ifdef _WIN32
    #include <windows.h>
    #include <conio.h>
    #define KEY_BACKSPACE 8
    #define KEY_ENTER 13
    #define KEY_TAB 9
#else
    #include <unistd.h>
    #include <termios.h>
    #define KEY_BACKSPACE 127
    #define KEY_ENTER 10
    #define KEY_TAB 9
    int _getch() {
        struct termios oldt, newt;
        int ch;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return ch;
    }
#endif

const int MAX_LOGIN_ATTEMPTS = 5;
const int LOCKOUT_DURATION = 30;

class account
{
public:
    std::string id;
    std::string pw;
    account() : id(""), pw("") {}
    account(std::string x, std::string y) : id(x), pw(y) {}
};

std::map<std::string, account> accounts;
std::map<std::string, int> failed_counts;
std::map<std::string, time_t> lockout_time;
account current_user;
ll state = 0;
bool is_admin = false;
const std::string database_file = "account.txt";

void clrscr()
{
#ifdef _WIN32
    system("cls");
#else
    std::cout << "\033[2J\033[1;1H";
#endif
}

void pause1()
{
    std::cout << "\n> Press <Enter> to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (std::cin.gcount() == 0)
        std::cin.get();
}

std::string get_input(const std::string &prompt)
{
    std::cout << prompt;
    std::string line;
    std::getline(std::cin, line);
    return line;
}

std::string get_masked_input(const std::string &prompt)
{
    std::cout << prompt;
    std::string password = "";
    bool show_password = false;
    int ch;
    while (true)
    {
        ch = _getch();
        if (ch == KEY_ENTER)
        {
            std::cout << std::endl;
            break;
        }
        else if (ch == KEY_TAB)
        {
            show_password = !show_password;
            std::cout << "\r" << std::string(80, ' ') << "\r";
            std::cout << prompt;
            if (show_password)
            {
                std::cout << password;
            }
            else
            {
                std::cout << std::string(password.length(), '*');
            }
        }
        else if (ch == KEY_BACKSPACE)
        {
            if (!password.empty())
            {
                password.pop_back();
                std::cout << "\b \b";
                std::cout << "\r" << std::string(80, ' ') << "\r" << prompt;
                if (show_password)
                    std::cout << password;
                else
                    std::cout << std::string(password.length(), '*');
            }
        }
        else if (std::isprint(ch))
        {
            password += (char)ch;
            if (show_password)
            {
                std::cout << (char)ch;
            }
            else
            {
                std::cout << '*';
            }
        }
    }
    return password;
}

void save_data()
{
    std::ofstream fout(database_file);
    if (!fout.is_open())
        return;
    for (const auto& entry: accounts)
    {
        fout << entry.first << " " << entry.second.pw << std::endl;
    }
    fout.close();
}

bool valid_id(const std::string &id)
{
    if (id.empty())
        return false;
    for (unsigned char ch : id)
    {
        if (std::isspace(ch))
            return false;
    }
    return true;
}

bool valid_pass(const std::string &pw)
{
    if (pw.length() < 8 || pw.length() > 16)
        return false;
    bool up = false, low = false, num = false;
    for (unsigned char ch : pw)
    {
        if (std::isupper(ch))
            up = true;
        else if (std::islower(ch))
            low = true;
        else if (std::isdigit(ch))
            num = true;
    }
    return up && low && num;
}

bool is_account_locked(const std::string &id)
{
    if (failed_counts[id] >= MAX_LOGIN_ATTEMPTS)
    {
        time_t now = std::time(nullptr);
        double seconds_passed = std::difftime(now, lockout_time[id]);
        if (seconds_passed < LOCKOUT_DURATION)
        {
            std::cout << ">> [LOCKED] Too many failed attempts. Try again in "
                      << (LOCKOUT_DURATION - (int)seconds_passed) << " seconds.\n";
            return true;
        }
        else
        {
            failed_counts[id] = 0;
            return false;
        }
    }
    return false;
}

void handle_login()
{
    std::cout << "=== LOGIN ===" << std::endl;
    std::string input_id = get_input("User ID: ");
    if (input_id != "admin" && is_account_locked(input_id))
    {
        return;
    }
    std::cout << "(Press 'Tab' to show/hide password)\n";
    std::string input_pw = get_masked_input("Password: ");
    if (input_id == "admin" && input_pw == "admin")
    {
        current_user = account("admin", "admin");
        is_admin = true;
        state = 1;
        std::cout << ">> Login succeeded as Administrator!" << std::endl;
        return;
    }

    auto it = accounts.find(input_id);
    if (it != accounts.end() && it->second.pw == input_pw)
    {
        current_user = it->second;
        is_admin = false;
        state = 1;
        failed_counts[input_id] = 0;
        std::cout << ">> Login succeeded! Welcome, " << current_user.id << "." << std::endl;
    }
    else
    {
        std::cout << ">> Login failed! Invalid ID or Password." << std::endl;
        if (it != accounts.end())
        {
            failed_counts[input_id]++;
            lockout_time[input_id] = std::time(nullptr);
            int remaining = MAX_LOGIN_ATTEMPTS - failed_counts[input_id];
            if (remaining > 0)
                std::cout << "   (Warning: " << remaining << " attempts remaining before lockout)\n";
            else
                std::cout << "   (Account locked for " << LOCKOUT_DURATION << " seconds)\n";
        }
    }
}

void handle_register()
{
    std::cout << "=== REGISTER ===" << std::endl;
    std::string new_id = get_input("User ID: ");
    if (!valid_id(new_id))
    {
        std::cout << ">> Invalid ID! Cannot be empty or contain spaces." << std::endl;
        return;
    }
    if (accounts.count(new_id) > 0)
    {
        std::cout << ">> User ID already exists!" << std::endl;
        return;
    }
    std::string new_pw;
    while (true)
    {
        std::cout << "Password Requirements:\n - 8 to 16 characters\n - At least 1 Uppercase, 1 Lowercase, 1 Digit\n";
        std::cout << "(Press 'Tab' to show/hide password)\n";
        new_pw = get_masked_input("Enter Password: ");
        if (valid_pass(new_pw))
            break;
        std::cout << ">> Password too weak. Please try again.\n"
                  << std::endl;
    }
    account new_acc(new_id, new_pw);
    accounts[new_id] = new_acc;
    save_data();
    current_user = new_acc;
    state = 1;
    is_admin = false;
    std::cout << ">> Registration successful! You are now logged in." << std::endl;
}

void handle_change_password()
{
    if (is_admin && current_user.id == "admin")
    {
        std::cout << "Admin password can not be changed for security reasons." << std::endl;
        return;
    }
    std::cout << "=== CHANGE PASSWORD ===" << std::endl;
    std::string old_pw = get_masked_input("Enter Old Password: ");
    if (old_pw != current_user.pw)
    {
        std::cout << ">> Incorrect old password!" << std::endl;
        return;
    }
    std::string new_pw;
    while (true)
    {
        std::cout << "\nNew Password Requirements:\n - 8 to 16 characters\n - At least 1 Uppercase, 1 Lowercase, 1 Digit\n";
        new_pw = get_masked_input("Enter New Password: ");
        if (valid_pass(new_pw))
            break;
        std::cout << ">> Password too weak." << std::endl;
    }
    current_user.pw = new_pw;
    accounts[current_user.id].pw = new_pw;
    save_data();
    std::cout << ">> Password changed successfully!" << std::endl;
}

void handle_logout()
{
    state = 0;
    is_admin = false;
    current_user = account();
    std::cout << ">> Logout succeeded." << std::endl;
}

void admin_view_all()
{
    std::cout << "--- All Accounts ---" << std::endl;
    if (accounts.empty())
    {
        std::cout << "(Database is empty)" << std::endl;
        return;
    }
    for (const auto& entry : accounts)
    {
        std::cout << "User: " << entry.first << " | Pass: " << entry.second.pw << std::endl;
    }
}

void admin_import_file()
{
    std::string filename = get_input("Enter filename to import: ");
    std::ifstream fin(filename);
    if (!fin.is_open())
    {
        std::cout << ">> Failed to open file: " << filename << std::endl;
        return;
    }
    std::string fid, fpw;
    ll count = 0;
    while (fin >> fid >> fpw)
    {
        if (accounts.count(fid) == 0)
        {
            if (valid_id(fid) && valid_pass(fpw))
            {
                accounts[fid] = account(fid, fpw);
                count++;
            }
        }
    }
    fin.close();
    if (count > 0)
        save_data();
    std::cout << ">> Imported " << count << " new accounts." << std::endl;
}

void admin_export_file()
{
    std::ofstream fout("list_all_account.txt");
    fout << "List of all accounts:" << std::endl;
    for (const auto& entry : accounts)
    {
        fout << "User ID: " << entry.first << ", Password: " << entry.second.pw << std::endl;
    }
    fout.close();
    std::cout << ">> Exported to 'list_all_account.txt'." << std::endl;
}

void admin_delete_user()
{
    std::string del_id = get_input("Enter User ID to delete: ");
    if (accounts.erase(del_id))
    {
        save_data();
        std::cout << ">> Account " << del_id << " deleted." << std::endl;
        failed_counts.erase(del_id);
        lockout_time.erase(del_id);
    }
    else
    {
        std::cout << ">> Account not found." << std::endl;
    }
}

void process()
{
    clrscr();
    std::string choice_str;
    ll choice = -1;
    if (state == 0)
    {
        std::cout << "=== MAIN MENU ===\n";
        std::cout << "1. Login\n";
        std::cout << "2. Register\n";
        std::cout << "3. Quit\n";
    }
    else if (state == 1)
    {
        std::cout << "=== USER MENU (" << current_user.id << ") ===\n";
        std::cout << "1. Logout\n";
        std::cout << "2. Change Password\n";
        if (is_admin)
        {
            std::cout << "\n[Admin Controls]\n";
            std::cout << "1001. View all accounts\n";
            std::cout << "1002. Import accounts from file\n";
            std::cout << "1003. Export accounts to file\n";
            std::cout << "1004. Delete an account\n";
        }
    }
    choice_str = get_input("\nEnter choice: ");
    try
    {
        if (!choice_str.empty())
            choice = std::stoll(choice_str);
    }
    catch (...)
    {
        choice = -1;
    }

    if (state == 0)
    {
        switch (choice)
        {
        case 1:
            handle_login();
            pause1();
            break;
        case 2:
            handle_register();
            pause1();
            break;
        case 3:
            state = -1;
            break;
        default:
            std::cout << "Invalid choice." << std::endl;
            pause1();
            break;
        }
    }
    else if (state == 1)
    {
        if (choice == 1)
        {
            handle_logout();
            pause1();
        }
        else if (choice == 2)
        {
            handle_change_password();
            pause1();
        }
        else if (is_admin)
        {
            switch (choice)
            {
            case 1001:
                admin_view_all();
                pause1();
                break;
            case 1002:
                admin_import_file();
                pause1();
                break;
            case 1003:
                admin_export_file();
                pause1();
                break;
            case 1004:
                admin_delete_user();
                pause1();
                break;
            default:
                std::cout << "Invalid admin choice." << std::endl;
                pause1();
                break;
            }
        }
        else
        {
            std::cout << "Invalid choice." << std::endl;
            pause1();
        }
    }
}

int main()
{
    std::setlocale(LC_ALL, ""); // use UTF-8
    std::ifstream fin(database_file);
    if (fin.is_open())
    {
        std::string fid, fpw;
        fin >> fid >> fpw;
        while (fin >> fid >> fpw)
        {
            accounts[fid] = account(fid, fpw);
        }
        fin.close();
    }
    state = 0;
    while (state >= 0)
    {
        process();
    }
    return 0;
}