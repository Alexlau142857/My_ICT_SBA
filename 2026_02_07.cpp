// Run this code with C++20 (C17) standard
#include <cassert>
#include <cctype>
#include <cerrno>
#include <cfenv>
#include <cfloat>
#include <cinttypes>
#include <climits>
#include <clocale>
#include <cmath>
#include <csetjmp>
#include <csignal>
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cuchar>
#include <cwchar>
#include <cwctype>
#include <charconv>
#include <algorithm>
#include <any>
#include <array>
#include <atomic>
#include <barrier>
#include <bit>
#include <bitset>
#include <chrono>
#include <concepts>
#include <compare>
#include <coroutine>
#include <deque>
#include <exception>
#include <execution>
#include <filesystem>
#include <forward_list>
#include <fstream>
#include <functional>
#include <future>
#include <initializer_list>
#include <iomanip>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <istream>
#include <iterator>
#include <latch>
#include <limits>
#include <list>
#include <locale>
#include <map>
#include <memory>
#include <memory_resource>
#include <mutex>
#include <new>
#include <numbers>
#include <numeric>
#include <optional>
#include <ostream>
#include <queue>
#include <random>
#include <ranges>
#include <ratio>
#include <regex>
#include <scoped_allocator>
#include <set>
#include <shared_mutex>
#include <span>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <stop_token>
#include <streambuf>
#include <string>
#include <string_view>
#include <syncstream>
#include <system_error>
#include <thread>
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <valarray>
#include <variant>
#include <vector>
#include <version>
using ll = long long;
// using ld = long double; using i128 = __ll128;
// using namespace std;

class account
{
public:
    std::string id;
    std::string pw;
    account() : id(""), pw("") {}
    account(std::string x, std::string y) : id(x), pw(y) {}
    auto operator<=>(const account &other) const
    {
        return id <=> other.id;
    }
    bool operator==(const account &other) const
    {
        return id == other.id;
    }
};

std::map<std::string, account> accounts;
account current_user;
ll state = 0;
bool is_admin = false;
const std::string database_file = "account.txt";

void clrscr()
{
    std::cout << "\033[2J\033[1;1H";
}

void pause()
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

void save_data()
{
    std::ofstream fout(database_file);
    if (!fout.is_open())
        return;

    for (const auto &[id, acc] : accounts)
    {
        fout << acc.id << " " << acc.pw << std::endl;
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

void handle_login()
{
    std::cout << "=== LOGIN ===" << std::endl;
    std::string input_id = get_input("User ID: ");
    std::string input_pw = get_input("Password: ");
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
        std::cout << ">> Login succeeded! Welcome, " << current_user.id << "." << std::endl;
    }
    else
    {
        std::cout << ">> Login failed! Invalid ID or Password." << std::endl;
        if (accounts.find(input_id) != accounts.end())
        {
            std::cout << "   (Hll: User ID exists, but password was incorrect.)" << std::endl;
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
    if (accounts.contains(new_id))
    {
        std::cout << ">> User ID already exists!" << std::endl;
        return;
    }

    std::string new_pw;
    while (true)
    {
        std::cout << "Password Requirements:\n - 8 to 16 characters\n - At least 1 Uppercase, 1 Lowercase, 1 Digit\n";
        new_pw = get_input("Enter Password: ");

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
    for (const auto &[id, acc] : accounts)
    {
        std::cout << "User: " << id << " | Pass: " << acc.pw << std::endl;
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
        if (!accounts.contains(fid))
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
    for (const auto &[id, acc] : accounts)
    {
        fout << "User ID: " << id << ", Password: " << acc.pw << std::endl;
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
        ll choice = -1; // Initialize choice to -1
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
        // Parse without exceptions using C API `strtoll` (errno-based)
        char *endptr = nullptr;
        errno = 0;
        long long tmp = std::strtoll(choice_str.c_str(), &endptr, 10);
        // Valid when at least one character was consumed, entire string consumed, and no range/parse error
        if (endptr != choice_str.c_str() && *endptr == '\0' && errno == 0)
        {
            choice = tmp;
        }
        else
        {
            choice = -1; // Set choice to -1 if parsing failed
        }
    if (state == 0)
    {
        switch (choice)
        {
        case 1:
            handle_login();
            pause();
            break;
        case 2:
            handle_register();
            pause();
            break;
        case 3:
            state = -1;
            break;
        default:
            std::cout << "Invalid choice." << std::endl;
            pause();
            break;
        }
    }
    else if (state == 1)
    {
        if (choice == 1)
        {
            handle_logout();
            pause();
        }
        else if (is_admin)
        {
            switch (choice)
            {
            case 1001:
                admin_view_all();
                pause();
                break;
            case 1002:
                admin_import_file();
                pause();
                break;
            case 1003:
                admin_export_file();
                pause();
                break;
            case 1004:
                admin_delete_user();
                pause();
                break;
            default:
                std::cout << "Invalid admin choice." << std::endl;
                pause();
                break;
            }
        }
        else
        {
            std::cout << "Invalid choice." << std::endl;
            pause();
        }
    }
}

int main()
{
    std::ifstream fin(database_file);
    if (fin.is_open())
    {
        std::string fid, fpw;
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
