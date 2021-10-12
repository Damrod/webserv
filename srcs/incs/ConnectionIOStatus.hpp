#ifndef SRCS_INCS_CONNECTIONIOSTATUS_HPP_
#define SRCS_INCS_CONNECTIONIOSTATUS_HPP_

struct ReadRequestStatus {
	typedef enum Type {
		kFail,
		kSuccess,
		kStart
	} Type;
};

struct SendResponseStatus {
	typedef enum Type {
		kFail,
		kSuccess,
		kCompleteClose,
		kCompleteKeep
	} Type;
};

#endif  // SRCS_INCS_CONNECTIONIOSTATUS_HPP_
