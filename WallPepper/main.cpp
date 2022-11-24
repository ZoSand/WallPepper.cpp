#include <clocale>
#include <exception>
#include <iostream>

#include "Application.h"

int main() {
    //var declaration
    Pepper::App::Application app;
    int status = EXIT_SUCCESS;

    //init
    ::setlocale(LC_ALL, "");
    app.Init();

    //run loop
    try {
        app.Run();
    }
    catch (std::exception &_e) {
        std::cerr << _e.what() << std::endl;
        status = EXIT_FAILURE;
    }

    //cleanup
    app.Clear();

    return status;
}