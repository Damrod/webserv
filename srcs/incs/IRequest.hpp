#ifndef SRCS_INCS_IREQUEST_HPP_
#define SRCS_INCS_IREQUEST_HPP_
#include <string>
#include <RequestState.hpp>

class IRequest {
	public:
		virtual std::size_t	ParseRawString(const std::string &raw_string) = 0;
		virtual RequestState::State	GetState() const = 0;
		virtual void		Reset() = 0;
		virtual				~IRequest() {}
};

#endif  // SRCS_INCS_IREQUEST_HPP_
