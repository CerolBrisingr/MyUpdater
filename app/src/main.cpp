#include "Updater2/logic/interface.h"
#include "io/fileinteractions.h"
#include "Updater2/widget/mainwindow.h"
#include "Updater2/hello.h"

#include <iostream>

using namespace Updater2;

int main()
{
    Core::Interface eventHandler{};
    IO::cleanUpRemainingTempFiles();
    std::cout << "Hello world!\n";
    printHello();

    return ui::widget::runMainwindow(eventHandler);
}