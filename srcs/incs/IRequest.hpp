#ifndef SRCS_INCS_IREQUEST_HPP_
#define SRCS_INCS_IREQUEST_HPP_
#include <string>
#include <RequestState.hpp>

class IRequest {
	public:
		virtual				~IRequest() {}
		virtual	void		SetContent(const std::string &raw_request) = 0;
		virtual std::size_t	ParsedOffset() const = 0;
		virtual RequestState::State	GetState() const = 0;
		virtual void		Reset() = 0;
		virtual bool		IsPartial() const = 0;
		virtual bool		IsComplete() const = 0;
		virtual bool		IsInvalid() const = 0;
};

#endif  // SRCS_INCS_IREQUEST_HPP_
