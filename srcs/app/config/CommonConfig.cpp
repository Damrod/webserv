#include <CommonConfig.hpp>

CommonConfig::CommonConfig()
	: root("html"), client_max_body_size(1000000), autoindex(false),
	index("index.html") {
}
