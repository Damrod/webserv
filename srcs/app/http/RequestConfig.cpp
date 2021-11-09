#include <RequestConfig.hpp>
#include <sys/types.h>

RequestConfig::RequestConfig(const ServerConfig &server_config,
		const std::string &request_path)
	: location_(FindLocation_(server_config, request_path)),
	common_(location_ ? location_->common : server_config.common),
	path_(location_ ? &location_->path : NULL),
	limit_except_(location_ ? &location_->limit_except : NULL) {
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
	return common_.root;
}

uint32_t	RequestConfig::getClientMaxBodySize() const {
	return common_.client_max_body_size;
}

bool	RequestConfig::hasAutoindex() const {
	return common_.autoindex;
}

std::string	RequestConfig::getIndex() const {
	return common_.index;
}

std::string	RequestConfig::getUploadStore() const {
	return common_.upload_store;
}

uint16_t	RequestConfig::getReturnStatus() const {
	return common_.return_status;
}

std::string	RequestConfig::getReturnUrl() const {
	return common_.return_url;
}

// CommonConfig::ErrorPagesMap	RequestConfig::getErrorPages() const {}

CommonConfig::BinaryHandlerPath
	RequestConfig::getCGIBin(std::string extension) const {
	return common_.cgi_assign.find(extension)->second;
}

bool RequestConfig::hasCGI(std::string extension) const {
	return common_.cgi_assign.count(extension) > 0;
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
