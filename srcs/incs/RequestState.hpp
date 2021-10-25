#ifndef SRCS_INCS_REQUESTSTATE_HPP_
#define SRCS_INCS_REQUESTSTATE_HPP_

struct RequestState {
	enum State {
		kPartial,
		kComplete,
		kInvalid
	};
};

#endif  // SRCS_INCS_REQUESTSTATE_HPP_
