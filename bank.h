#ifndef BANK_H
#define BANK_H

#include <iostream>
#include <string>
#include <vector>
#include <sstream> 
#include <pthread.h>
#include <unistd.h> 
#include <fcntl.h>
#include <bits/stdc++.h>

#define RD_BUF_SIZE 4096

#define RD_SUCCESS 0
#define RD_ERR -1
#define RD_EOF -2

#define ONE_SECOND_DELAY 1000000
#define HUNDRED_MILI_DELAY 100000


struct atmThreadArgs {
    std::string inputFileName;
    int atmNum;
};

struct atmCommandArgs{
    std::string cmd;
    int accoutNum;
    int password;
    int amount;
    int targetAccountNum;
};

class bankAccount {
private:
    int accountNumber;
    int password;
    int balance;
    pthread_mutex_t account_mutex;
public:
    bankAccount() = default;
    bankAccount(int accNum, int accPwd, int accBln);
    //~bankAccount() = default;
    ~bankAccount();
    int deposit(int amount);
    int withdraw(int amount);
    int getBalance();
    int getPassword();
    void lockMutex();
    void unlockMutex();
};


int rdLine (int fileDescriptor, int atmNum, std::string* destString);
int cmdParser (std::string cmd, atmCommandArgs* cmdArgs);
void *atmRoutine (void *args);

#endif