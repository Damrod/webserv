#include <webserv.hpp>

int startup(const std::vector<std::string> &argv,
			const std::vector<std::string> &env) {
	(void) argv;
	(void) env;
	return 100;
}
