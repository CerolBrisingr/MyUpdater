#include "io/fileinteractions.h"
#include "Updater2/hello.h"

#include <iostream>

using namespace Updater2;

int main()
{
    IO::cleanUpRemainingTempFiles();
    int test{ 10 };
    std::cout << "Hello world!\n";
    printHello();
    IO::printCurrentPath();

    return 0;
}