#include <stdexcept>
#include <clocale>
#include <iostream>

#include "Application.h"

int main()
{
    //var declaration
    Pepper::Main::Application app;
    int status = EXIT_SUCCESS;

    //init
    ::setlocale(LC_ALL, "");
    app.Init();

    try
    {
        //run loop
        app.Run();
    }
    catch (std::runtime_error &_e)
    {
        std::cerr << _e.what() << std::endl;
        status = EXIT_FAILURE;
    }

    //cleanup
    app.Clear();

    return status;
}