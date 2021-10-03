#include <iostream>
#include <cstdlib>
#include <WebServer.hpp>

int main(int argc, char *argv[]) {
	WebServer	webserver;
	std::string	config_path = "config/default.conf";

	if (argc == 2) {
		config_path = argv[1];
	} else if (argc > 2) {
		std::cerr << "Usage: " << argv[0] << " [config path]\n";
		return EXIT_FAILURE;
	}
	try {
		webserver.Init(config_path);
		webserver.Run();
	}
	catch (const std::exception &e) {
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
