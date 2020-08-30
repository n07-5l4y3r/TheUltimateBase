#pragma once

typedef enum __eCmdID {
	IGNORE = 0,
	ECHO,
	PING
} eCmdID;

typedef struct __sParam {
	unsigned __int64 ui64CMDid;
	//
	unsigned __int64 ui64CMDpBuf;
	unsigned __int64 ui64CMDsize;
	//
	unsigned __int64 ui64RPLpBuf;
	unsigned __int64 ui64RPLsize;
} sParam;