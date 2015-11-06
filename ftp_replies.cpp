#include <ftp_replies.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>

static FTPReplyFormatString ftpReplyFormatStrings[] =
{
	#define FTP_REPLY_XMACRO(replyID, replyFormatString) replyFormatString,
	#include "ftp_replies.def"
	NULL
};

bool formatFTPReply(FTPReplyID replyID, char *replyBuffer, size_t replyBufferLength, ...)
{
	va_list args;
	va_start(args, replyBufferLength);

	int bytesWritten = vsnprintf(replyBuffer, replyBufferLength, ftpReplyFormatStrings[replyID], args);

	va_end (args);

	return bytesWritten > 0 && bytesWritten < replyBufferLength;
}
