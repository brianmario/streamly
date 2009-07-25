#include <curl/curl.h>
#include <ruby.h>

VALUE mStreamly, cRequest, eStreamlyError, eUnsupportedProtocol, eURLFormatError, eHostResolutionError;
VALUE eConnectionFailed, ePartialFileError, eTimeoutError, eTooManyRedirects;
VALUE sym_method, sym_url, sym_payload, sym_headers, sym_head, sym_get, sym_post, sym_put, sym_delete;
VALUE sym_response_header_handler, sym_response_body_handler;

#define GetInstance(obj, sval) (sval = (struct curl_instance*)DATA_PTR(obj));

#ifdef HAVE_RBTRAP
    #include <rubysig.h>
#else
    void rb_enable_interrupt(void);
    void rb_disable_interrupt(void);

    #define TRAP_BEG rb_enable_interrupt();
    #define TRAP_END do { rb_disable_interrupt(); rb_thread_check_ints(); } while(0);
#endif

struct curl_instance {
    CURL* handle;
    char error_buffer[CURL_ERROR_SIZE];
    struct curl_slist* request_headers;
    VALUE request_payload_handler;
    VALUE response_header_handler;
    VALUE response_body_handler;
    VALUE options;
};

// libcurl callbacks
// static size_t header_handler(char * stream, size_t size, size_t nmemb, VALUE handler);
// static size_t data_handler(char * stream, size_t size, size_t nmemb, VALUE handler);
// static size_t put_data_handler(char * stream, size_t size, size_t nmemb, VALUE upload_stream);
// 
// static VALUE select_error(CURLcode code);
// static VALUE each_http_header(VALUE header, VALUE header_array);
void streamly_instance_mark(struct curl_instance *curl);
void streamly_instance_free(struct curl_instance *curl);

VALUE rb_streamly_new(VALUE klass);
VALUE rb_streamly_init(VALUE klass);

// VALUE rb_streamly_head(int argc, VALUE * argv, VALUE klass);
// VALUE rb_streamly_get(int argc, VALUE * argv, VALUE klass);
// VALUE rb_streamly_post(int argc, VALUE * argv, VALUE klass);
// VALUE rb_streamly_put(int argc, VALUE * argv, VALUE klass);
// VALUE rb_streamly_delete(int argc, VALUE * argv, VALUE klass); 