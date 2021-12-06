#include <HttpDeleteResponse.hpp>

HttpDeleteResponse::HttpDeleteResponse(
	RequestConfig *requestConfig,
	HttpRequest *request) : HttpBaseResponse(requestConfig, request) {
	if (error_code_) {
		SetErrorRawResponse_(error_code_);
	} else {
		const std::string full_path =
				request_config_->GetRoot() + request_->GetDecodedPath();
		try {
			File file(full_path);

			if (file.IsRegularFile() || file.HasEndSlash()) {
				Delete_(file);
			} else {
				SetErrorRawResponse_(409);
			}
		} catch(File::Error & e) {
			SetErrorRawResponse_(e.what());
		}
	}
}

void	HttpDeleteResponse::Delete_(File const &file) {
	if (file.IsDirectory()) {
		BuildResponse_(
			nftw(file.GetPath().c_str(),
					RemoveSingleFile_,
					64,
					FTW_DEPTH | FTW_PHYS));
	} else {
		BuildResponse_(
			RemoveSingleFile_(file.GetPath().c_str(), NULL, 0, NULL));
	}
}

int	HttpDeleteResponse::RemoveSingleFile_(
								const char *fpath,
								__attribute__((unused)) const struct stat *sb,
								__attribute__((unused)) int typeflag,
								__attribute__((unused)) struct FTW *ftwbuf) {
    return remove(fpath);
}

void	HttpDeleteResponse::BuildResponse_(int return_status) {
	if (return_status) {
		errno == EACCES
			? SetErrorRawResponse_(403)
			: SetErrorRawResponse_(500);
	} else {
		DefaultStatusResponse_(200);
	}
}

void	HttpDeleteResponse::SetErrorRawResponse_(int error_code) {
	raw_response_ = HttpErrorResponse(
									error_code,
									request_config_,
									request_).Content();
}
