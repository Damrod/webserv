#include <string>
#include <sstream>
#include <Utils.hpp>
#include <catch2.hpp>
#include <parser/Preprocessor.hpp>

TEST_CASE("Testing the preprocessor, AKA comment remover", "[parser]") {
	std::string expected_result("\n"
								"\n"
								"\n"
								"server {\n"
								"\tlisten 80 default_server;\n"
								"\tlisten [::]:80 default_server;\n"
								"\t\n"
								"\t\n"
								"\n"
								"\t\n"
								"\tlocation / {\n"
								"\t\troot /var/www/html;\n"
								"\t}\n"
								"\n"
								"\t\n"
								"\tlocation = /404.html {\n"
								"\t\tinternal;\n"
								"\t}\n"
								"}\n"
								"\n"
								"\n"
								"\n"
								"\n"
								"\n"
								"\n"
								"\n"
								"\n"
								"\n"
								"\n"
								"\n"
								"\n"
								"\n"
								"\n"
								"\n"
								"\n"
								"\n"
								"\n"
								"\n"
								"\n");
	std::ostringstream result;
	try {
		std::string path = "srcs/app/test/unit/config_analyser/"
			"nginx_docker/vol/http.d/default.conf";
		Preprocessor file(path);
		std::string filebuff = file.GetFileBuffer();
		REQUIRE(expected_result == filebuff);
	} catch(const std::exception &e) {
		FAIL(e.what());
	}
}
