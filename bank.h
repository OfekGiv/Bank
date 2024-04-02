#ifndef BANK_H
#define BANK_H

#include <iostream>
#include <string>
#include <vector>
#include <sstream> 
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h> 
#include <fcntl.h>
#include <bits/stdc++.h>

#define RD_BUF_SIZE 4096

#define RD_SUCCESS 0
#define RD_ERR -1
#define RD_EOF -2

#define ONE_SECOND_DELAY 1000000
#define THREE_SECOND_DELAY 3000000
#define HUNDRED_MILI_DELAY 100000


struct atmThreadArgs {
    std::string inputFileName;
    int atmNum;
};

struct atmCommandArgs{
    std::string cmd;
    std::string accoutNum;
    std::string password;
    int amount;
    std::string targetAccountNum;
};

class bankAccount {
private:
    std::string accountNumber;
    std::string password;
    int balance;
    pthread_mutex_t account_mutex;
public:
    bankAccount() = default;
    bankAccount(std::string accNum, std::string accPwd, int accBln);
    //~bankAccount() = default;
    ~bankAccount();
    int deposit(int amount);
    int withdraw(int amount);
    int getBalance();
    std::string getPassword();
    void lockMutex();
    void unlockMutex();
};


int rdLine (int fileDescriptor, int atmNum, std::string* destString);
int cmdParser (std::string cmd, atmCommandArgs* cmdArgs);
void *atmRoutine (void *args);
void *bankRoutine (void *args);

#endif