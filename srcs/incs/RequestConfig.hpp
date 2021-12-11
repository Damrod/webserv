#ifndef SRCS_INCS_REQUESTCONFIG_HPP_
#define SRCS_INCS_REQUESTCONFIG_HPP_
#include <sys/types.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <ServerConfig.hpp>

// All config related to a request in a Sever

class RequestConfig {
	public:
		RequestConfig(const ServerConfig &server_config,
						const std::string &request_path);
		std::string	GetPath() const;
		bool Limits(std::string const &method) const;
		std::string	GetRoot() const;
		uint32_t	GetClientMaxBodySize() const;
		bool	HasAutoindex() const;
		std::string	GetIndex() const;
		std::string	GetUploadStore() const;
		uint16_t	GetReturnStatus() const;
		std::string	GetReturnUrl() const;
		bool	HasCGI(const std::string &extension) const;
		std::string	GetErrorPagePath(std::size_t errCode) const;
		CommonConfig::BinaryHandlerPath GetCGIBin(const std::string &extension) const;
		CommonConfig::ErrorPagesMap GetErrorPages() const;

	private:
		RequestConfig();
		RequestConfig(const RequestConfig &);
		RequestConfig &	operator=(const RequestConfig &);

		const Location	*FindLocation_(
			const ServerConfig &server_config,
			const std::string &request_path)
			const;

		void SetRedirectionConfig_(const ServerConfig &server_config);

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
