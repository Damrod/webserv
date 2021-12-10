#include <RequestConfig.hpp>

RequestConfig::RequestConfig(const ServerConfig &server_config,
		const std::string &request_path) :
	location_(FindLocation_(server_config, request_path)),
	common_(location_ ? location_->common : server_config.common),
	location_path_(location_ ? &location_->path : NULL),
	limit_except_(location_ ? &location_->limit_except : NULL),
	root_(common_.root),
	client_max_body_size_(common_.client_max_body_size),
	autoindex_(common_.autoindex),
	index_(common_.index),
	error_pages_(common_.error_pages),
	upload_store_(common_.upload_store),
	cgi_assign_(common_.cgi_assign),
	request_path_(FindRequestPath_(request_path)),
	request_path_info_(FindRequestPathInfo_(request_path)) {
		SetRedirectionConfig_(server_config);
}

void RequestConfig::SetRedirectionConfig_(const ServerConfig &server_config) {
	if (!server_config.common.return_url.empty()) {
		return_url_ = server_config.common.return_url;
		return_status_ = server_config.common.return_status;
	} else {
		return_url_ = common_.return_url;
		return_status_ = common_.return_status;
	}
}

bool RequestConfig::Limits(std::string const &method) const {
	if (limit_except_ &&
		!limit_except_->empty() &&
		std::find(
			limit_except_->begin(),
			limit_except_->end(),
			method) == limit_except_->end()) {
		return true;
	}
	return false;
}

std::string	RequestConfig::GetLocationPath() const {
	return location_ ? *location_path_ : "/";
}

std::string	RequestConfig::GetRoot() const {
	return root_;
}

uint32_t	RequestConfig::GetClientMaxBodySize() const {
	return client_max_body_size_;
}

bool	RequestConfig::HasAutoindex() const {
	return autoindex_;
}

std::string	RequestConfig::GetIndex() const {
	return index_;
}

std::string	RequestConfig::GetUploadStore() const {
	return upload_store_;
}

uint16_t	RequestConfig::GetReturnStatus() const {
	return return_status_;
}

std::string	RequestConfig::GetReturnUrl() const {
	return return_url_;
}

std::string	RequestConfig::GetErrorPagePath(std::size_t errCode) const {
	CommonConfig::ErrorPagesMap::const_iterator it =
										error_pages_.find(errCode);
	return it == error_pages_.end() ? "" : root_ + it->second;
}

CommonConfig::BinaryHandlerPath
	RequestConfig::GetCGIBin(std::string extension) const {
	return cgi_assign_.find(extension)->second;
}

bool RequestConfig::HasCGI(std::string extension) const {
	return cgi_assign_.count(extension) > 0;
}

std::string	RequestConfig::GetRequestPath() const {
	return request_path_;
}

std::string RequestConfig::GetRequestPathInfo() const {
	return request_path_info_;
}

const Location* RequestConfig::FindLocation_(
		const ServerConfig &server_config,
		const std::string &request_path
	) const {
	const std::vector<Location>	&locations = server_config.locations;
	if (locations.empty()) {
		return NULL;
	}
	ssize_t	best_match_idx = -1;
	for (std::size_t i = 0; i < locations.size(); ++i) {
		if (request_path.rfind(locations[i].path, 0) != 0) {
			continue;
		}
		if (best_match_idx == -1 ||
			locations[best_match_idx].path.size() < locations[i].path.size()) {
			best_match_idx = i;
		}
	}
	return best_match_idx == -1 ? NULL : &locations[best_match_idx];
}

std::string	RequestConfig::FindRequestPath_(const std::string &request_path) {
	CommonConfig::CgiAssignMap::const_iterator it = cgi_assign_.begin();
	CommonConfig::CgiAssignMap::const_iterator ite = cgi_assign_.end();
	while (it != ite) {
		const std::string delimiter = "." + it->first + "/";
		const std::size_t offset = request_path.find(delimiter);
		if (offset != std::string::npos) {
			return request_path.substr(0, offset + it->first.size() + 1);
		}
		++it;
	}
	return request_path;
}

// https://datatracker.ietf.org/doc/html/rfc3875#section-4.1.5
std::string
RequestConfig::FindRequestPathInfo_(const std::string &request_path) {
	if (request_path_.size() < request_path.size()) {
		return request_path.substr(request_path_.size());
	}
	return "";
}
