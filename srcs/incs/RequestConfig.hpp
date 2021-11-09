#ifndef SRCS_INCS_REQUESTCONFIG_HPP_
#define SRCS_INCS_REQUESTCONFIG_HPP_
#include <string>
#include <vector>
#include <algorithm>
#include <ServerConfig.hpp>

// All config related to a request in a Sever

class RequestConfig {
	public:
		RequestConfig(const ServerConfig &server_config,
						const std::string &request_path);
		RequestConfig();
		RequestConfig(const RequestConfig &);
		std::string	getPath() const;
		bool limits(std::string method) const;
		std::string	getRoot() const;
		uint32_t	getClientMaxBodySize() const;
		bool	hasAutoindex() const;
		std::string	getIndex() const;
		std::string	getUploadStore() const;
		uint16_t	getReturnStatus() const;
		std::string	getReturnUrl() const;
		bool	hasReturnUrl() const;
		bool	hasCGI(std::string extension) const;
		std::string	getErrorPagePath(std::size_t errCode) const;
		CommonConfig::BinaryHandlerPath getCGIBin(std::string extension) const;

		//temporal
		const CommonConfig	&getCommonConfig() const;

	private:
		RequestConfig &	operator=(const RequestConfig &);
		const Location	*FindLocation_(
			const ServerConfig &server_config,
			const std::string &request_path)
			const;

		const Location	*location_;
		const CommonConfig	&common_;
		const std::string	*path_;
		const std::vector<std::string>	*limit_except_;
		std::string	root_;
		uint32_t	client_max_body_size_;
		bool	autoindex_;
		std::string	index_;
		CommonConfig::ErrorPagesMap error_pages_;
		std::string	upload_store_;
		uint16_t	return_status_;
		std::string	return_url_;
		CommonConfig::CgiAssignMap	cgi_assign_;
};

#endif  // SRCS_INCS_REQUESTCONFIG_HPP_
