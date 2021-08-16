#include <arpa/inet.h>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <stdint.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace {
const uint16_t    kServerPort = 8080;
const std::string kHttpResponse = "HTTP/1.1 200 OK\r\n"
								  "Server: hello_world\r\n"
								  "Content-Length: 22\r\n"
								  "Content-Type: text/html\r\n"
								  "\r\n"
								  "<h1>Hello, World!</h1>";

int               SetMaxSocket(int curr_sd, int max_sd, fd_set *master_set) {
    if (curr_sd == max_sd) {
        while (FD_ISSET(max_sd, master_set) == 0) {
            --max_sd;
        }
    }
    return max_sd;
}

int BindListeningSocket(uint32_t address, uint16_t port) {
	int listen_sd = socket(AF_INET, SOCK_STREAM, 0);
	fcntl(listen_sd, F_SETFL, O_NONBLOCK);

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;  // IPv4
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(address);
	std::memset(addr.sin_zero, 0, sizeof(addr.sin_zero));

	int on = 1;
	setsockopt(listen_sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	bind(listen_sd, (struct sockaddr *)&addr, sizeof(addr));

	listen(listen_sd, SOMAXCONN);  // 128
	return listen_sd;
}

void MonitorConnections(int listen_sd) {
	std::map<int, std::string> connections_buffers;

	fd_set                     master_set, read_set, write_set;
	FD_ZERO(&master_set);
	FD_ZERO(&write_set);

	int max_sd = listen_sd;
	FD_SET(listen_sd, &master_set);

	while (1) {
		std::memcpy(&read_set, &master_set, sizeof(master_set));
		int ready_connections =
			select(max_sd + 1, &read_set, &write_set, NULL, NULL);
		for (int i = 0; i <= max_sd && ready_connections > 0; ++i) {
			if (FD_ISSET(i, &read_set)) {
				--ready_connections;
				if (i == listen_sd) {
					int new_sd = accept(listen_sd, NULL, NULL);
					fcntl(new_sd, F_SETFL, O_NONBLOCK);
					FD_SET(new_sd, &master_set);
					FD_SET(new_sd, &write_set);
					connections_buffers[new_sd] = kHttpResponse;
					if (new_sd > max_sd)
						max_sd = new_sd;
				} else {
					char buff[4096];
					int  nbytes = recv(i, buff, sizeof(buff), 0);
					if (nbytes == 0) {
						close(i);
						FD_CLR(i, &master_set);
						max_sd = SetMaxSocket(i, max_sd, &master_set);
					} else if (nbytes > 0) {
						write(1, buff, nbytes);
					}
				}
			} else if (FD_ISSET(i, &write_set)) {
				--ready_connections;
				int nbytes = send(i, connections_buffers[i].c_str(),
					(int)connections_buffers[i].size(), 0);
				if (nbytes > 0 &&
					nbytes <= (int)connections_buffers[i].size()) {
					connections_buffers[i].erase(0, nbytes);
				}
				if (nbytes < 0 || connections_buffers[i].empty()) {
					close(i);
					FD_CLR(i, &master_set);
					FD_CLR(i, &write_set);
					max_sd = SetMaxSocket(i, max_sd, &master_set);
				}
			}
		}
	}
}

}  // namespace

int main() {
	int listen_sd = BindListeningSocket(INADDR_LOOPBACK, kServerPort);
	MonitorConnections(listen_sd);
}
