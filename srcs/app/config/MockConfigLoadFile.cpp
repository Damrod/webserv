#include <MockConfigLoadFile.hpp>
#include <utility>
#include <ServerConfig.hpp>

MockConfigLoadFile::MockConfigLoadFile() {
	InitConfig_();
}

std::vector<ServerConfig>	MockConfigLoadFile::GetServersConfigs() const {
	return servers_configs_;
}

/*
server {
	listen			8080;
	server_name		localhost;
	autoindex		on;
	index			nonexistent.html;
}

server {
	listen			8081;
	server_name		localhost;
	root			html/web1;

	location		/google {
		return 301	https://www.google.com;
	}
}

server {
	listen			8082;
	server_name		localhost;
	root			html/web2;
	error_page 404	/404.html;

	location		/index/ {
		index		nonexistent.html;
		autoindex	on;
	}
}

server {
	listen			8083;
	return 301		https://www.google.com;
}

server {
	listen			8084;
	root			html/web3;

	location	/upload/ {
		limit_except POST GET;
		upload_store	html/web3/test/;
	}

	location	/test/ {
		limit_except DELETE GET;
	}

	location	/images/ {
		limit_except GET POST;
		client_max_body_size	42;
	}

	location	/cgi-bin/ {
		limit_except POST GET;
		cgi-assign .py	/usr/bin/python2;
	}
}
*/
void MockConfigLoadFile::InitConfig_() {
	// Add the server listening on 8080 with autoindex enable
	ServerConfig server1;
	server1.listen_port = 8080;
	server1.server_name.push_back("localhost");
	server1.common.autoindex = true;
	server1.common.index = "nonexistent.html";
	servers_configs_.push_back(server1);

	// Add the server listening on 8081
	ServerConfig server2;
	server2.listen_port = 8081;
	server2.server_name.push_back("localhost");
	server2.common.root = "html/web1";

	Location web1_redirect("/google", server2.common);
	web1_redirect.common.return_status = 301;
	web1_redirect.common.return_url = "https://www.google.com";
	server2.locations.push_back(web1_redirect);
	servers_configs_.push_back(server2);

	// Add the server listening on 8082
	ServerConfig server3;
	server3.listen_port = 8082;
	server3.server_name.push_back("localhost");
	server3.common.root = "html/web2";
	server3.common.error_pages.insert(std::make_pair(404, "/404.html"));

	Location web2_autoindex("/index/", server3.common);
	web2_autoindex.common.index = "nonexistent.html";
	web2_autoindex.common.autoindex = true;
	server3.locations.push_back(web2_autoindex);
	servers_configs_.push_back(server3);

	// Add the server listening on 8083, performing an HTTP redirect
	ServerConfig	server4;
	server4.listen_port = 8083;
	server4.common.return_status = 301;
	server4.common.return_url = "https://www.google.com";
	servers_configs_.push_back(server4);

	// Add the server listening on 8085
	ServerConfig server5;
	server5.listen_port = 8084;
	server5.common.root = "html/web3";

	Location web3_upload("/upload/", server5.common);
	web3_upload.limit_except.push_back("POST");
	web3_upload.limit_except.push_back("GET");
	web3_upload.common.upload_store = "html/web3/test/";
	server5.locations.push_back(web3_upload);

	Location web3_test("/test/", server5.common);
	web3_test.limit_except.push_back("DELETE");
	web3_test.limit_except.push_back("GET");
	server5.locations.push_back(web3_test);

	Location web3_images("/images/", server5.common);
	web3_images.limit_except.push_back("GET");
	web3_images.limit_except.push_back("POST");
	web3_images.common.client_max_body_size = 42;
	server5.locations.push_back(web3_images);

	Location web3_cgi_bin("/cgi-bin/", server5.common);
	web3_cgi_bin.limit_except.push_back("POST");
	web3_cgi_bin.limit_except.push_back("GET");
	web3_cgi_bin.common.cgi_assign.insert(
									std::make_pair("py", "/usr/bin/python2"));
	server5.locations.push_back(web3_cgi_bin);
	servers_configs_.push_back(server5);
}
