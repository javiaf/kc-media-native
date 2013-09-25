
#include "log.h"
#include <stdio.h>

static char log_buf[LOG_BUF_SIZE];


// Media log level headers, indexed by media log level
const char * const media_log_level_headers[] = {
	"-",
	"VERBOSE",
	"DEBUG",
	"INFO",
	"WARN",
	"ERROR",
	"FATAL"
};

static void media_log_default_callback(MediaLogLevel level, const char *tag,
						const char *fmt, va_list vargs) {
	int media_log_level =  MEDIA_LOG_UNKNOWN;

	switch(level){
	case MEDIA_LOG_FATAL:
		media_log_level = MEDIA_LOG_FATAL;
		break;
	case MEDIA_LOG_ERROR:
		media_log_level = MEDIA_LOG_ERROR;
		break;
	case MEDIA_LOG_WARN:
		media_log_level = MEDIA_LOG_WARN;
		break;
	case MEDIA_LOG_INFO:
		media_log_level = MEDIA_LOG_INFO;
		break;
	case MEDIA_LOG_VERBOSE:
		media_log_level = MEDIA_LOG_VERBOSE;
		break;
	case MEDIA_LOG_DEBUG:
		media_log_level = MEDIA_LOG_DEBUG;
		break;
	}

	vsnprintf(log_buf, sizeof(log_buf), fmt, vargs);
	fprintf(stderr, "%s - %s: %s\n",
			media_log_level_headers[media_log_level], tag, log_buf);
}

static void (*media_log_callback)(MediaLogLevel level, const char *tag,
					const char *fmt, va_list vargs) =
	media_log_default_callback;

void media_log(MediaLogLevel level, const char *tag, const char *fmt, ...) {
	va_list vl;
	va_start(vl, fmt);
	media_vlog(level, tag, fmt, vl);
	va_end(vl);
}

void media_vlog(MediaLogLevel level, const char *tag, const char *fmt, va_list vl) {
	media_log_callback(level, tag, fmt, vl);
}

void media_log_set_callback(void (*callback)(MediaLogLevel level, const char *tag,
						const char *fmt, va_list vargs)) {
	media_log_callback = callback;
}
