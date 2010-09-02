#include <curl/curl.h>
#include <ruby.h>

VALUE mStreamly, cRequest, eStreamlyError, eUnsupportedProtocol, eURLFormatError, eHostResolutionError;
VALUE eConnectionFailed, ePartialFileError, eTimeoutError, eTooManyRedirects;
VALUE sym_method, sym_url, sym_payload, sym_headers, sym_head, sym_get, sym_post, sym_put, sym_delete;
VALUE sym_response_header_handler, sym_response_body_handler, sym_username, sym_password;

#define GetInstance(obj, sval) (sval = (struct curl_instance*)DATA_PTR(obj));

struct curl_instance {
  CURL* handle;
  char error_buffer[CURL_ERROR_SIZE];
  struct curl_slist* request_headers;
  VALUE request_payload_handler;
  VALUE response_header_handler;
  VALUE response_body_handler;
  VALUE request_method;
  VALUE options;
};

// libcurl callbacks
static size_t header_handler(char * stream, size_t size, size_t nmemb, VALUE handler);
static size_t data_handler(char * stream, size_t size, size_t nmemb, VALUE handler);
// static size_t put_data_handler(char * stream, size_t size, size_t nmemb, VALUE upload_stream);
// 
static VALUE select_error(CURLcode code);
static VALUE each_http_header(VALUE header, VALUE header_array);
static void streamly_instance_mark(struct curl_instance * instance);
static void streamly_instance_free(struct curl_instance * instance);

static VALUE rb_streamly_new(int argc, VALUE * argv, VALUE klass);

#if defined(__GNUC__) && (__GNUC__ >= 3)
#define RB_STREAMLY_UNUSED __attribute__ ((unused))
#else
#define RB_STREAMLY_UNUSED
#endif

/*
 * partial emulation of the 1.9 rb_thread_blocking_region under 1.8,
 * this is enough for dealing with blocking I/O functions in the
 * presence of threads.
 */
#ifndef HAVE_RB_THREAD_BLOCKING_REGION

#include <rubysig.h>
#define RUBY_UBF_IO ((rb_unblock_function_t *)-1)
typedef void rb_unblock_function_t(void *);
typedef VALUE rb_blocking_function_t(void *);
static VALUE
rb_thread_blocking_region(
  rb_blocking_function_t *func, void *data1,
  RB_STREAMLY_UNUSED rb_unblock_function_t *ubf,
  RB_STREAMLY_UNUSED void *data2)
{
  VALUE rv;

  TRAP_BEG;
  rv = func(data1);
  TRAP_END;

  return rv;
}

#endif /* ! HAVE_RB_THREAD_BLOCKING_REGION */
