#ifndef SRCS_INCS_HTTPREDIRECTIONRESPONSE_HPP_
#define SRCS_INCS_HTTPREDIRECTIONRESPONSE_HPP_

#include <string>
#include <IResponse.hpp>
#include <HttpResponse.hpp>
#include <HttpRequest.hpp>
#include <ctime>
#include <RequestConfig.hpp>

class HttpRedirectionResponse: public IResponse {
	public:
		HttpRedirectionResponse(
			RequestConfig *requestConfig,
			HttpRequest *request);
		std::string content();

	private:
		// common
		void	AddCommonHeaders_(HttpResponse *response);
		void	CheckKeepAlive_();
		std::string	CurrentDate_() const;
		std::string DefaultResponseBody_(const std::size_t status_code) const;

		RequestConfig *requestConfig_;
		HttpRequest *request_;
		bool	keep_alive_;
};

#endif  // SRCS_INCS_HTTPREDIRECTIONRESPONSE_HPP_
