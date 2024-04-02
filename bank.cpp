
#include "bank.h"

extern std::map<std::string,bankAccount> accounts;

/***************** Bank account class functions ******************/

bankAccount::bankAccount(std::string accNum, std::string accPwd, int accBln)
    : accountNumber(accNum), password(accPwd), balance(accBln) 
{
    pthread_mutex_init(&account_mutex,NULL);
}

bankAccount::~bankAccount()
{
    pthread_mutex_destroy(&account_mutex);
}

int bankAccount::deposit(int amount)
{
    balance += amount;
    return balance;
}

int bankAccount::withdraw(int amount)
{
    if (amount > balance)
    {
        return -1;
    }
    else
    {
        balance -= amount;
        return balance;
    }
}

int bankAccount::getBalance()
{
    return balance;
}

std::string bankAccount::getPassword()
{
    return password;
}

void bankAccount::lockMutex()
{
    pthread_mutex_lock(&account_mutex);
}

void bankAccount::unlockMutex()
{
    pthread_mutex_unlock(&account_mutex);
}

/************ Bank basic functions **************/

int rdLine (int fileDescriptor, int atmNum, std::string* destString)
{
    char buffer[RD_BUF_SIZE];
    int rdSize;
    int bytesRead = 0;

    while (rdSize = read(fileDescriptor,buffer+bytesRead,1) > 0)
    {
        if (buffer[bytesRead] == '\n') // line break encountered
        {
            buffer[bytesRead] = '\0';
            *destString = buffer;
            return RD_SUCCESS;
        }
        bytesRead += rdSize;
        if (bytesRead >= RD_BUF_SIZE - 1)
        {
            std::cout << "Bank error: ATM " << atmNum << " buffer overflow" << std::endl;
            return RD_ERR;
        }
    }
    if (rdSize == -1)
    {
        perror("Bank error");
        return RD_ERR;
    }
    else if (rdSize == 0) // EOF
    {
        if (bytesRead != 0)
        {
            buffer[bytesRead+1] = '\0';
            *destString = buffer;
            return RD_SUCCESS;
        }
        else
            return RD_EOF;
    }

}

int cmdParser (std::string cmd, atmCommandArgs* cmdArgs)
{
    std::stringstream ss(cmd);
    std::string token;
    std::vector<std::string> tokens;
    int tokenCount = 0;

    // std::cout << cmd << std::endl;

    while (std::getline(ss,token,' '))
    {
        tokens.push_back(token);
        tokenCount++;
    }

    if (tokenCount == 4)
    {
        
        cmdArgs->cmd = tokens.at(0);
        cmdArgs->accoutNum = tokens.at(1);
        cmdArgs->password = tokens.at(2);
        cmdArgs->amount = std::stoi(tokens.at(3));
    }
    else
    {
        
        cmdArgs->cmd = tokens.at(0);
        cmdArgs->accoutNum = tokens.at(1);
        cmdArgs->password = tokens.at(2);
        cmdArgs->targetAccountNum = tokens.at(3);
        cmdArgs->amount = std::stoi(tokens.at(4));
    }

}
void createBankaccount()
{
    accounts["Bank"] = bankAccount("Bank","0000",0);
}

/********************** ATM thread routine ***************************/

void *atmRoutine (void *args)
{
    atmThreadArgs *atmArgs = (atmThreadArgs*) args;
    std::string cmdString;
    int fd;

    if ((fd=open(atmArgs->inputFileName.c_str(),O_RDONLY)) == -1)
    {
        perror("Bank error");
        pthread_exit(NULL); // TODO: check if return value is NULL
    }

    while(rdLine(fd,atmArgs->atmNum,&cmdString) != RD_EOF)
    {
        
        atmCommandArgs currentCmd;
        cmdParser(cmdString,&currentCmd);
        //std::cout << cmdString << std::endl;
        
        usleep(HUNDRED_MILI_DELAY);

        if (currentCmd.cmd == std::string(1,'O'))  // If commnad is open account 
        {
            usleep(ONE_SECOND_DELAY);
            auto it = accounts.find(currentCmd.accoutNum);
            if (it != accounts.end()) // If account with accoutNum ID exists
            {
                std::cout << "Error " << atmArgs->atmNum << ": Your transaction failed - account with the same id exists" << std::endl;
                //return NULL;
            }
            else
            {
                accounts[currentCmd.accoutNum] = bankAccount(currentCmd.accoutNum,currentCmd.password,currentCmd.amount);
                std::cout << atmArgs->atmNum << ": New account id is " << currentCmd.accoutNum << " with password " << currentCmd.password << " and initial balance " << currentCmd.amount << std::endl;
            }
        }
        else if (currentCmd.cmd == std::string(1,'D')) 
        {
            usleep(ONE_SECOND_DELAY);
            auto it = accounts.find(currentCmd.accoutNum);
            if (it == accounts.end()) // If account with accoutNum ID does not exist
            {
                std::cout << "Error " << atmArgs->atmNum << ": Your transaction failed - account id " << currentCmd.accoutNum << " does not exist" << std::endl;
                //return NULL;
            }
            else if (currentCmd.password != accounts[currentCmd.accoutNum].getPassword()) // Wrong password error
            {
                std::cout << "Error " << atmArgs->atmNum << ": Your transaction failed - password for account id " << currentCmd.accoutNum << " is incorrect" << std::endl;
                //return NULL;
            }
            else
            {
                int bal = accounts[currentCmd.accoutNum].deposit(currentCmd.amount);
                std::cout << atmArgs->atmNum << ": Account " << currentCmd.accoutNum << " new balance is " << bal << " after " << currentCmd.amount << "$ was deposited" << std::endl;
            }
        }
        else if (currentCmd.cmd == std::string(1,'W')) 
        {
            usleep(ONE_SECOND_DELAY);
            auto it = accounts.find(currentCmd.accoutNum);
            if (it == accounts.end()) // If account with accoutNum ID does not exist
            {
                std::cout << "Error " << atmArgs->atmNum << ": Your transaction failed - account id " << currentCmd.accoutNum << " does not exist" << std::endl;
                //return NULL;
            }
            else if (currentCmd.password != accounts[currentCmd.accoutNum].getPassword()) // Wrong password error
            {
                std::cout << "Error " << atmArgs->atmNum << ": Your transaction failed - password for account id " << currentCmd.accoutNum << " is incorrect" << std::endl;
                //return NULL;
            }
            else
            {
                int bal = accounts[currentCmd.accoutNum].withdraw(currentCmd.amount);
                if (bal == -1)
                    std::cout << "Error " << atmArgs->atmNum << ": Your transaction failed - account id " << currentCmd.accoutNum << " balance is lower than " << currentCmd.amount << std::endl;
                else 
                    std::cout << atmArgs->atmNum << ": Account " << currentCmd.accoutNum << " new balance is " << bal << " after " << currentCmd.amount << "$ was withdrew" << std::endl;
            }
        }
        else if (currentCmd.cmd == std::string(1,'B')) 
        {
            usleep(ONE_SECOND_DELAY);
            auto it = accounts.find(currentCmd.accoutNum);
            if (it == accounts.end()) // If account with accoutNum ID does not exist
            {
                std::cout << "Error " << atmArgs->atmNum << ": Your transaction failed - account id " << currentCmd.accoutNum << " does not exist" << std::endl;
                //return NULL;
            }
            else if (currentCmd.password != accounts[currentCmd.accoutNum].getPassword()) // Wrong password error
            {
                std::cout << "Error " << atmArgs->atmNum << ": Your transaction failed - password for account id " << currentCmd.accoutNum << " is incorrect" << std::endl;
                //return NULL;
            }
            else
            {
                int bal = accounts[currentCmd.accoutNum].getBalance();
                std::cout << atmArgs->atmNum << ": Account " << currentCmd.accoutNum << " balance is " << bal << std::endl;
            }
        }
        else if (currentCmd.cmd == std::string(1,'Q')) 
        {
            usleep(ONE_SECOND_DELAY);
            auto it = accounts.find(currentCmd.accoutNum);
            if (it == accounts.end()) // If account with accoutNum ID does not exist
            {
                std::cout << "Error " << atmArgs->atmNum << ": Your transaction failed - account id " << currentCmd.accoutNum << " does not exist" << std::endl;
                //return NULL;
            }
            else if (currentCmd.password != accounts[currentCmd.accoutNum].getPassword()) // Wrong password error
            {
                std::cout << "Error " << atmArgs->atmNum << ": Your transaction failed - password for account id " << currentCmd.accoutNum << " is incorrect" << std::endl;
                //return NULL;
            }
            else
            {
                int bal = accounts[currentCmd.accoutNum].getBalance();
                std::cout << atmArgs->atmNum << ": Account " << currentCmd.accoutNum << " is not closed. balance was " << bal << std::endl;
                accounts.erase(currentCmd.accoutNum);
            }
    
        }
        else if (currentCmd.cmd == std::string(1,'T')) 
        {
            usleep(ONE_SECOND_DELAY);
            std::string accNum;
            auto it1 = accounts.find(currentCmd.accoutNum);
            auto it2 = accounts.find(currentCmd.targetAccountNum);
            if (it1 == accounts.end())
                accNum = currentCmd.accoutNum;
            else if (it2 == accounts.end())
                accNum = currentCmd.targetAccountNum;

            if (!accNum.empty()) // If account with accoutNum ID does not exist
            {
                std::cout << "Error " << atmArgs->atmNum << ": Your transaction failed - account id " << accNum << " does not exist" << std::endl;
                //return NULL;
            }
            else if (currentCmd.password != accounts[currentCmd.accoutNum].getPassword()) // Wrong password error
            {
                std::cout << "Error " << atmArgs->atmNum << ": Your transaction failed - password for account id " << currentCmd.accoutNum << " is incorrect" << std::endl;
                //return NULL;
            }
            else
            {
                int accBalance = accounts[currentCmd.accoutNum].withdraw(currentCmd.amount);
                if (accBalance == -1)
                    std::cout << "Error " << atmArgs->atmNum << ": Your transaction failed - account id " << currentCmd.accoutNum << " balance is lower than " << currentCmd.amount << std::endl;
                else 
                {
                    int targetBalance = accounts[currentCmd.targetAccountNum].deposit(currentCmd.amount);
                    std::cout << atmArgs->atmNum << ": Transfer " << currentCmd.amount << " to account " << currentCmd.targetAccountNum << \
                    " new account balance is " << accBalance << " new target account balance is " \
                    << targetBalance << std::endl;
                }
            }
        }
        
        usleep(HUNDRED_MILI_DELAY);

    }

    if (close(fd) == -1)
    {
        perror("Bank error");
        pthread_exit(NULL); // TODO: check if return value is NULL
    }
    // std::cout << "ATM number " << atmArgs->atmNum << ". File name: " << atmArgs->inputFileName << std::endl;
}


/********************** Bank thread routine ***************************/

void *bankRoutine (void *args)
{
    float commissionPercent;
    int commissionFee;
    int curBalance;
    int newBalance;
    while(1)
    {
        commissionPercent = ((rand() % 5) + 1.0)/100.0;

        for (auto it =accounts.begin(); it != accounts.end(); ++it)
        {
            if (it->first != "Bank")
            {
                curBalance = accounts[it->first].getBalance();
                commissionFee = curBalance*commissionPercent;
                newBalance = curBalance - commissionFee;
                if (newBalance > 0 )
                {
                    accounts[it->first].withdraw(commissionFee);
                    accounts["Bank"].deposit(commissionFee);
                    std::cout << "Bank: commissions of " << commissionPercent << "% were charged, the bank gained "\
                    << commissionFee << "$ from account " << it->first << std::endl;
                }
            }
        }
        //std::cout << "Bank " << accounts["Bank"].getBalance() << std::endl;
        usleep(THREE_SECOND_DELAY);

    }
}