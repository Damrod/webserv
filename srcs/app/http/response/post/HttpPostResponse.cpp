#include "HttpPostResponse.hpp"

HttpPostResponse::HttpPostResponse(
	RequestConfig *request_config,
	HttpRequest *request) :
	HttpBaseResponse(request_config, request) {
	if (error_code_) {
			SetErrorRawResponse_(error_code_);
	} else {
		const std::string full_path =
								request_config_->GetRoot() + request_->GetPath();
		// Mirar este try-catch en POST?
		try {
			File file(full_path);

			if (file.IsRegularFile()) {
				HandleCGI_(file);
			} else {
				HandleUpload_(file);
			}
		} catch(File::Error & e) {
			SetErrorRawResponse_(e.what());
		}
	}
}

void	HttpPostResponse::HandleCGI_(File file) {
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

void	HttpPostResponse::HandleUpload_(File file) {
	if (IsUploadEnabled_() && IsValidUploadPath_(request_->GetPath())) {
		Upload_(file);
	} else {
		SetErrorRawResponse_(404);
	}
}

bool	HttpPostResponse::IsUploadEnabled_() const {
	return !request_config_->GetUploadStore().empty();
}

bool	HttpPostResponse::IsValidUploadPath_(const std::string &path) const {
	return path == request_config_->GetPath();
}

void	HttpPostResponse::Upload_(File file) {
	try {
		FormFile form_file(*request_);
		const std::string upload_path =
								request_config_->GetUploadStore() +
								form_file.GetFilename();
		const std::string file_content = form_file.GetFileContent();

		file.Upload(upload_path, file_content);
		DefaultStatusResponse_(200);
	} catch (const std::exception &e) {
		SetErrorRawResponse_(400);
	} catch (File::Error &e) {
		SetErrorRawResponse_(e.what());
	}
}

void	HttpPostResponse::SetErrorRawResponse_(int error_code) {
	raw_response_ = HttpErrorResponse(
									error_code,
									request_config_,
									request_).Content();
}
