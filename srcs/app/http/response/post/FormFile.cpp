#include <FormFile.hpp>
#include <stdexcept>
#include <Utils.hpp>

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
	const std::string content_type = request.GetHeaderValue("Content-Type");
	std::size_t index = ParseMediaType_(content_type, 0, "multipart/form-data");
	ParseBoundary_(content_type, index);
}

/*
** Example body
--something\r\n
Content-Disposition: form-data; name="filename"; filename="foo.txt"\r\n
Content-Type: text/plain\r\n
\r\n
foobar\n\r\n
--something--\r\n
*/
void	FormFile::ParseRequestBody_(const HttpRequest &request) {
	const std::string dash_boundary = "--";
	const std::string body = request.GetBody();

	// Skip first boundary
	const std::string boundary_start = dash_boundary + boundary_
		+ Constants::kCRLF_;
	std::size_t headers_start = SkipWord_(body, 0, boundary_start);

	// Parse the headers to extract the filename
	const std::string body_delimiter = std::string(Constants::kCRLF_)
		+ Constants::kCRLF_;
	std::size_t headers_end = body.find(body_delimiter, headers_start);
	if (headers_end == std::string::npos) {
		throw std::invalid_argument("[FormFile] Invalid body headers");
	}
	headers_end += body_delimiter.size();
	std::string headers =
						body.substr(headers_start, headers_end - headers_start);
	ParseFormHeaders_(headers);

	// Parse the file content
	std::size_t file_start = headers_end;
	std::string form_part_end =
		Constants::kCRLF_ + dash_boundary + boundary_ + dash_boundary
		+ Constants::kCRLF_;
	std::size_t file_end = body.find(form_part_end, file_start);
	if (file_end == std::string::npos) {
		throw std::invalid_argument("[FormFile] Invalid file content");
	}
	file_content_ = body.substr(file_start, file_end - file_start);
}

/*
** Example of the form headers
Content-Disposition: form-data; name="myFile"; filename="foo.txt"\r\n
Content-Type: text/plain\r\n
\r\n
*/
void	FormFile::ParseFormHeaders_(const std::string &headers) {
	// Parse the content disposition
	std::size_t start = 0;
	std::size_t end = headers.find(Constants::kCRLF_);
	if (end == std::string::npos) {
		throw std::invalid_argument("[FormFile] Invalid Content-Disposition");
	}
	const std::string content_disposition = headers.substr(start, end);
	ParseFormContentDisposition_(TrimString(content_disposition,
											Constants::kWhitespace_));

	// Parse the Content-Type of the form data
	start = end + sizeof(Constants::kCRLF_) - 1;
	end = headers.find(std::string(Constants::kCRLF_) + Constants::kCRLF_, start);
	if (end == std::string::npos) {
		throw std::invalid_argument("[FormFile] Invalid form Content-Type");
	}
	const std::string content_type = headers.substr(start, end - start);
	ParseHeaderName_(content_type, 0, "content-type");

	// Verify that there are no more headers
	end += (sizeof(Constants::kCRLF_) - 1) * 2;
	if (end != headers.size()) {
		throw std::invalid_argument("[FormFile] Invalid form headers");
	}
}

// Syntax: Content-Disposition: form-data; name="myFile"; filename="foo.txt"
void	FormFile::ParseFormContentDisposition_(const std::string &header) {
	std::size_t index = ParseHeaderName_(header, 0, "content-disposition");
	index = ParseMediaType_(header, index, "form-data");

	// Parse the disposition name
	index = SkipWord_(header, index, "name=");
	ParseDoubleQuotedString_(header, &index);
	index = SkipWhitespace_(header, index);
	if (header[index] != ';') {
		throw std::invalid_argument("[FormFile] Invalid Content-Disposition");
	}
	index = SkipWhitespace_(header, index + 1);
	// Parse the disposition filename
	index = SkipWord_(header, index, "filename=");
	const std::string quoted_filename = ParseDoubleQuotedString_(header, &index);
	if (index != header.size()) {
		throw std::invalid_argument("[FormFile] Invalid Content-Disposition");
	}
	filename_ = TrimString(quoted_filename, "\"");
	if (!IsValidFilename(filename_)) {
		throw std::invalid_argument("[FormFile] Invalid filename");
	}
}

std::string
FormFile::ParseDoubleQuotedString_(const std::string &str,
									std::size_t *index) const {
	const char double_quote = '"';
	if (str[*index] != double_quote || str.size() < 3) {
		throw std::invalid_argument("[FormFile] Invalid value");
	}
	std::size_t start = *index;
	*index += 1;
	bool found = false;
	while (!found) {
		if (str[*index] == double_quote) {
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

std::size_t
FormFile::ParseHeaderName_(const std::string &str, std::size_t start,
							const std::string &name) const {
	std::size_t end = str.find(':', start);
	if (end == std::string::npos) {
		throw std::invalid_argument("[FormFile] Invalid header format");
	}
	const std::string header_name = str.substr(start, end - start);
	const std::string lwc_header_name =
							ToLowerString(TrimString(header_name,
													 Constants::kWhitespace_));
	const std::string lwc_name = ToLowerString(name);
	if (lwc_header_name != lwc_name) {
		throw std::invalid_argument("[FormFile] Invalid header: " + name);
	}
	return end + 1;
}

std::size_t
FormFile::ParseMediaType_(const std::string &str, std::size_t start,
							const std::string &name) const {
	start = SkipWhitespace_(str, start);
	std::size_t end = str.find(';', start);
	if (end == std::string::npos) {
		throw std::invalid_argument("[FormFile] Invalid media-type");
	}
	const std::string media_type = str.substr(start, end - start);
	if (TrimString(media_type, Constants::kWhitespace_) != name) {
		throw std::invalid_argument("[FormFile] Invalid media-type");
	}
	return SkipWhitespace_(str, end + 1);
}

void	FormFile::ParseBoundary_(const std::string &str, std::size_t index) {
	const std::string pair = str.substr(index);
	std::size_t start = SkipWord_(pair, 0, "boundary=");
	boundary_ = pair.substr(start);
	if (boundary_.empty() || boundary_.size() > 70) {
		throw std::invalid_argument("[FormFile] Invalid boundary");
	}
}

std::size_t	FormFile::SkipWord_(const std::string &str, std::size_t index,
										const std::string &word) const {
	if (str.rfind(word, index) != index) {
		throw std::invalid_argument(
				"[FormFile] Invalid identifier");
	}
	return index + word.size();
}

std::size_t
FormFile::SkipWhitespace_(const std::string &str, std::size_t index) const {
	index = str.find_first_not_of(Constants::kWhitespace_, index);
	if (index == std::string::npos) {
		throw std::invalid_argument("[FormFile] Invalid form");
	}
	return index;
}

bool	FormFile::IsValidFilename(const std::string &filename) const {
	return !filename.empty() && filename.find("/") == std::string::npos;
}
