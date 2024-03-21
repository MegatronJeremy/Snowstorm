#include <exception>
#include <iostream>

#include "Core/Core.h"

int main()
{
	Core::HelloTriangleApplication app;

	try
	{
		app.Run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
