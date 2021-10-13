#ifndef SRCS_INCS_CONNECTIONIOSTATUS_HPP_
#define SRCS_INCS_CONNECTIONIOSTATUS_HPP_

struct ReadRequestStatus {
	enum Type {
		kFail,
		kSuccess,
		kStart
	};
};

struct SendResponseStatus {
	enum Type {
		kFail,
		kSuccess,
		kCompleteClose,
		kCompleteKeep
	};
};

#endif  // SRCS_INCS_CONNECTIONIOSTATUS_HPP_
