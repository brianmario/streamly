#include "streamly.h"

static size_t header_handler(char * stream, size_t size, size_t nmemb, VALUE out) {
    rb_str_buf_cat(out, stream, size * nmemb);
    // rb_funcall(callback, rb_intern("call"), 2, rb_str_new2("headers"), rb_str_new(stream, size * nmemb));
    return size * nmemb;
}

static size_t data_handler(char * stream, size_t size, size_t nmemb, VALUE out) {
    rb_str_buf_cat(out, stream, size * nmemb);
    // rb_funcall(callback, rb_intern("call"), 2, rb_str_new2("body"), rb_str_new(stream, size * nmemb));
    return size * nmemb;
}

static VALUE select_error(CURLcode code) {
    VALUE error = Qnil;
    
    switch (code) {
        case CURLE_UNSUPPORTED_PROTOCOL:
            error = eUnsupportedProtocol;
            break;
        case CURLE_URL_MALFORMAT:
            error = eURLFormatError;
            break;
        case CURLE_COULDNT_RESOLVE_HOST:
            error = eHostResolutionError;
            break;
        case CURLE_COULDNT_CONNECT:
            error = eConnectionFailed;
            break;
        case CURLE_PARTIAL_FILE:
            error = ePartialFileError;
            break;
        case CURLE_OPERATION_TIMEDOUT:
            error = eTimeoutError;
            break;
        case CURLE_TOO_MANY_REDIRECTS:
            error = eTooManyRedirects;
            break;
        default:
            error = eStreamlyError;
    }

    return error;
}

// head(url, headers = {})
VALUE rb_streamly_head(int argc, VALUE * argv, VALUE klass) {
    CURL *curl;
    CURLcode res;
    char error_buf[CURL_ERROR_SIZE];
    VALUE url = Qnil, headers = Qnil, header_buffer = Qnil, blk = Qnil;

    rb_scan_args(argc, argv, "11&", &url, &headers, &blk);
    
    // if (blk == Qnil) {
    //     // TODO: raise exception
    //     return Qnil;
    // }
    
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
        curl_easy_setopt(curl, CURLOPT_URL, RSTRING_PTR(url));
        
        // Header handling
        header_buffer = rb_str_new2("");
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, &header_handler);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, header_buffer);
        
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error_buf);

        res = curl_easy_perform(curl);
        if (CURLE_OK != res) {
            rb_raise(select_error(res), error_buf);
        }

        /* always cleanup */
        curl_easy_cleanup(curl);
    }

    return header_buffer;
}

// get(url, headers = {})
VALUE rb_streamly_get(int argc, VALUE * argv, VALUE klass) {
    CURL *curl;
    CURLcode res;
    char error_buf[CURL_ERROR_SIZE];
    VALUE body_buffer = Qnil, url = Qnil, headers = Qnil, header_buffer = Qnil, blk = Qnil;

    rb_scan_args(argc, argv, "11&", &url, &headers, &blk);
    
    // if (blk == Qnil) {
    //     // TODO: raise exception
    //     return Qnil;
    // }
    
    curl = curl_easy_init();
    if (curl) {
        body_buffer = rb_str_new2("");
        curl_easy_setopt(curl, CURLOPT_URL, RSTRING_PTR(url));
        
        // Header handling
        header_buffer = rb_str_new2("");
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, &header_handler);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, header_buffer);
        
        // Body handling
        body_buffer = rb_str_new2("");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, (curl_write_callback)&data_handler);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, body_buffer);

        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error_buf);

        res = curl_easy_perform(curl);
        if (CURLE_OK != res) {
            rb_raise(select_error(res), error_buf);
        }

        /* always cleanup */
        curl_easy_cleanup(curl);
    }

    return body_buffer;
}

// post(url, payload, headers = {})
VALUE rb_streamly_post(int argc, VALUE * argv, VALUE klass) {
    return Qnil;
}

// put(url, payload, headers = {})
VALUE rb_streamly_put(int argc, VALUE * argv, VALUE klass) {
    return Qnil;
}

// delete(url, headers = {})
VALUE rb_streamly_delete(int argc, VALUE * argv, VALUE klass) {
    CURL *curl;
    CURLcode res;
    char error_buf[CURL_ERROR_SIZE];
    VALUE body_buffer = Qnil, url = Qnil, headers = Qnil, header_buffer = Qnil, blk = Qnil;

    rb_scan_args(argc, argv, "11&", &url, &headers, &blk);
    
    // if (blk == Qnil) {
    //     // TODO: raise exception
    //     return Qnil;
    // }
    
    curl = curl_easy_init();
    if (curl) {
        body_buffer = rb_str_new2("");
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
        curl_easy_setopt(curl, CURLOPT_URL, RSTRING_PTR(url));
        
        // Header handling
        header_buffer = rb_str_new2("");
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, &header_handler);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, header_buffer);
        
        // Body handling
        body_buffer = rb_str_new2("");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, (curl_write_callback)&data_handler);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, body_buffer);

        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error_buf);

        res = curl_easy_perform(curl);
        if (CURLE_OK != res) {
            rb_raise(select_error(res), error_buf);
        }

        /* always cleanup */
        curl_easy_cleanup(curl);
    }

    return body_buffer;
}

// Ruby Extension initializer
void Init_streamly_ext() {
    mStreamly = rb_define_module("Streamly");

    rb_define_module_function(mStreamly, "head", rb_streamly_head, -1);
    rb_define_module_function(mStreamly, "get", rb_streamly_get, -1);
    rb_define_module_function(mStreamly, "post", rb_streamly_post, -1);
    rb_define_module_function(mStreamly, "put", rb_streamly_put, -1);
    rb_define_module_function(mStreamly, "delete", rb_streamly_delete, -1);
    
    eStreamlyError = rb_define_class_under(mStreamly, "Error", rb_eStandardError);
    eUnsupportedProtocol = rb_define_class_under(mStreamly, "UnsupportedProtocol", rb_eStandardError);
    eURLFormatError = rb_define_class_under(mStreamly, "URLFormatError", rb_eStandardError);
    eHostResolutionError = rb_define_class_under(mStreamly, "HostResolutionError", rb_eStandardError);
    eConnectionFailed = rb_define_class_under(mStreamly, "ConnectionFailed", rb_eStandardError);
    ePartialFileError = rb_define_class_under(mStreamly, "PartialFileError", rb_eStandardError);
    eTimeoutError = rb_define_class_under(mStreamly, "TimeoutError", rb_eStandardError);
    eTooManyRedirects = rb_define_class_under(mStreamly, "TooManyRedirects", rb_eStandardError);
}
