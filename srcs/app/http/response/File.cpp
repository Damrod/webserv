#include <File.hpp>

File::File(const std::string &file_path): file_path_(file_path) {
	SetPathExtension_();
	// Ver cómo gestionar esto en POST
	if (lstat(file_path_.c_str(), &statbuf_) != 0) {
		ThrowPathError_();
	}
}

std::string File::GetPathExtension() const {
	return path_extension_;
}

std::string File::GetPath() const {
	return file_path_;
}

std::string	File::GetMimeType() const {
	return MimeTypes::GetMimeType(path_extension_);
}

std::string	File::GetContent() {
	std::ifstream ifs(file_path_.c_str(), std::ios::in | std::ios::binary);
	if (!ifs) {
		ThrowPathError_();
	}
	return std::string(
					std::istreambuf_iterator<char>(ifs),
					std::istreambuf_iterator<char>());
}

void	File::SetSubpath(std::string subpath) {
	file_path_ += subpath;
	SetPathExtension_();
	if (lstat(file_path_.c_str(), &statbuf_) != 0) {
		ThrowPathError_();
	}
}

bool	File::IsRegularFile() const {
	return S_ISREG(statbuf_.st_mode);
}

bool	File::IsDirectory() const {
	return S_ISDIR(statbuf_.st_mode);
}

bool	File::HasEndSlash() const {
	return file_path_[file_path_.size() - 1] == '/';
}

void	File::Upload(std::string filename, std::string content) {
	const std::string upload_path = file_path_ + filename;
	std::ofstream out(upload_path.c_str());

	if (!out) {
		ThrowPathError_();
	}
	out.write(content.c_str(), content.size());
	if (!out) {
		throw File::Error(500);
	}
}

void	File::SetPathExtension_() {
	const std::size_t extension_position = file_path_.rfind(".");
	if (extension_position == std::string::npos || extension_position < 2) {
		path_extension_ = "";
		return;
	}
	const std::size_t	last_dir_position = file_path_.rfind("/");
	if (last_dir_position == std::string::npos) {
		path_extension_ = file_path_.substr(extension_position + 1);
		return;
	}
	if (last_dir_position < extension_position - 1) {
		const std::string last_path_part = file_path_.substr(last_dir_position + 1);
		if (last_path_part != "." && last_path_part != "..") {
			path_extension_ = file_path_.substr(extension_position + 1);
			return;
		}
	}
	path_extension_ = "";
}

void	File::ThrowPathError_() {
	if (errno == ENOENT || errno == ENOTDIR) {
		throw File::Error(404);
	} else if (errno == EACCES) {
		throw File::Error(403);
	} else {
		throw File::Error(500);
	}
}

std::string	File::GetDirectoryContent() {
	DIR *dir = opendir(file_path_.c_str());
	std::stringstream stream;

	if (dir == NULL) {
		ThrowPathError_();
	}
	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL) {
		std::string name = entry->d_name;
		if (name.rfind(".", 0) == 0) {
			continue;
		}
		File file(file_path_ + "/" + name);
		if (file.IsDirectory()) {
				name.append("/");
		}
		stream << "<a href=\"" << name << "\">" << name << "</a>\n";
	}
	closedir(dir);
	return stream.str();
}
