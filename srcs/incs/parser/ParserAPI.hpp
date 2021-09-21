#ifndef SRCS_INCS_PARSER_PARSERAPI_HPP_
#define SRCS_INCS_PARSER_PARSERAPI_HPP_

#include <string>
#include <vector>
#include <parser/Token.hpp>

struct ServerConfig;

class ParserAPI {
 private:
	std::vector<ServerConfig>	*servers_settings_;
 public:
	explicit ParserAPI(std::vector<ServerConfig> *server_settings);
	std::vector<ServerConfig>	&GetServersSettings(void);
	void SetServersSettings(std::vector<ServerConfig> *server_settings);
	virtual ~ParserAPI(void) {}
	void SetListenPort(uint16_t port, t_parsing_state ctx);
	void SetListenAddress(uint32_t address, t_parsing_state ctx);
	void AddServerName(const std::string &name, t_parsing_state ctx);
	void SetRoot(const std::string &root, t_parsing_state ctx);
	void AddIndex(const std::string &index, t_parsing_state ctx);
	void AddAutoindex(bool autoindex, t_parsing_state ctx);
	void SetClientMaxSz(uint32_t size, t_parsing_state ctx);
	void AddServer(t_parsing_state ctx);
	void AddLocation(const std::string &path, t_parsing_state ctx);
};

std::ostream &operator<<(std::ostream &o, ParserAPI &c);

std::ostream &operator<<(std::ostream &o,
						 const std::vector<ServerConfig> &server_settings);

#endif  // SRCS_INCS_PARSER_PARSERAPI_HPP_
