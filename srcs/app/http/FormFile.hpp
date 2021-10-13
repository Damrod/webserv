#include <FormFile.hpp>
#include <stdexcept>
#include <StringUtils.hpp>

FormFile::FormFile(const HttpRequest &request) {
	ParseContentType_(request);
	ParseBody_(request);
}

// Syntax: Content-Type: multipart/form-data; boundary=something
void	FormFile::ParseContentType_(const HttpRequest &request) {
	if (!request.HasHeader("Content-Type")) {
		throw std::invalid_argument("[FormFile] Invalid Content-Type");
	}
	const std::string content_type = request.GetHeaderValue("Content-Type");
	std::size_t start = 0;
	std::size_t end = content_type.find(';');
	const std::string media_type = content_type.substr(start, end - start);
	if (TrimString(media_type, " ") != "multipart/form-data") {
		throw std::invalid_argument("[FormFile] Invalid media-type");
	}
	if (end == std::string::npos ||
			content_type.find(';', end + 1) != std::string::npos) {
		throw std::invalid_argument("[FormFile] Invalid boundary");
	}
	start = end + 1;
	const std::string directive = TrimString(content_type.substr(start));
	const std::string name = "boundary=";
	if (!directive.rfind(name, 0) == 0 || directive.length() == name.length()) {
		throw std::invalid_argument("[FormFile] Invalid boundary");
	}
	boundary_ = directive.substr(name.length() + 1);
}

/*
** https://datatracker.ietf.org/doc/html/rfc2046#section-5.1.1
POST /foo HTTP/1.1
Content-Length: 68137
Content-Type: multipart/form-data; boundary=something

--something
Content-Disposition: form-data; name="myFile"; filename="foo.txt"
Content-Type: text/plain

(content of the uploaded file foo.txt)
--something--
*/
/*
--something\r\nContent-Disposition: form-data; name=\"filename\"; filename=\"10.txt\"\r\nContent-Type: text/plain\r\n\r\nhello\n\r\n--something--\r\n
*/
void	FormFile::ParseBody_(const HttpRequest &request) {
	const std::string dash_boundary = "--";
	const std::string crlf = "\r\n";
	const std::string body = request.GetBody();

	// Skip first boundary
	const std::string form_part_start = dash_boundary + boundary_ + crlf;
	if (body.rfind(0, part_start) != 0) {
		throw std::invalid_argument("[FormFile] Invalid body");
	}
	std::size_t	headers_start = part_start.size();
	std::size_t headers_end = body.find(crlf + crlf, headers_start);

	std::size_t file_start = headers_end + crlf.size() * 2;
	std::string form_part_end =
						crlf + dash_boundary + boundary + dash_boundary + crlf;
	std::size_t file_end = body.find(part_end, file_start);
}
