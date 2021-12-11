#ifndef SRCS_INCS_CGIHANDLER_HPP_
#define SRCS_INCS_CGIHANDLER_HPP_
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include <stdexcept>
#include <CommonDefinitions.hpp>
#include <FDsets.hpp>
#include <File.hpp>
#include <HttpResponse.hpp>
#include <HttpStatusCodes.hpp>
#include <Utils.hpp>
#include <SyscallWrap.hpp>
#include <CgiInfo.hpp>

class CgiHandler {
	public:
		CgiHandler(FDsets *fd_sets, int socket, const CgiInfo &cgi_info);
		~CgiHandler();
		ssize_t	ReadCgiOutput();
		ssize_t	SendCgiOutput();
		int		GetSocket() const;
		int		GetCgiOutFd() const;
		bool	IsComplete() const;

	private:
		CgiHandler();
		CgiHandler(const CgiHandler &);
		CgiHandler &operator=(const CgiHandler &);

		void		TryParseHeaders_();
		void		ParseHeaders_();
		void		ParseHeader_(const std::string &header);
		std::string	ParseHeaderName_(const std::string &header) const;
		std::string	ParseHeaderValue_(const std::string &header) const;
		bool		IsValidHeaderName_(const std::string &header_name) const;
		bool		IsValidHeaderValue_(const std::string &header_value) const;
		bool		ContainOnlyVisibleChars_(const std::string &str) const;
		bool		HasHeader_(const std::string &header_name) const;
		std::string	GetHeaderValue_(const std::string &header_name) const;
		void		ValidateHeaders_();
		void		PrependHeaders_();
		void		ParseStatus_(const std::string &status_str);
		void		SetErrorResponse_(const std::size_t &error_code);
		std::string	GetErrorPagePath_(const std::size_t &error_code);
		void		DefaultStatusResponse_(const std::size_t &error_code);
		void		Serve_(const File &file, const std::size_t &error_code);
		void		SetResponse_(const int &code,
								const HttpResponse::HeadersMap &headers,
								const std::string &body);
		FDsets		*fd_sets_;
		int			socket_;
		CgiInfo		cgi_info_;
		bool		cgi_complete_;
		bool		headers_parsing_complete_;
		std::size_t	status_;
		HttpResponse::HeadersMap
					headers_;
		std::string	data_;
};

#endif  // SRCS_INCS_CGIHANDLER_HPP_
