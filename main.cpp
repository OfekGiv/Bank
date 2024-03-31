#include <iostream>
#include <string>
// #include <mutex>
#include <map>
#include <pthread.h>
#include "bank.h"

std::map<int,bankAccount> accounts;

int main(int argc, char *argv[])
{
/*
    bankAccount account1(1234,1111,500);

    std::cout << account1.getBalance() << std::endl;
    account1.deposit(250);
    std::cout << account1.getBalance() << std::endl;
    account1.withdraw(700);
    std::cout << account1.getBalance() << std::endl;
    account1.withdraw(200);
    std::cout << account1.getBalance() << std::endl;
   

    accounts.insert({1234,bankAccount(1234,1111,500)});
    
    std::cout << accounts[1234].getBalance() << std::endl;
*/
    
    pthread_t threads[argc-1];
    atmThreadArgs atmThrArgs[argc-1];
   
    int rc,t,i;
    
    for (t=1;t < argc;t++)
    {
        atmThrArgs[t-1].atmNum = t;
        atmThrArgs[t-1].inputFileName = argv[t];
        if (rc = pthread_create(&threads[t-1],NULL,atmRoutine,&atmThrArgs[t-1]))
        {
            perror("Bank error");
            return EXIT_FAILURE;
        }
    }

    for(i=1; i<argc;i++)
    {
        pthread_join(threads[i-1],NULL);
    }
    
    return EXIT_SUCCESS; 
}