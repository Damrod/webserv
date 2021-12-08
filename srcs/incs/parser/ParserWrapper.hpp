#ifndef SRCS_INCS_PARSER_PARSERWRAPPER_HPP_
#define SRCS_INCS_PARSER_PARSERWRAPPER_HPP_

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <parser/Analyser.hpp>
#include <parser/ParsingState.hpp>

struct ServerConfig;
struct CommonConfig;

namespace Parser {

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
	size_t i;
	for (i = 0; i < level; ++i)
		o << "\t";
	o << title << " : \n";
	i = 0;
	for(typename std::map<T, U>::const_iterator iter = map.begin();
		iter != map.end();
		++iter, ++i) {
		for (size_t j = 0; j < static_cast<size_t>(level + 1); ++j)
			o << "\t";
		o << i << ": " << key << ": " << iter->first << ", "<< value << ":"
		  << iter->second << "\n";
	}
	return o.str();
}

class Wrapper : public Analyser {
 private:
	std::vector<ServerConfig>	*servers_settings_;
	bool CanAddLocation_(const std::string &path) const;

 public:
	explicit Wrapper(std::vector<ServerConfig> *server_settings);
	virtual ~Wrapper(void) {}
	void SetListenAddress(uint32_t address, uint16_t port, State::e_id ctx,
						  size_t line);
	void AddServerName(const std::vector<std::string> &args, State::e_id ctx,
					   size_t line);
	void SetRoot(const std::string &root, State::e_id ctx, size_t line);
	void SetIndex(const std::string &index, State::e_id ctx, size_t line);
	void SetAutoindex(bool autoindex, State::e_id ctx, size_t line);
	void SetClientMaxSz(uint32_t size, State::e_id ctx, size_t line);
	void AddErrorPage(uint16_t code, const std::string &uri, State::e_id ctx,
						size_t line);
	void AddCgiAssign(const std::string &extension,
					  const std::string &binaryHandlerPath, State::e_id ctx,
					  size_t line);
	void AddServer(State::e_id ctx, size_t line);
	void AddLocation(const std::string &path, State::e_id ctx,
					 size_t line);
	void SetReturn(uint16_t status, const std::string &url,
				   State::e_id ctx, size_t line);
	void SetUploadStore(const std::string &store, State::e_id ctx, size_t line);
	void SetLimitExcept(const std::vector<std::string> &httpMethods,
						State::e_id ctx, size_t line);
};

}  // namespace Parser

#endif  // SRCS_INCS_PARSER_PARSERWRAPPER_HPP_
