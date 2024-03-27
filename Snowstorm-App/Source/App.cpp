#include <exception>
#include <iostream>

#include "SNowstorm/Core/Core.h"

int main()
{
	Snowstorm::HelloTriangleApplication app;

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
