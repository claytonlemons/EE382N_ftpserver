#include "ftp_replies.h"

#include "dynamic_string.h"
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>

typedef const char *const FTPReplyFormatString;

static FTPReplyFormatString ftpReplyFormatStrings[] =
{
	#define FTP_REPLY_XMACRO(replyID, replyFormatString) replyFormatString,
	#include "ftp_replies.def"
	NULL
};

bool formatFTPReply(FTPReplyID replyID, DynamicString *reply, ...)
{
	va_list args;
	va_start(args, reply);

	int bytesWritten = vsnprintf(reply->buffer, reply->length, ftpReplyFormatStrings[replyID], args);
	bool formattingSuccessful = false;

	if (bytesWritten < 0) // formatting error
	{
		formattingSuccessful = false;
	}
	else if (bytesWritten >= reply->length) // buffer is too small
	{
		if (resizeDynamicString(reply, bytesWritten + 1)) // resize buffer and try again
		{
			vsnprintf(reply->buffer, reply->length, ftpReplyFormatStrings[replyID], args);
			formattingSuccessful = true;
		}
		else // resizing failed, must be out of memory!
		{
			formattingSuccessful = false;
		}
	}
	else // everything is OK!
	{
		formattingSuccessful = true;
	}

	va_end (args);

	return formattingSuccessful;
}
