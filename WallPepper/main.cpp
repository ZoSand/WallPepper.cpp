#include <stdexcept>
#include <clocale>
#include <iostream>

#include "Application.h"

int main(int argc, char* argv[])
{
	//unused parameters warning
	(void) argc;
	(void) argv;

	//var declaration
	Pepper::Core::EngineBase::EngineType engineType = Pepper::Core::EngineBase::EngineType::Vulkan;
	Pepper::Main::Application app;
	int status = EXIT_SUCCESS;

	//init
	::setlocale(LC_ALL, "");
	app.Init(engineType);

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