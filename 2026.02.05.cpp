// Run this code with C++20 standard
#include <cassert>
#include <cctype>
#include <cerrno>
#include <cfloat>
#include <ciso646>
#include <climits>
#include <clocale>
#include <cmath>
#include <csetjmp>
#include <csignal>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cwchar>
#include <cwctype>
#include <ccomplex>
#include <cfenv>
#include <cinttypes>
#include <cstdbool>
#include <cstdint>
#include <tgmath.h>
#include <algorithm>
#include <bitset>
#include <complex>
#include <deque>
#include <exception>
#include <fstream>
#include <functional>
#include <iomanip>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <istream>
#include <iterator>
#include <limits>
#include <list>
#include <locale>
#include <map>
#include <memory>
#include <new>
#include <numeric>
#include <ostream>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <streambuf>
#include <string>
#include <typeinfo>
#include <utility>
#include <valarray>
#include <vector>
#include <array>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <forward_list>
#include <future>
#include <initializer_list>
#include <mutex>
#include <random>
#include <ratio>
#include <regex>
#include <scoped_allocator>
#include <system_error>
#include <thread>
#include <tuple>
#include <typeindex>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <compare>
#include <concepts>
#include <ranges>
using ll = long long;
using ld = long double;
//using i128 = __int128;
//using namespace std;

class account {
    public:
        std::string id, pw;
        account(){
            id = "";
            pw = "";
        }
        account(std::string x, std::string y){
            id = x;
            pw = y;
        }
        auto operator<=>(const Account& other) const {
            return id <=> other.id;
        }
        bool operator==(const Account& other) const {
            return id == other.id;
        }
};

ll state;
account user;
std::vector<account> A;
bool is_admin = false;
bool flag = false;

bool is_valid_id(const std::string &id) {
    if (id.empty()) return false;
    for (unsigned char ch : id) {
        if (std::isspace(ch)) {
            return false;
        }
    }return true;
}

bool is_valid_password(const std::string &pw) {
    std::size_t n = pw.length();
    if (n < 8 || n > 16) return false;
    bool up = false, low = false, num = false;
    for (unsigned char ch : pw) {
        if (std::isupper(ch)) up = true;
        else if (std::islower(ch)) low = true;
        else if (std::isdigit(ch)) num = true;
    }
    return up && low && num;
}

bool account_exists_in_file(const std::string &filename, const std::string &id) {
    std::ifstream fin(filename);
    std::string line;
    while (std::getline(fin, line)) {
        if (line.find(id) != std::string::npos) return true;
    }return false;
}

void clrscr() {
    std::cout << "\033[2J\033[1;1H";
    return;
}

void menu(ll &choice) {
    clrscr();
    std::cout << "Please choose from the following:" << std::endl;
    if (state == 0) {
        std::cout << "1. Login" << std::endl;
        std::cout << "2. Register" << std::endl;
        std::cout << "3. Quit" << std::endl;
    } else if (state == 1) {
        std::cout << "1. Logout" << std::endl;
        if (is_admin) {
            std::cout << "1002. View all accounts" << std::endl;
            std::cout << "1003. Output all the accounts id and password to a file named account.txt";
        }
    }
    std::cout << "Enter your choice: ";
    std::string temp;
    std::getline(std::cin,temp);
    if (temp.empty() || !std::isdigit(static_cast<unsigned char>(temp[0]))) {
        choice = -1; // invalid choice
    } else {
        choice = temp[0] - '0';
    }
    return;
}

void login(account &user, bool &flag) {
    std::cout << "Please enter your user id and password:" << std::endl;
    std::cout << "User ID: "; std::getline(std::cin,user.id);
    std::cout << "Password: "; std::getline(std::cin,user.pw);
    flag = false;
    for (ll i = 0; i < (ll)A.size(); ++i) {
        if (A[i].id == user.id && A[i].pw == user.pw) {
            flag = true;
            break;
        }
    }
    if (user.id == "admin" and user.pw == "admin") {
        flag = true;
        is_admin = true;
    }
    if (flag){
        std::cout << "Login succeeded! You will be directed to the main menu." << std::endl;
    } else {
        std::cout << "Login failed! You will be directed to the front page." << std::endl;
        if (account_exists_in_file("account.txt", user.id)) {
            std::cout << "Note: the user id exists in account.txt but the password did not match." << std::endl;
        }
    }
    return;
}

void insert(const account &new_account) {
    A.push_back(new_account);
    ll idx = 0;
    ll l = 0, r = (ll)A.size() - 1;
    while (l <= r) {
        ll mid = (l + r) / 2;
        if (A[mid].id == new_account.id) {
            idx = mid;
            break;
        } else if (A[mid].id < new_account.id) {
            l = mid + 1;
        } else {
            r = mid - 1;
        }
    } if (l > r) {
        idx = l;
    } for (ll i = (ll)A.size() - 1; i > idx; --i) {
        A[i] = A[i - 1];
    } A[idx] = new_account;
    /*
    stable_sort(A.begin(), A.end(), [](const account &a, const account &b) {
        return a.id < b.id;
    });
    */
}

void create(account &user, bool &flag) {
    std::cout << "User ID: \nPlease input your user ID: "; std::getline(std::cin,user.id);
    std::cout << "Password: \nShould be contain: \n - at least one uppercase letter, \n - one lowercase letter, \n - one digit, \n - length between 8 and 16\n> Please input your password: "; std::getline(std::cin,user.pw);
    if (!is_valid_id(user.id)) {
        std::cout << "The user ID is invalid (empty or contains spaces)! You will be directed to the front page." << std::endl;
        flag = false;
        return;
    }
    ll l = 0, r = (ll)A.size() - 1;
    bool found = false;
    while (l <= r) {
        ll mid = (l + r) / 2;
        if (A[mid].id == user.id) {
            found = true;
            break;
        } else if (A[mid].id < user.id) {
            l = mid + 1;
        } else {
            r = mid - 1;
        }
    }
    if (found) {
        std::cout << "The user ID is already in use! You will be directed to the front page." << std::endl;
        flag = false;
        return;
        }
    }
    if (!is_valid_password(user.pw)) {
        std::cout << "Register failed, your password is weak! Please enter your password again." << std::endl;
        while (true) {
            std::cout << "Password: "; std::getline(std::cin, user.pw);
            if (is_valid_password(user.pw)) break;
            std::cout << "Password still weak. It must be 8-16 chars with upper, lower, and digit." << std::endl;
        }
    }
    insert(account(user.id, user.pw));
    std::cout << "Register succeeded! Login successfully. \nYou will be directed to the main menu. " << std::endl;
    flag = true;
    return;
}

void logout() {
    is_admin = false;
    flag = false;
    std::cout << "Logout succeeded! You will be directed to the front page." << std::endl;
}

void process(ll choice) {
    clrscr();
    if (state == 0 and choice == 1) {
        login(user, flag);
        if (flag)
            state = 1;
    } else if (state == 0 and choice == 2) {
        bool flag;
        create(user,flag);
        if (flag)
            state = 1;
    } else if (state == 0 and choice == 3) {
        state = -1;
        return;
    } else if (state == 1 and choice == 1) {
        logout();
        state = 0;
    } else if (state == 1 and choice == 1002 and is_admin) {
        std::cout << "List of all accounts:" << std::endl;
        for (const auto &acc : A) {
            std::cout << "User ID: " << acc.id << ", Password: " << acc.pw << std::endl;
        }
    } else if (state == 1 and choice == 1003 and is_admin) {
        std::ofstream fout("account.txt");
        fout << "List of all accounts:" << std::endl;
        for (const auto &acc : A) {
            fout << "User ID: " << acc.id << ", Password: " << acc.pw << std::endl;
        }
        fout.close();
        std::cout << "All accounts have been output to account.txt" << std::endl;
    } else {
        std::cout << "Invalid choice! You will be directed to the main menu." << std::endl;
    }
    std::cout << "Press <Enter> to continue";
    std::string temp;
    std::getline(std::cin,temp);
    return;
}

int main() {
    insert(account("admin", "admin"));
    insert(account("lkkc001", "band1A"));
    insert(account("lkkc002", "band1B"));
    insert(account("mhp003", "band1C"));
    state = 0;
    ll choice;
    while (state >= 0) {
        menu(choice);
        process(choice);
    }
    return 0;
}
