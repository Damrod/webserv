#include <iostream>
#include <csignal>
#include <cstdlib>
#include <WebServer.hpp>

int main(int argc, char *argv[]) {
	std::signal(SIGPIPE, SIG_IGN);
	std::string	config_path = "config/default.conf";

	if (argc == 2) {
		config_path = argv[1];
	} else if (argc > 2) {
		std::cerr << "Usage: " << argv[0] << " [config path]\n";
		return EXIT_FAILURE;
	}
	try {
		WebServer	webserver(config_path);
		webserver.Run();
	}
	catch (const Analyser::SyntaxError &e) {
		std::cerr << "Syntax error while parsing the configuration file:\n"
					<< e.what() << '\n';
		return EXIT_FAILURE;
	}
	catch (const std::exception &e) {
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
