#include <RequestLocation.hpp>
#include <sys/types.h>

RequestLocation::RequestLocation(const ServerConfig &server_config,
		const std::string &request_path)
	: location_(FindLocation_(server_config, request_path)),
	common(location_ ? location_->common : server_config.common),
	limit_except(location_ ? &location_->limit_except : NULL) {
}

bool	RequestLocation::HasLocation() const {
	return location_ != NULL;
}

const Location*
RequestLocation::FindLocation_(const ServerConfig &server_config,
										const std::string &request_path) const {
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
