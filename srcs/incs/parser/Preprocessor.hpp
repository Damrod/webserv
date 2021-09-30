#ifndef SRCS_INCS_PARSER_PREPROCESSOR_HPP_
#define SRCS_INCS_PARSER_PREPROCESSOR_HPP_

# include <cerrno>
# include <iostream>
# include <fstream>
# include <cstring>
# include <string>
# include <algorithm>
# include <cctype>
# include <list>
# include <map>
# include <vector>
# include <sstream>
# include <parser/Analyser.hpp>

class Preprocessor : public Analyser {
 public:
	explicit Preprocessor(const std::string &path);
	virtual ~Preprocessor(void);
	std::string GetFileBuffer(void);
 private:
	std::string preprocess(std::ifstream &file);
	std::string	path_;
	std::string	filebuff_;
};

#endif  // SRCS_INCS_PARSER_PREPROCESSOR_HPP_
