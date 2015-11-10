#ifndef FTP_REPLIES_H_
#define FTP_REPLIES_H_

#include "dynamic_string.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef enum
{
	#define FTP_REPLY_XMACRO(replyID, replyFormatString) FTPREPLYID_##replyID,
	#include "ftp_replies.def"
	UNKNOWN_REPLY
} FTPReplyID;


// Example usage:
//
// int minutes = 10;
// formatFTPReply(FTPREPLYID_120, replyBuffer, replyBufferLength, minutes) ->
//
// replyBuffer contains "Service ready in 10 minutes."
//
// Refer to ftp_replies.def for detials on the format arguments
//
// Returns true if the formatted string was written completely to the buffer.
// If false, either the client should allocate a larger buffer and try again, or an
// encoding error was encountered
bool formatFTPReply(FTPReplyID replyID, DynamicString *replyBuffer, ...);

#endif
