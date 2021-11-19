#include <parser/Preprocessor.hpp>

Preprocessor::~Preprocessor(void) {
}

Preprocessor::Preprocessor(const std::string &path)
	: path_(path) {
	std::ifstream	file(path_.c_str(), std::ios::binary);
	if (!file)
		throw std::invalid_argument(strerror(errno));
	filebuff_ = Preprocess_(file);
	file.close();
}


std::string Preprocessor::Preprocess_(std::ifstream &file) {
	std::string buffer;
	std::string filebuff;
	bool	insidedquote = false;
	bool	insidesquote = false;

	while(std::getline(file, buffer)) {
		size_t commentpos = buffer.npos;
		for(unsigned int i = 0 ; i < buffer.size(); ++i) {
			if (buffer[i] == '\'' && !insidedquote)
				insidesquote ^= true;
			else if (buffer[i] == '\"' && !insidesquote)
				insidedquote ^= true;
			else
				if (buffer[i] == '#' && !insidedquote && !insidesquote) {
					commentpos = i;
					break;
				}
		}
		if (commentpos == buffer.npos)
			filebuff += buffer;
		else
			filebuff += buffer.substr(0, commentpos);
		if (!file.eof())
			filebuff += "\n";
	}
	return filebuff;
}

std::string Preprocessor::GetFileBuffer(void) {
	return filebuff_;
}
