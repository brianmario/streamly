#include <curl/curl.h>
#include <ruby.h>

VALUE mStreamly, eStreamlyError, eUnsupportedProtocol, eURLFormatError, eHostResolutionError;
VALUE eConnectionFailed, ePartialFileError, eTimeoutError, eTooManyRedirects;

#define GetInstance(obj, sval) (sval = (struct curl_instance*)DATA_PTR(obj));

struct curl_instance {
    CURL* handle;
    char* upload_buf;
    char error_buf[CURL_ERROR_SIZE];
    struct curl_slist* headers;
};
  
static size_t header_handler(char * stream, size_t size, size_t nmemb, VALUE handler);
static size_t data_handler(char * stream, size_t size, size_t nmemb, VALUE handler);
static VALUE select_error(CURLcode code);
static VALUE each_http_header(VALUE header, VALUE header_array);
void streamly_instance_free(struct curl_instance *curl);

VALUE rb_streamly_new(VALUE klass);
VALUE rb_streamly_init(VALUE klass);

VALUE rb_streamly_head(int argc, VALUE * argv, VALUE klass);
VALUE rb_streamly_get(int argc, VALUE * argv, VALUE klass);
VALUE rb_streamly_post(int argc, VALUE * argv, VALUE klass);
VALUE rb_streamly_put(int argc, VALUE * argv, VALUE klass);
VALUE rb_streamly_delete(int argc, VALUE * argv, VALUE klass); 