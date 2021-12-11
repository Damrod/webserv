#include <HttpPostResponse.hpp>

HttpPostResponse::HttpPostResponse(
	RequestConfig *request_config,
	HttpRequest *request) :
	HttpBaseResponse(request_config, request) {
	if (error_code_) {
		SetErrorRawResponse_(error_code_);
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
	const std::string decoded_path = request_->GetDecodedPath();
	if (request_config_->HasCGI(PathExtension(decoded_path))) {
		return request_config_->GetRoot() + decoded_path;
	} else if (IsUploadEnabled_() && IsValidUploadPath_(decoded_path)) {
		return request_config_->GetUploadStore();
	}
	SetErrorRawResponse_(404);
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
	return path == request_config_->GetPath();
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

void	HttpPostResponse::SetErrorRawResponse_(const int error_code) {
	raw_response_ = HttpErrorResponse(
									error_code,
									request_config_,
									request_).Content();
}
