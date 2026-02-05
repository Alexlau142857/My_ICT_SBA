#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <time.h>
#include <vector>
using namespace std;
using ll = long long;
using ld = long double;
using i128 = __int128;

class account {
    public:
        string id, pw;    
        account(){
            id = "";
            pw = "";
        }
        account(string x, string y){
            id = x;
            pw = y;
        }
};

ll state;
account user;
vector<account> A;

void clrscr() {
    cout << "\033[2J\033[1;1H";
    return;
}

void menu(ll &choice) {
    clrscr();
    cout << "Please choose from the following:" << endl;
    if (state == 0) {
        cout << "1. Login" << endl;
        cout << "2. Register" << endl;
        cout << "3. Quit" << endl;
    } else if (state == 1) {
        cout << "1. Logout" << endl;
    }
    cout << "Enter your choice: ";
    string temp;
    getline(cin,temp);
    if (temp.empty() || !isdigit(static_cast<unsigned char>(temp[0]))) {
        choice = -1; // invalid choice
    } else {
        choice = temp[0] - '0';
    }
    return;
}

void login(account &user, bool &flag) {
    cout << "Please enter your user id and password:" << endl;
    cout << "User ID: "; getline(cin,user.id);
    cout << "Password: "; getline(cin,user.pw);
    flag = false;
    for (ll i = 0; i < (ll)A.size(); ++i) {
        if (A[i].id == user.id && A[i].pw == user.pw) {
            flag = true;
            break;
        }
    }
    if (user.id == "admin" and user.pw == "admin") {
        flag = true;
    }
    if (flag){
        cout << "Login succeeded! You will be directed to the main menu." << endl;
    } else {
        cout << "Login failed! You will be directed to the front page." << endl;
    }
    return;
}

void insert(const account &new_account) {
    A.push_back(new_account);
    stable_sort(A.begin(), A.end(), [](const account &a, const account &b) {
        return a.id < b.id;
    });
}

void create(account &user, bool &flag) {
    cout << "Please enter your user id and password:" << endl;
    cout << "User ID: "; getline(cin,user.id);
    cout << "Password: "; getline(cin,user.pw);
    if (user.id.empty()) {
        cout << "The user ID cannot be empty! You will be directed to the front page." << endl;
        flag = false;
        return;
    }
    for (ll i = 0; i < (ll)A.size(); i++){
        if (A[i].id == user.id) {
            cout << "The user ID is already in use! You will be directed to the front page." << endl;
            flag = false;
            return;
        }
    }
    bool up = false, low = false, num = false, leng = false;
    if (user.pw.length() <= 16 && user.pw.length() >= 8){
        leng = true;
    }
    for (char ch : user.pw) {
        if (!up && (ch >= 'A' && ch <= 'Z'))
            up = true;
        if (!low && (ch >= 'a' && ch <= 'z'))
            low = true;
        if (!num && (ch >= '0' && ch <= '9'))
            num = true;
    }
    if (up && low && num && leng){
        insert(account(user.id, user.pw));
        cout << "Register succeeded! You will be directed to the main menu." << endl;
        flag = true;
    } else{
        cout << "Register failed, your password is weak! You will be directed to the main menu." << endl;
        flag = false;
    }
    return;
}

void logout() {
    cout << "Logout succeeded! You will be directed to the front page." << endl;
}

void process(ll choice) {
    clrscr();
    if (state == 0 and choice == 1) {
        bool flag;
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
    }
    cout << "Press <Enter> to continue";
    string temp;
    getline(cin,temp);
    return;
}

int main() {
    A.push_back(account("lkkc001", "band1A"));
    A.push_back(account("lkkc002", "band1B"));
    A.push_back(account("mhp003", "band1C"));
    state = 0;
    ll choice;
    while (state >= 0) {
        menu(choice);
        process(choice);
    }    
    
    return 0;
}
