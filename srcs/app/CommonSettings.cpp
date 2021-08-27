#include <CommonSettings.hpp>

CommonSettings::CommonSettings()
	: root("html"), client_max_body_size(1000000), autoindex(false),
	index("index.html") {
}
