#include <HttpPostResponse.hpp>

HttpPostResponse::HttpPostResponse(
	RequestConfig *request_config,
	HttpRequest *request) :
	HttpBaseResponse(request_config, request) {
	if (status_code_ != 200) {
		SetErrorRawResponse_(status_code_);
	} else {
		const std::string full_path = ConstructFullPath_();
		if (!full_path.empty()) {
			try {
				File file(full_path);

				if (file.IsRegularFile()) {
					HandleCGI_(file);
				} else {
					Upload_(file);
				}
			} catch(File::Error & e) {
				SetErrorRawResponse_(e.what());
			}
		}
	}
}

std::string	HttpPostResponse::ConstructFullPath_() {
	const std::string request_path = request_config_->GetRequestPath();
	if (request_config_->HasCGI(PathExtension(request_path))) {
		return request_config_->GetRoot() + request_path;
	} else if (IsUploadEnabled_() && IsValidUploadPath_(request_path)) {
		return request_config_->GetUploadStore();
	}
	SetErrorRawResponse_(403);
	return "";
}

void	HttpPostResponse::HandleCGI_(const File &file) {
	if (request_config_->HasCGI(file.GetPathExtension())) {
		try {
			ExecuteCGI_(file);
		} catch (const std::exception &e) {
			SetErrorRawResponse_(500);
		}
	} else {
		SetErrorRawResponse_(501);
	}
}

bool	HttpPostResponse::IsUploadEnabled_() const {
	return !request_config_->GetUploadStore().empty();
}

bool	HttpPostResponse::IsValidUploadPath_(const std::string &path) const {
	return path == request_config_->GetLocationPath();
}

void	HttpPostResponse::Upload_(const File &file) {
	try {
		FormFile form_file(*request_);
		const std::string file_content = form_file.GetFileContent();

		file.Upload(form_file.GetFilename(), file_content);
		DefaultStatusResponse_(200);
	} catch (const std::exception &e) {
		SetErrorRawResponse_(400);
	} catch (File::Error &e) {
		SetErrorRawResponse_(e.what());
	}
}

void	HttpPostResponse::SetErrorRawResponse_(const int status_code) {
	raw_response_ = HttpErrorResponse(
									status_code,
									request_config_,
									request_).Content();
}
