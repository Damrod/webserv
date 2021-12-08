#include <string>
#include <sstream>
#include <Utils.hpp>
#include <catch2.hpp>
#include <parser/Lexer.hpp>
#include <parser/Token.hpp>
#include <parser/Preprocessor.hpp>

TEST_CASE("Testing the tokenizer AKA lexer", "[parser]") {
	std::string expected_result("{\n"
								"server\n"
								"{\n"
								"listen\n"
								"80\n"
								"default_server\n"
								";\n"
								"listen\n"
								"[::]:80\n"
								"default_server\n"
								";\n"
								"location\n"
								"/\n"
								"{\n"
								"root\n"
								"/var/www/html\n"
								";\n"
								"}\n"
								"location\n"
								"=\n"
								"/404.html\n"
								"{\n"
								"internal\n"
								";\n"
								"}\n"
								"}\n"
								"}\n");
	std::ostringstream result;
	try {
		std::string path = "srcs/test/unit/config_analyser/"
			"nginx_docker/vol/http.d/default.conf";
		Preprocessor file(path);
		Lexer lexed(file.GetFileBuffer());
		std::list<Token> tokens = lexed.GetTokens();
		std::list<Token>::iterator it = tokens.begin();
		for (; it != tokens.end(); ++it) {
			result << it->GetRawData() << "\n";
		}
		REQUIRE(expected_result == result.str());
#ifdef DBG
		it = tokens->begin();
		for (; it != tokens->end(); ++it) {
			std::cout << it->getRawData() << " type: "
					  << it->GetTokenTypeStr() << "\n";
		}
#endif
	} catch(const std::exception &e) {
		FAIL(e.what());
	}
}
