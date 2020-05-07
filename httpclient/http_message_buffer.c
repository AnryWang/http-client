/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2020-05-05 17:21:58
 * @LastEditTime: 2020-05-07 17:59:38
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */

#include <http_message_buffer.h>
#include <http_log.h>
#include <http_error.h>
#include <platform_memory.h>

/* grows for given size */
void _http_message_buffer_grow(http_message_buffer_t *buf, size_t newsize) 
{
    if (newsize > buf->length) {
        /* if it's not big enough already... */
        buf->length = ((newsize / HTTP_MESSAGE_BUFFER_GROWTH) + 1) * HTTP_MESSAGE_BUFFER_GROWTH;

        buf->data = platform_memory_realloc(buf->data, buf->length);
    }
}

http_message_buffer_t *http_message_buffer_init(size_t size)
{
    http_message_buffer_t *buf;

    buf = platform_memory_alloc(sizeof(*buf));
    
    buf->data = platform_memory_alloc(size);
    buf->data[0] = '\0';
    buf->length = size;
    buf->used = 1;      /* use 1 byte space in advance to prevent overflow */     
    
    return buf;
}

int http_message_buffer_reinit(http_message_buffer_t *buf)
{
    HTTP_ROBUSTNESS_CHECK(buf , HTTP_NULL_VALUE_ERROR);

    memset(buf->data, 0, buf->length);
    buf->data[0] = '\0';
    buf->used = 1;

    RETURN_ERROR(HTTP_SUCCESS_ERROR);
}

/* concatenate all given strings on to the end of the buf. the strings must all be NULL terminated */
void http_message_buffer_concat(http_message_buffer_t *buf, ...)
{
    va_list ap;
    ssize_t total;

    va_start(ap, buf);
    total = buf->used + http_utils_count_concat(&ap);
    va_end(ap);    

    /* grow the buffer */
    _http_message_buffer_grow(buf, total);
    
    va_start(ap, buf);    
    http_utils_concat(buf->data + buf->used - 1, &ap);
    va_end(ap); 
    
    buf->used = total;
    buf->data[total - 1] = '\0';
}

void http_message_buffer_append(http_message_buffer_t *buf, const char *str, size_t len) 
{
    _http_message_buffer_grow(buf, buf->used + len);
    memcpy(buf->data + buf->used - 1, str, len);
    buf->used += len;
    buf->data[buf->used - 1] = '\0';
}


