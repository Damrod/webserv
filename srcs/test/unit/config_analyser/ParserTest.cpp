#include <string>
#include <sstream>
#include <catch2/catch_test_macros.hpp>
#include <parser/Preprocessor.hpp>
#include <parser/ParserManager.hpp>

TEST_CASE("Testing the parser", "[parser]") {
	std::string expected = "server 0:\n"
							"\tlisten_address : 0.0.0.0\n"
							"\tlisten_port : 8080\n"
							"\tserver_names :\n"
							"\t\tserver_name 0: localhost\n"
							"\troot : html\n"
							"\tclient_max_body_size : 1000000\n"
							"\tautoindex : 1\n"
							"\tindex : nonexistent.html\n"
							"\tupload_store : \n"
							"\treturn_status : 0\n"
							"\treturn_url : \n"
							"\terror_pages map : \n"
							"\tcgi_assign map : \n"
							"\tlocations :\n"
							"server 1:\n"
							"\tlisten_address : 0.0.0.0\n"
							"\tlisten_port : 8081\n"
							"\tserver_names :\n"
							"\t\tserver_name 0: localhost\n"
							"\troot : html/web1\n"
							"\tclient_max_body_size : 1000000\n"
							"\tautoindex : 0\n"
							"\tindex : index.html\n"
							"\tupload_store : \n"
							"\treturn_status : 0\n"
							"\treturn_url : \n"
							"\terror_pages map : \n"
							"\tcgi_assign map : \n"
							"\tlocations :\n"
							"\tpath 0: /google\n"
							"\t\tlimit except : \n"
							"\t\troot : html/web1\n"
							"\t\tclient_max_body_size : 1000000\n"
							"\t\tautoindex : 0\n"
							"\t\tindex : index.html\n"
							"\t\tupload_store : \n"
							"\t\treturn_status : 301\n"
							"\t\treturn_url : https://www.google.com\n"
							"\t\terror_pages map : \n"
							"\t\tcgi_assign map : \n"
							"server 2:\n"
							"\tlisten_address : 0.0.0.0\n"
							"\tlisten_port : 8082\n"
							"\tserver_names :\n"
							"\t\tserver_name 0: localhost\n"
							"\troot : html/web2\n"
							"\tclient_max_body_size : 1000000\n"
							"\tautoindex : 0\n"
							"\tindex : index.html\n"
							"\tupload_store : \n"
							"\treturn_status : 0\n"
							"\treturn_url : \n"
							"\terror_pages map : \n"
							"\t\t0: error code: 404, error URI:/404.html\n"
							"\tcgi_assign map : \n"
							"\tlocations :\n"
							"\tpath 0: /index/\n"
							"\t\tlimit except : \n"
							"\t\troot : html/web2\n"
							"\t\tclient_max_body_size : 1000000\n"
							"\t\tautoindex : 1\n"
							"\t\tindex : nonexistent.html\n"
							"\t\tupload_store : \n"
							"\t\treturn_status : 0\n"
							"\t\treturn_url : \n"
							"\t\terror_pages map : \n"
							"\t\t\t0: error code: 404, error URI:/404.html\n"
							"\t\tcgi_assign map : \n"
							"server 3:\n"
							"\tlisten_address : 0.0.0.0\n"
							"\tlisten_port : 8083\n"
							"\tserver_names :\n"
							"\troot : html\n"
							"\tclient_max_body_size : 1000000\n"
							"\tautoindex : 0\n"
							"\tindex : index.html\n"
							"\tupload_store : \n"
							"\treturn_status : 301\n"
							"\treturn_url : https://www.google.com\n"
							"\terror_pages map : \n"
							"\tcgi_assign map : \n"
							"\tlocations :\n"
							"server 4:\n"
							"\tlisten_address : 0.0.0.0\n"
							"\tlisten_port : 8084\n"
							"\tserver_names :\n"
							"\troot : html/web3\n"
							"\tclient_max_body_size : 1000000\n"
							"\tautoindex : 0\n"
							"\tindex : index.html\n"
							"\tupload_store : \n"
							"\treturn_status : 0\n"
							"\treturn_url : \n"
							"\terror_pages map : \n"
							"\tcgi_assign map : \n"
							"\tlocations :\n"
							"\tpath 0: /upload/\n"
							"\t\tlimit except : \n"
							"\t\t\t0: POST\n"
							"\t\t\t1: GET\n"
							"\t\troot : html/web3\n"
							"\t\tclient_max_body_size : 1000000\n"
							"\t\tautoindex : 0\n"
							"\t\tindex : index.html\n"
							"\t\tupload_store : html/web3/test/\n"
							"\t\treturn_status : 0\n"
							"\t\treturn_url : \n"
							"\t\terror_pages map : \n"
							"\t\tcgi_assign map : \n"
							"\tpath 1: /test/\n"
							"\t\tlimit except : \n"
							"\t\t\t0: DELETE\n"
							"\t\t\t1: GET\n"
							"\t\troot : html/web3\n"
							"\t\tclient_max_body_size : 1000000\n"
							"\t\tautoindex : 0\n"
							"\t\tindex : index.html\n"
							"\t\tupload_store : \n"
							"\t\treturn_status : 0\n"
							"\t\treturn_url : \n"
							"\t\terror_pages map : \n"
							"\t\tcgi_assign map : \n"
							"\tpath 2: /images/\n"
							"\t\tlimit except : \n"
							"\t\t\t0: GET\n"
							"\t\t\t1: POST\n"
							"\t\troot : html/web3\n"
							"\t\tclient_max_body_size : 42\n"
							"\t\tautoindex : 0\n"
							"\t\tindex : index.html\n"
							"\t\tupload_store : \n"
							"\t\treturn_status : 0\n"
							"\t\treturn_url : \n"
							"\t\terror_pages map : \n"
							"\t\tcgi_assign map : \n"
							"\tpath 3: /cgi-bin/\n"
							"\t\tlimit except : \n"
							"\t\t\t0: POST\n"
							"\t\t\t1: GET\n"
							"\t\troot : html/web3\n"
							"\t\tclient_max_body_size : 1000000\n"
							"\t\tautoindex : 0\n"
							"\t\tindex : index.html\n"
							"\t\tupload_store : \n"
							"\t\treturn_status : 0\n"
							"\t\treturn_url : \n"
							"\t\terror_pages map : \n"
							"\t\tcgi_assign map : \n"
							"\t\t\t0: file extension: py, "
							"binary handler path:/usr/bin/python2\n";
	std::ostringstream result;
	try {
		ParserManager tst("srcs/test/unit/config_analyser/nginx_docker/vol/"
						  "http.d/AGVTest.conf");
		std::vector<ServerConfig> cnf = tst.GetServersSettings();
		result << cnf;
		// std::cout << cnf;
		// std::cout << "=================  VS =================\n";
		// std::cout << expected;
		REQUIRE(result.str() == expected);
		// std::cout << config;
	} catch(const std::exception &e) {
		FAIL(e.what());
	}
}

TEST_CASE("Test bad context : location", "[parser]") {
	try {
		ParserManager tst("srcs/test/unit/config_analyser/nginx_docker/"
						  "vol/http.d/failing/bad_ctx_location.conf");
		std::ostringstream result;
		std::vector<ServerConfig> cnf = tst.GetServersSettings();
		result << cnf;
	} catch(const std::exception &e) {
		// std::cout << e.what();
		REQUIRE(std::strcmp(e.what(),
		"Keyword `location' not allowed in global scope in line 35") == 0);
	}
}

TEST_CASE("Test bad context : location2", "[parser]") {
	try {
		ParserManager tst("srcs/test/unit/config_analyser/nginx_docker/"
				  "vol/http.d/failing/bad_ctx_location2.conf");
		std::ostringstream result;
		std::vector<ServerConfig> cnf = tst.GetServersSettings();
		result << cnf;
	} catch(const std::exception &e) {
		// std::cout << e.what();
		REQUIRE(std::strcmp(e.what(),
					"Keyword `location' not allowed in context `location'"
					" in line 40") == 0);
}
}

TEST_CASE("Test bad context : server", "[parser]") {
	try {
		ParserManager tst("srcs/test/unit/config_analyser/nginx_docker/"
						  "vol/http.d/failing/bad_ctx_server.conf");
		std::ostringstream result;
		std::vector<ServerConfig> cnf = tst.GetServersSettings();
		result << cnf;
	} catch(const std::exception &e) {
		// std::cout << e.what();
		REQUIRE(std::strcmp(e.what(),
		"Keyword `server' not allowed in context `server' in line 6") == 0);
	}
}

TEST_CASE("Test bad context : limit_except", "[parser]") {
	try {
		ParserManager tst("srcs/test/unit/config_analyser/"
			"nginx_docker/vol/http.d/failing/bad_ctx_limit_exc.conf");
		std::ostringstream result;
		std::vector<ServerConfig> cnf = tst.GetServersSettings();
		result << cnf;
	} catch(const std::exception &e) {
		// std::cout << e.what();
		REQUIRE(std::strcmp(e.what(),
		"Keyword `limit_except' not allowed "
		"in context `server' in line 33") == 0);
	}
}

TEST_CASE("Test bad context : server_name", "[parser]") {
	try {
		ParserManager tst("srcs/test/unit/config_analyser/nginx_docker/"
						  "vol/http.d/failing/bad_ctx_server_name.conf");
		std::ostringstream result;
		std::vector<ServerConfig> cnf = tst.GetServersSettings();
		result << cnf;
	} catch(const std::exception &e) {
		// std::cout << e.what();
		REQUIRE(std::strcmp(e.what(),
			"Keyword `server_name' not allowed in context `location' "
			"in line 14") == 0);
	}
}

TEST_CASE("Test bad context : listen", "[parser]") {
	try {
		ParserManager tst("srcs/test/unit/config_analyser/nginx_docker/"
						  "vol/http.d/failing/bad_ctx_listen.conf");
		std::ostringstream result;
		std::vector<ServerConfig> cnf = tst.GetServersSettings();
		result << cnf;
	} catch(const std::exception &e) {
		// std::cout << e.what();
		REQUIRE(std::strcmp(e.what(),
			"Keyword `listen' not allowed in context "
			"`location' in line 42") == 0);
	}
}
