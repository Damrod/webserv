#include <RequestConfig.hpp>
#include <sys/types.h>

RequestConfig::RequestConfig(const ServerConfig &server_config,
		const std::string &request_path) :
	location_(FindLocation_(server_config, request_path)),
	common_(location_ ? location_->common : server_config.common),
	path_(location_ ? &location_->path : NULL),
	limit_except_(location_ ? &location_->limit_except : NULL),
	root_(common_.root),
	client_max_body_size_(common_.client_max_body_size),
	autoindex_(common_.autoindex),
	index_(common_.index),
	error_pages_(common_.error_pages),
	upload_store_(common_.upload_store),
	return_status_(server_config.common.return_url.empty() ?
		server_config.common.return_status :
		common_.return_status),
	return_url_(server_config.common.return_url.empty() ?
		server_config.common.return_url :
		common_.return_url),
	cgi_assign_(common_.cgi_assign) {
}

bool RequestConfig::limits(std::string method) const {
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

std::string	RequestConfig::getPath() const {
	return location_ ? *path_ : "/";
}

std::string	RequestConfig::getRoot() const {
	return root_;
}

uint32_t	RequestConfig::getClientMaxBodySize() const {
	return client_max_body_size_;
}

bool	RequestConfig::hasAutoindex() const {
	return autoindex_;
}

std::string	RequestConfig::getIndex() const {
	return index_;
}

std::string	RequestConfig::getUploadStore() const {
	return upload_store_;
}

uint16_t	RequestConfig::getReturnStatus() const {
	return return_status_;
}

std::string	RequestConfig::getReturnUrl() const {
	return return_url_;
}

std::string	RequestConfig::getErrorPagePath(std::size_t errCode) const {
	CommonConfig::ErrorPagesMap::const_iterator it =
										error_pages_.find(errCode);
	return it == error_pages_.end() ? "" : root_ + it->second;
}

CommonConfig::BinaryHandlerPath
	RequestConfig::getCGIBin(std::string extension) const {
	return cgi_assign_.find(extension)->second;
}

bool RequestConfig::hasCGI(std::string extension) const {
	return cgi_assign_.count(extension) > 0;
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

const CommonConfig	&RequestConfig::getCommonConfig() const {
	return common_;
}
