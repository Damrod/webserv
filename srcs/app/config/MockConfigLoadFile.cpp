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

	location		/web2 {
		return 301	http://localhost:8082;
	}

}

server {
    listen			8082;
    server_name		localhost;
    root			html/web2;
    error_page 404	/404.html;

	location		/web1 {
		root		html;
		index		nonexistent.html;
		autoindex	on;
	}
}

server {
    listen			8083;
}
*/
void MockConfigLoadFile::InitConfig_() {
	// Add the server listening on 8080 with autoindex enable
	ServerConfig	server1;
	server1.listen_port = 8080;
	server1.server_name.push_back("localhost");
	server1.common.autoindex = true;
	server1.common.index = "nonexistent.html";
	servers_configs_.push_back(server1);

	// Add the server listening on 8081, serving the web1 page
	ServerConfig	server2;
	server2.listen_port = 8081;
	server2.server_name.push_back("localhost");
	server2.common.root = "html/web1";
	Location	web1_redirect("/web2", server2.common);
	web1_redirect.common.return_status = 301;
	web1_redirect.common.return_url = "http://localhost:8082";
	server2.locations.push_back(web1_redirect);
	servers_configs_.push_back(server2);

	// Add the server listening on 8082, serving the web2 page
	ServerConfig	server3;
	server3.listen_port = 8082;
	server3.server_name.push_back("localhost");
	server3.common.root = "html/web2";
	server3.common.error_pages.insert(std::make_pair(404, "/404.html"));
	Location	web2_autoindex("/web1", server3.common);
	web2_autoindex.common.root = "html";
	web2_autoindex.common.index = "nonexistent.html";
	web2_autoindex.common.autoindex = true;
	server3.locations.push_back(web2_autoindex);
	servers_configs_.push_back(server3);

	// Add the server listening on 8083, accessing the root page results in 403
	ServerConfig	server4;
	server4.listen_port = 8083;
	servers_configs_.push_back(server4);
}
