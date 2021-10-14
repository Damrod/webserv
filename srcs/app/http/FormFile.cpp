#include <FormFile.hpp>
#include <stdexcept>
#include <StringUtils.hpp>

const char FormFile::kCRLF[] = "\r\n";
const char FormFile::kWhitespace[] = " \t";

FormFile::FormFile(const HttpRequest &request) {
	ParseRequestContentType_(request);
	ParseRequestBody_(request);
}

std::string	FormFile::GetFilename() const {
	return filename_;
}

std::string	FormFile::GetFileContent() const {
	return file_content_;
}

// Syntax: Content-Type: multipart/form-data; boundary=something
void	FormFile::ParseRequestContentType_(const HttpRequest &request) {
	if (!request.HasHeader("Content-Type")) {
		throw std::invalid_argument("[FormFile] Invalid request Content-Type");
	}
	// Check the media-type, should be multipart/form-data
	const std::string content_type = request.GetHeaderValue("Content-Type");
	std::size_t start = 0;
	std::size_t end = content_type.find(';');
	const std::string media_type = content_type.substr(start, end - start);
	if (TrimString(media_type, kWhitespace) != "multipart/form-data") {
		throw std::invalid_argument("[FormFile] Invalid media-type");
	}

	// Check the boundary field
	if (end == std::string::npos ||
			content_type.find(';', end + 1) != std::string::npos) {
		throw std::invalid_argument("[FormFile] Invalid boundary");
	}
	start = end + 1;
	const std::string directive =
							TrimString(content_type.substr(start), kWhitespace);
	const std::string name = "boundary=";
	if (!directive.rfind(name, 0) == 0) {
		throw std::invalid_argument("[FormFile] Invalid boundary");
	}
	boundary_ = directive.substr(name.length());
	if (boundary_.empty() || boundary_.size() > 70) {
		throw std::invalid_argument("[FormFile] Invalid boundary");
	}
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
** Example body
--something\r\nContent-Disposition: form-data; name="filename"; filename="foo.txt"\r\nContent-Type: text/plain\r\n\r\nfoobar\n\r\n--something--\r\n
*/
void	FormFile::ParseRequestBody_(const HttpRequest &request) {
	const std::string dash_boundary = "--";
	const std::string body = request.GetBody();

	// Skip first boundary
	const std::string form_part_start = dash_boundary + boundary_ + kCRLF;
	if (body.rfind(form_part_start, 0) != 0) {
		throw std::invalid_argument("[FormFile] Invalid body boundary");
	}

	// Parse the headers to extract the filename
	std::size_t	headers_start = form_part_start.size();
	std::size_t headers_end = body.find(
									std::string(kCRLF) + kCRLF, headers_start);
	if (headers_end == std::string::npos) {
		throw std::invalid_argument("[FormFile] Invalid body headers");
	}
	headers_end += (sizeof(kCRLF) - 1) * 2;
	std::string headers =
						body.substr(headers_start, headers_end - headers_start);
	ParseFormHeaders_(headers);

	// Parse the file content
	std::size_t file_start = headers_end;
	std::string form_part_end =
					kCRLF + dash_boundary + boundary_ + dash_boundary + kCRLF;
	std::size_t file_end = body.find(form_part_end, file_start);
	file_content_ = body.substr(file_start, file_end - file_start);
}

/*
** Example of the form headers
Content-Disposition: form-data; name="myFile"; filename="foo.txt"\r\nContent-Type: text/plain\r\n\r\n
*/
void	FormFile::ParseFormHeaders_(const std::string &headers) {
	// Parse the content disposition
	std::size_t start = 0;
	std::size_t end = headers.find(kCRLF);
	if (end == std::string::npos) {
		throw std::invalid_argument("[FormFile] Invalid Content-Disposition");
	}
	const std::string content_disposition = headers.substr(start, end);
	ParseFormContentDisposition_(content_disposition);

	// Parse the Content-Type of the form data
	start = end + sizeof(kCRLF) - 1;
	end = headers.find(std::string(kCRLF) + kCRLF, start);
	if (end == std::string::npos) {
		throw std::invalid_argument("[FormFile] Invalid form Content-Type");
	}
	const std::string content_type = headers.substr(start, end - start);
	ParseFormContentType_(content_type);

	// Check that there are no more headers
	end += (sizeof(kCRLF) - 1) * 2;
	if (end != headers.size()) {
		throw std::invalid_argument("[FormFile] Invalid form headers");
	}
}

// Syntax: Content-Disposition: form-data; name="myFile"; filename="foo.txt"
void	FormFile::ParseFormContentDisposition_(const std::string &header) {
	// Parse header name
	std::size_t start = 0;
	std::size_t end = header.find(':');
	const std::string header_name = header.substr(start, end - start);
	const std::string lwc_header_name =
							ToLowerString(TrimString(header_name, kWhitespace));
	if (lwc_header_name != "content-disposition" || end == std::string::npos) {
		throw std::invalid_argument("[FormFile] Invalid form header");
	}

	// Parse the disposition type
	start = end + 1;
	end = header.find(';', start);
	if (end == std::string::npos) {
		throw std::invalid_argument("[FormFile] Invalid Content-Disposition");
	}
	const std::string disposition_type = header.substr(start, end - start);
	if (TrimString(disposition_type, kWhitespace) != "form-data") {
		throw std::invalid_argument(
				"[FormFile] Invalid Content-Disposition type");
	}

	// Parse the disposition name
	std::size_t index = end + 1;
	index = SkipSpace(header, index);
	std::string name = "name=";
	if (header.rfind(name, index) != index) {
		throw std::invalid_argument(
				"[FormFile] Invalid Content-Disposition name");
	}
	index += name.size();
	ParseDoubleQuotedString(header, &index);
	index = SkipSpace(header, index);
	if (index >= header.size() || header[index] != ';') {
		throw std::invalid_argument("[FormFile] Invalid Content-Disposition");
	}
	index += 1;
	index = SkipSpace(header, index);

	// Parse the disposition filename
	name = "filename=";
	if (header.rfind(name, index) != index) {
		throw std::invalid_argument(
				"[FormFile] Invalid Content-Disposition filename");
	}
	index += name.size();
	const std::string quoted_filename = ParseDoubleQuotedString(header, &index);
	filename_ = TrimString(quoted_filename, "\"");
	if (index != header.size()) {
		throw std::invalid_argument("[FormFile] Invalid Content-Disposition");
	}
}

// Syntax: Content-Type: text/plain
void	FormFile::ParseFormContentType_(const std::string &header) const {
	// Parse header name
	std::size_t start = 0;
	std::size_t end = header.find(':');
	const std::string header_name = header.substr(start, end - start);
	const std::string lwc_header_name =
							ToLowerString(TrimString(header_name, kWhitespace));
	if (lwc_header_name != "content-type" || end == std::string::npos) {
		throw std::invalid_argument("[FormFile] Invalid form header");
	}
}

std::size_t	FormFile::SkipSpace(const std::string &str, std::size_t idx) const {
	return str.find_first_not_of(kWhitespace, idx);
}

std::string
FormFile::ParseDoubleQuotedString(const std::string &str,
									std::size_t *index) const {
	const char double_quote = '"';
	const char backslash = '\\';
	if (str[*index] != double_quote || str.size() < 3) {
		throw std::invalid_argument("[FormFile] Invalid value");
	}
	std::size_t start = *index;
	*index += 1;
	bool found = false;
	while (!found) {
		if (str[*index] == backslash) {
			if (*index + 2 >= str.size()) {
				throw std::invalid_argument("[FormFile] Missing closing quote");
			}
			*index += 2;
		} else if (str[*index] == double_quote) {
			found = true;
		} else if (*index < str.size()) {
			++(*index);
		} else {
			throw std::invalid_argument("[FormFile] Missing closing quote");
		}
	}
	++(*index);
	const std::string unquoted_str = str.substr(start, *index - start);
	return unquoted_str;
}
