#include <WebServer.hpp>

int main(int argc, char *argv[]) {
	WebServer	webserver;
	std::string	config_path = "config/default.conf";
	if (argc == 2)
		config_path = argv[1];
	if (!webserver.Init(config_path))
		return -1;
	webserver.Run();
	return 0;
}
