#ifndef _TRACE_LOG_H_
#define _TRACE_LOG_H_

typedef struct _cfiles_desc
{
    char* file_name;
    int     file_id;
} CFILES_DESC;

#define TRACE_LOG(_p_, _siz_, _struct_, _field_, _msg_) tl_trace_log(__BASE_FILE__, __LINE__, _p_, _siz_);

void tl_trace_log(char* file_name, int linenum, void* p, int size);
#endif
