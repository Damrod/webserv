#include <CommonConfig.hpp>

CommonConfig::CommonConfig()
	: root("html"), client_max_body_size(1000000), autoindex(false),
	index("index.html"), error_pages(), upload_store(""), return_status(),
	return_url(""), cgi_assign() {
}
