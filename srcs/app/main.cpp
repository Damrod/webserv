#include <webserv.hpp>

int main(int argc, char *argv[], char *environ[]) {
	std::vector<std::string> argvect;
	std::vector<std::string> envect;

	for (int i = 1; i < argc; i++)
		argvect.push_back(argv[i]);
	for (; *environ; ++environ)
		argvect.push_back(*environ);
	return startup(argvect, envect);
}
