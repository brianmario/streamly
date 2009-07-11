#include "streamly.h"

static size_t header_handler(char * stream, size_t size, size_t nmemb, VALUE handler) {
    if(TYPE(handler) == T_STRING) {
        rb_str_buf_cat(handler, stream, size * nmemb);
    } else {
        rb_funcall(handler, rb_intern("call"), 1, rb_str_new(stream, size * nmemb));
    }
    return size * nmemb;
}

static size_t data_handler(char * stream, size_t size, size_t nmemb, VALUE handler) {
    if(TYPE(handler) == T_STRING) {
        rb_str_buf_cat(handler, stream, size * nmemb);
    } else {
        rb_funcall(handler, rb_intern("call"), 1, rb_str_new(stream, size * nmemb));
    }
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
    VALUE handler = Qnil, url = Qnil, headers = Qnil, blk = Qnil;

    rb_scan_args(argc, argv, "11&", &url, &headers, &blk);
    
    if (blk != Qnil) {
        handler = blk;
    } else {
        handler = rb_str_new2("");
    }
    
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
        curl_easy_setopt(curl, CURLOPT_URL, RSTRING_PTR(url));
        
        // Header handling
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, &header_handler);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, handler);

        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error_buf);

        res = curl_easy_perform(curl);
        if (CURLE_OK != res) {
            rb_raise(select_error(res), error_buf);
        }

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    
    if (TYPE(handler) == T_STRING) {
        return handler;
    }
    return Qnil;
}

// get(url, headers = {})
VALUE rb_streamly_get(int argc, VALUE * argv, VALUE klass) {
    CURL *curl;
    CURLcode res;
    char error_buf[CURL_ERROR_SIZE];
    VALUE handler = Qnil, url = Qnil, headers = Qnil, blk = Qnil;

    rb_scan_args(argc, argv, "11&", &url, &headers, &blk);
    
    if (blk != Qnil) {
        handler = blk;
    } else {
        handler = rb_str_new2("");
    }
    
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, RSTRING_PTR(url));
        curl_easy_setopt(curl, CURLOPT_ENCODING, "identity,deflate,gzip");
        
        // Body handling
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, (curl_write_callback)&data_handler);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, handler);

        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error_buf);

        res = curl_easy_perform(curl);
        if (CURLE_OK != res) {
            rb_raise(select_error(res), error_buf);
        }

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    
    if (TYPE(handler) == T_STRING) {
        return handler;
    }
    return Qnil;
}

// post(url, payload, headers = {})
VALUE rb_streamly_post(int argc, VALUE * argv, VALUE klass) {
    CURL *curl;
    CURLcode res;
    char error_buf[CURL_ERROR_SIZE];
    VALUE handler = Qnil, url = Qnil, payload = Qnil, headers = Qnil, blk = Qnil;

    rb_scan_args(argc, argv, "21&", &url, &payload, &headers, &blk);
    
    if (blk != Qnil) {
        handler = blk;
    } else {
        handler = rb_str_new2("");
    }
    
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, RSTRING_PTR(url));
        curl_easy_setopt(curl, CURLOPT_ENCODING, "identity,deflate,gzip");

        curl_easy_setopt(curl, CURLOPT_POST, 1);
        
        // TODO: support "Transfer-Encoding: chunked" for request body
        // TODO: support CURLOPT_HTTPPOST (multipart/formdata)
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, RSTRING_PTR(payload));
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, RSTRING_LEN(payload));
        
        // Body handling
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, (curl_write_callback)&data_handler);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, handler);

        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error_buf);

        res = curl_easy_perform(curl);
        if (CURLE_OK != res) {
            rb_raise(select_error(res), error_buf);
        }

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    
    if (TYPE(handler) == T_STRING) {
        return handler;
    }
    return Qnil;
}

// put(url, payload, headers = {})
VALUE rb_streamly_put(int argc, VALUE * argv, VALUE klass) {
    CURL *curl;
    CURLcode res;
    char error_buf[CURL_ERROR_SIZE];
    VALUE handler = Qnil, url = Qnil, payload = Qnil, headers = Qnil, blk = Qnil;

    rb_scan_args(argc, argv, "21&", &url, &payload, &headers, &blk);
    
    if (blk != Qnil) {
        handler = blk;
    } else {
        handler = rb_str_new2("");
    }
    
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, RSTRING_PTR(url));
        curl_easy_setopt(curl, CURLOPT_ENCODING, "identity,deflate,gzip");
        
        // VALUE data = rb_iv_get(request, "@upload_data");
        // state->upload_buf = StringValuePtr(data);
        // int len = RSTRING_LEN(data);
        
        // deprecated, apparently
        // curl_easy_setopt(curl, CURLOPT_PUT, 1);
        
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1);
        // curl_easy_setopt(curl, CURLOPT_READFUNCTION, &session_read_handler);
        // curl_easy_setopt(curl, CURLOPT_READDATA, &state->upload_buf);
        // curl_easy_setopt(curl, CURLOPT_INFILESIZE, len);
        
        // Body handling
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, (curl_write_callback)&data_handler);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, handler);

        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error_buf);

        res = curl_easy_perform(curl);
        if (CURLE_OK != res) {
            rb_raise(select_error(res), error_buf);
        }

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    
    if (TYPE(handler) == T_STRING) {
        return handler;
    }
    return Qnil;
}

// delete(url, headers = {})
VALUE rb_streamly_delete(int argc, VALUE * argv, VALUE klass) {
    CURL *curl;
    CURLcode res;
    char error_buf[CURL_ERROR_SIZE];
    VALUE handler = Qnil, url = Qnil, headers = Qnil, blk = Qnil;

    rb_scan_args(argc, argv, "11&", &url, &headers, &blk);
    
    if (blk != Qnil) {
        handler = blk;
    } else {
        handler = rb_str_new2("");
    }
    
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
        curl_easy_setopt(curl, CURLOPT_URL, RSTRING_PTR(url));
        curl_easy_setopt(curl, CURLOPT_ENCODING, "identity,deflate,gzip");
        
        // Body handling
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, (curl_write_callback)&data_handler);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, handler);

        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error_buf);

        res = curl_easy_perform(curl);
        if (CURLE_OK != res) {
            rb_raise(select_error(res), error_buf);
        }

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    
    if (TYPE(handler) == T_STRING) {
        return handler;
    }
    return Qnil;
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
