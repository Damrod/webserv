#ifndef SRCS_INCS_PARSER_PARSERAPI_HPP_
#define SRCS_INCS_PARSER_PARSERAPI_HPP_

#include <sstream>
#include <map>
#include <string>
#include <vector>
#include <parser/Token.hpp>

struct ServerConfig;

template <typename T>
std::string toStrIndented(uint8_t level, const std::string &key,
const T &value) {
	std::ostringstream str;
	for (size_t i = 0; i < level; ++i)
		str << "\t";
	str << key << " : " << value << std::endl;
	return str.str();
}

template <typename T, typename U>
std::string MapToStrIndented(uint8_t level, const std::string &title,
std::map<T, U>map, const std::string &key, const std::string &value) {
	std::stringstream o;
	for (size_t i = 0; i < level; ++i)
		o << "\t";
	o << title << " : \n";
	for(typename std::map<T, U>::const_iterator iterr_pages = map.begin();
		iterr_pages != map.end();
		++iterr_pages) {
		for (size_t i = 0; i < static_cast<size_t>(level + 1); ++i)
			o << "\t";
		o << key << ": " << iterr_pages->first << ", "<< value << ":"
		  << iterr_pages->second << "\n";
	}
	return o.str();
}

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

std::ostream &operator<<(std::ostream &o,
						 const std::vector<ServerConfig> &server_settings);

#endif  // SRCS_INCS_PARSER_PARSERAPI_HPP_
