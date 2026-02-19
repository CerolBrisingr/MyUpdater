#include "io/fileinteractions.h"
#include "Updater2/hello.h"

#include <iostream>

using namespace Updater2;

int main()
{
    IO::cleanUpRemainingTempFiles();
    std::cout << "Hello world!\n";
    printHello();

    return 0;
}