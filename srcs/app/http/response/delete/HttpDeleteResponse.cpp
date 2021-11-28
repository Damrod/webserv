#include <HttpDeleteResponse.hpp>

HttpDeleteResponse::HttpDeleteResponse(
	RequestConfig *requestConfig,
	HttpRequest *request) : HttpBaseResponse(requestConfig, request) {

	if (error_code_) {
		SetErrorRawResponse_(error_code_);
	}
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

void	HttpDeleteResponse::Delete_(File &file) {
	int remove_ret;

	if (file.IsDirectory()) {
		remove_ret = nftw(
						file.GetPath().c_str(),
						RemoveSingleFile_,
						64,
						FTW_DEPTH | FTW_PHYS);
	} else {
		remove_ret = RemoveSingleFile_(file.GetPath().c_str(), NULL, 0, NULL);
	}

	if (remove_ret) {
		SetErrorRawResponse_(500);
	} else {
		DefaultStatusResponse_(200);
	}
}

int	HttpDeleteResponse::RemoveSingleFile_(
								const char *fpath,
								const struct stat *sb,
								int typeflag,
								struct FTW *ftwbuf) {

	// cast to void when function not used as callback
	if (!sb) (void)sb;
	if (!typeflag) (void)typeflag;
	if (!ftwbuf) (void)ftwbuf;

    return remove(fpath);
}

void	HttpDeleteResponse::SetErrorRawResponse_(int error_code) {
	raw_response_ = HttpErrorResponse(
									error_code,
									request_config_,
									request_).Content();
}
