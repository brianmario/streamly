#include <curl/curl.h>
#include <ruby.h>

VALUE mStreamly, eStreamlyError, eUnsupportedProtocol, eURLFormatError, eHostResolutionError,
      eConnectionFailed, ePartialFileError, eTimeoutError, eTooManyRedirects;

static size_t header_handler(char * stream, size_t size, size_t nmemb, VALUE out);
static size_t data_handler(char * stream, size_t size, size_t nmemb, VALUE out);
static VALUE select_error(CURLcode code);

VALUE rb_streamly_head(int argc, VALUE * argv, VALUE klass);
VALUE rb_streamly_get(int argc, VALUE * argv, VALUE klass);
VALUE rb_streamly_post(int argc, VALUE * argv, VALUE klass);
VALUE rb_streamly_put(int argc, VALUE * argv, VALUE klass);
VALUE rb_streamly_delete(int argc, VALUE * argv, VALUE klass);
 