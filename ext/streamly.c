#include "streamly.h"

static size_t header_handler(char * stream, size_t size, size_t nmemb, VALUE handler) {
    TRAP_BEG;
    if(TYPE(handler) == T_STRING) {
        rb_str_buf_cat(handler, stream, size * nmemb);
    } else {
        rb_funcall(handler, rb_intern("call"), 1, rb_str_new(stream, size * nmemb));
    }
    TRAP_END;
    return size * nmemb;
}

static size_t data_handler(char * stream, size_t size, size_t nmemb, VALUE handler) {
    TRAP_BEG;
    if(TYPE(handler) == T_STRING) {
        rb_str_buf_cat(handler, stream, size * nmemb);
    } else {
        rb_funcall(handler, rb_intern("call"), 1, rb_str_new(stream, size * nmemb));
    }
    TRAP_END;
    return size * nmemb;
}

static size_t put_data_handler(char * stream, size_t size, size_t nmemb, char ** upload_stream) {
    size_t result = 0;
    
    // TODO
    // Change upload_stream back to a VALUE
    // if TYPE(upload_stream) == T_STRING - read at most "len" continuously
    // if upload_stream is IO-like, read chunks of it
    // OR
    // if upload_stream responds to "each", use that?
    
    TRAP_BEG;
    if (upload_stream != NULL && *upload_stream != NULL) {
        int len = size * nmemb;
        char *s1 = strncpy(stream, *upload_stream, len);
        result = strlen(s1);
        *upload_stream += result;
    }
    TRAP_END;
    
    return result;
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

static VALUE each_http_header(VALUE header, VALUE self) {
    struct curl_instance *instance;
    Data_Get_Struct(self, struct curl_instance, instance);

    VALUE name = rb_obj_as_string(rb_ary_entry(header, 0));
    VALUE value = rb_obj_as_string(rb_ary_entry(header, 1));

    VALUE header_str = Qnil;
    header_str = rb_str_plus(name, rb_str_new2(": "));
    header_str = rb_str_plus(header_str, value);

    instance->headers = curl_slist_append(instance->headers, RSTRING_PTR(header_str));
    return Qnil;
}

void streamly_instance_free(struct curl_instance *curl) {
    curl_easy_cleanup(curl->handle);
    free(curl);
}

// Our constructor for ruby
VALUE rb_streamly_new(VALUE klass) {
    struct curl_instance* curl;
    VALUE obj = Data_Make_Struct(klass, struct curl_instance, NULL, streamly_instance_free, curl);
    rb_obj_call_init(obj, 0, 0);
    return obj;
}
VALUE rb_streamly_init(VALUE self) {
    struct curl_instance *instance;
    Data_Get_Struct(self, struct curl_instance, instance);
    instance->handle = curl_easy_init();
    return self;
}

// head(url, headers = {})
VALUE rb_streamly_head(int argc, VALUE * argv, VALUE self) {
    CURLcode res;
    struct curl_instance *instance;
    GetInstance(self, instance);
    VALUE handler = Qnil, url = Qnil, headers = Qnil, blk = Qnil;
    
    rb_scan_args(argc, argv, "11&", &url, &headers, &blk);
    
    // Set handler to the passed block, so we can pass chunks to it
    // or just use a string so we can append the response to, and finally return it
    if (blk != Qnil) {
        handler = blk;
    } else {
        handler = rb_str_new2("");
    }
    
    if (instance->handle) {
        curl_easy_setopt(instance->handle, CURLOPT_FOLLOWLOCATION, 1);
        curl_easy_setopt(instance->handle, CURLOPT_MAXREDIRS, 3);
        
        curl_easy_setopt(instance->handle, CURLOPT_NOBODY, 1);
        curl_easy_setopt(instance->handle, CURLOPT_URL, RSTRING_PTR(url));
        
        // Header handling
        curl_easy_setopt(instance->handle, CURLOPT_HEADERFUNCTION, &header_handler);
        curl_easy_setopt(instance->handle, CURLOPT_HEADERDATA, handler);
        
        if (!NIL_P(headers)) {
            if (rb_type(headers) != T_HASH) {
              rb_raise(rb_eArgError, "Headers must be passed in as a hash.");
            }
            
            rb_iterate(rb_each, headers, each_http_header, self);
            
            curl_easy_setopt(instance->handle, CURLOPT_HTTPHEADER, instance->headers);
        }
        
        curl_easy_setopt(instance->handle, CURLOPT_ERRORBUFFER, instance->error_buf);
        
        res = curl_easy_perform(instance->handle);
        
        if (CURLE_OK != res) {
            rb_raise(select_error(res), instance->error_buf);
        }
        
        // always cleanup
        if (!NIL_P(headers)) {
            curl_slist_free_all(instance->headers);
            instance->headers = NULL;
        }
        curl_easy_reset(instance->handle);
        instance->upload_stream = NULL;
        
        if (TYPE(handler) == T_STRING) {
            return handler;
        }
    }
    return Qnil;
}

// get(url, headers = {})
VALUE rb_streamly_get(int argc, VALUE * argv, VALUE self) {
    CURLcode res;
    struct curl_instance *instance;
    GetInstance(self, instance);
    VALUE handler = Qnil, url = Qnil, headers = Qnil, blk = Qnil;
    
    rb_scan_args(argc, argv, "11&", &url, &headers, &blk);
    
    // Set handler to the passed block, so we can pass chunks to it
    // or just use a string so we can append the response to, and finally return it
    if (blk != Qnil) {
        handler = blk;
    } else {
        handler = rb_str_new2("");
    }
    
    if (instance->handle) {
        curl_easy_setopt(instance->handle, CURLOPT_FOLLOWLOCATION, 1);
        curl_easy_setopt(instance->handle, CURLOPT_MAXREDIRS, 3);
        
        curl_easy_setopt(instance->handle, CURLOPT_HTTPGET, 1);
        curl_easy_setopt(instance->handle, CURLOPT_ENCODING, "identity, deflate, gzip");
        curl_easy_setopt(instance->handle, CURLOPT_URL, RSTRING_PTR(url));
        
        // TODO: add support to get these to the caller
        // Header handling
        // curl_easy_setopt(instance->handle, CURLOPT_HEADERFUNCTION, &header_handler);
        // curl_easy_setopt(instance->handle, CURLOPT_HEADERDATA, handler);
        
        // Body handling
        curl_easy_setopt(instance->handle, CURLOPT_WRITEFUNCTION, (curl_write_callback)&data_handler);
        curl_easy_setopt(instance->handle, CURLOPT_WRITEDATA, handler);
        
        if (!NIL_P(headers)) {
            if (rb_type(headers) != T_HASH) {
              rb_raise(rb_eArgError, "Headers must be passed in as a hash.");
            }
            
            rb_iterate(rb_each, headers, each_http_header, self);
            
            curl_easy_setopt(instance->handle, CURLOPT_HTTPHEADER, instance->headers);
        }
        
        curl_easy_setopt(instance->handle, CURLOPT_ERRORBUFFER, instance->error_buf);
        
        res = curl_easy_perform(instance->handle);

        if (CURLE_OK != res) {
            rb_raise(select_error(res), instance->error_buf);
        }
        
        // always cleanup
        if (!NIL_P(headers)) {
            curl_slist_free_all(instance->headers);
            instance->headers = NULL;
        }
        curl_easy_reset(instance->handle);
        instance->upload_stream = NULL;
        
        if (TYPE(handler) == T_STRING) {
            return handler;
        }
    }
    return Qnil;
}

// post(url, payload, headers = {})
VALUE rb_streamly_post(int argc, VALUE * argv, VALUE self) {
    CURLcode res;
    struct curl_instance *instance;
    GetInstance(self, instance);
    VALUE handler = Qnil, url = Qnil, payload = Qnil, headers = Qnil, blk = Qnil;
    
    rb_scan_args(argc, argv, "21&", &url, &payload, &headers, &blk);
    
    // Set handler to the passed block, so we can pass chunks to it
    // or just use a string so we can append the response to, and finally return it
    if (blk != Qnil) {
        handler = blk;
    } else {
        handler = rb_str_new2("");
    }
    
    if (instance->handle) {
        curl_easy_setopt(instance->handle, CURLOPT_FOLLOWLOCATION, 1);
        curl_easy_setopt(instance->handle, CURLOPT_MAXREDIRS, 3);
        
        curl_easy_setopt(instance->handle, CURLOPT_POST, 1);
        curl_easy_setopt(instance->handle, CURLOPT_ENCODING, "identity, deflate, gzip");
        curl_easy_setopt(instance->handle, CURLOPT_URL, RSTRING_PTR(url));
        
        // TODO: add support to get these to the caller
        // Header handling
        // curl_easy_setopt(instance->handle, CURLOPT_HEADERFUNCTION, &header_handler);
        // curl_easy_setopt(instance->handle, CURLOPT_HEADERDATA, handler);

        // TODO: support "Transfer-Encoding: chunked" for request body
        // TODO: support CURLOPT_HTTPPOST (multipart/formdata)
        //
        // The following are optional, but allow you to send the POST body chunked
        // or even just allow you to stream it.
        // This is probably prefered for this library because it's all about streaming ;)
        // curl_easy_setopt(instance->handle, CURLOPT_UPLOAD, 1);
        // curl_easy_setopt(instance->handle, CURLOPT_READFUNCTION, &put_data_handler);
        // curl_easy_setopt(instance->handle, CURLOPT_READDATA, &instance->upload_stream);
        // curl_easy_setopt(instance->handle, CURLOPT_INFILESIZE, len);
        
        curl_easy_setopt(instance->handle, CURLOPT_POSTFIELDS, RSTRING_PTR(payload));
        curl_easy_setopt(instance->handle, CURLOPT_POSTFIELDSIZE, RSTRING_LEN(payload));
        
        // Body handling
        curl_easy_setopt(instance->handle, CURLOPT_WRITEFUNCTION, (curl_write_callback)&data_handler);
        curl_easy_setopt(instance->handle, CURLOPT_WRITEDATA, handler);
        
        if (!NIL_P(headers)) {
            if (rb_type(headers) != T_HASH) {
              rb_raise(rb_eArgError, "Headers must be passed in as a hash.");
            }
            
            rb_iterate(rb_each, headers, each_http_header, self);
            
            curl_easy_setopt(instance->handle, CURLOPT_HTTPHEADER, instance->headers);
        }
        
        curl_easy_setopt(instance->handle, CURLOPT_ERRORBUFFER, instance->error_buf);
        
        res = curl_easy_perform(instance->handle);
        
        if (CURLE_OK != res) {
            rb_raise(select_error(res), instance->error_buf);
        }
        
        // always cleanup
        if (!NIL_P(headers)) {
            curl_slist_free_all(instance->headers);
            instance->headers = NULL;
        }
        curl_easy_reset(instance->handle);
        instance->upload_stream = NULL;
        
        if (TYPE(handler) == T_STRING) {
            return handler;
        }
    }
    return Qnil;
}

// put(url, payload, headers = {})
VALUE rb_streamly_put(int argc, VALUE * argv, VALUE self) {
    CURLcode res;
    struct curl_instance *instance;
    GetInstance(self, instance);
    VALUE handler = Qnil, url = Qnil, payload = Qnil, headers = Qnil, blk = Qnil;
    
    rb_scan_args(argc, argv, "21&", &url, &payload, &headers, &blk);
    
    // Set handler to the passed block, so we can pass chunks to it
    // or just use a string so we can append the response to, and finally return it
    if (blk != Qnil) {
        handler = blk;
    } else {
        handler = rb_str_new2("");
    }
    
    if (instance->handle) {
        curl_easy_setopt(instance->handle, CURLOPT_FOLLOWLOCATION, 1);
        curl_easy_setopt(instance->handle, CURLOPT_MAXREDIRS, 3);
        
        curl_easy_setopt(instance->handle, CURLOPT_ENCODING, "identity, deflate, gzip");
        curl_easy_setopt(instance->handle, CURLOPT_URL, RSTRING_PTR(url));
        
        // TODO: add support to get these to the caller
        // Header handling
        // curl_easy_setopt(instance->handle, CURLOPT_HEADERFUNCTION, &header_handler);
        // curl_easy_setopt(instance->handle, CURLOPT_HEADERDATA, handler);
        
        // Let libcurl know this is an HTTP PUT request
        instance->upload_stream = RSTRING_PTR(payload);
        int len = RSTRING_LEN(payload);
        
        curl_easy_setopt(instance->handle, CURLOPT_UPLOAD, 1);
        curl_easy_setopt(instance->handle, CURLOPT_READFUNCTION, &put_data_handler);
        curl_easy_setopt(instance->handle, CURLOPT_READDATA, &instance->upload_stream);
        curl_easy_setopt(instance->handle, CURLOPT_INFILESIZE, len);
        
        // Body handling
        curl_easy_setopt(instance->handle, CURLOPT_WRITEFUNCTION, (curl_write_callback)&data_handler);
        curl_easy_setopt(instance->handle, CURLOPT_WRITEDATA, handler);
        
        if (!NIL_P(headers)) {
            if (rb_type(headers) != T_HASH) {
              rb_raise(rb_eArgError, "Headers must be passed in as a hash.");
            }
            
            rb_iterate(rb_each, headers, each_http_header, self);
            
            curl_easy_setopt(instance->handle, CURLOPT_HTTPHEADER, instance->headers);
        }
        
        curl_easy_setopt(instance->handle, CURLOPT_ERRORBUFFER, instance->error_buf);
        
        res = curl_easy_perform(instance->handle);
        
        if (CURLE_OK != res) {
            rb_raise(select_error(res), instance->error_buf);
        }
        
        // always cleanup
        if (!NIL_P(headers)) {
            curl_slist_free_all(instance->headers);
            instance->headers = NULL;
        }
        curl_easy_reset(instance->handle);
        instance->upload_stream = NULL;
        
        if (TYPE(handler) == T_STRING) {
            return handler;
        }
    }
    return Qnil;
}

// delete(url, headers = {})
VALUE rb_streamly_delete(int argc, VALUE * argv, VALUE self) {
    CURLcode res;
    struct curl_instance *instance;
    GetInstance(self, instance);
    VALUE handler = Qnil, url = Qnil, headers = Qnil, blk = Qnil;
    
    rb_scan_args(argc, argv, "11&", &url, &headers, &blk);
    
    // Set handler to the passed block, so we can pass chunks to it
    // or just use a string so we can append the response to, and finally return it
    if (blk != Qnil) {
        handler = blk;
    } else {
        handler = rb_str_new2("");
    }
    
    if (instance->handle) {
        curl_easy_setopt(instance->handle, CURLOPT_FOLLOWLOCATION, 1);
        curl_easy_setopt(instance->handle, CURLOPT_MAXREDIRS, 3);
        
        curl_easy_setopt(instance->handle, CURLOPT_CUSTOMREQUEST, "DELETE");
        curl_easy_setopt(instance->handle, CURLOPT_ENCODING, "identity, deflate, gzip");
        curl_easy_setopt(instance->handle, CURLOPT_URL, RSTRING_PTR(url));
        
        // TODO: add support to get these to the caller
        // Header handling
        // curl_easy_setopt(instance->handle, CURLOPT_HEADERFUNCTION, &header_handler);
        // curl_easy_setopt(instance->handle, CURLOPT_HEADERDATA, handler);
        
        // Body handling
        curl_easy_setopt(instance->handle, CURLOPT_WRITEFUNCTION, (curl_write_callback)&data_handler);
        curl_easy_setopt(instance->handle, CURLOPT_WRITEDATA, handler);
        
        if (!NIL_P(headers)) {
            if (rb_type(headers) != T_HASH) {
              rb_raise(rb_eArgError, "Headers must be passed in as a hash.");
            }
            
            rb_iterate(rb_each, headers, each_http_header, self);
            
            curl_easy_setopt(instance->handle, CURLOPT_HTTPHEADER, instance->headers);
        }
        
        curl_easy_setopt(instance->handle, CURLOPT_ERRORBUFFER, instance->error_buf);
        
        res = curl_easy_perform(instance->handle);
        
        if (CURLE_OK != res) {
            rb_raise(select_error(res), instance->error_buf);
        }
        
        // always cleanup
        if (!NIL_P(headers)) {
            curl_slist_free_all(instance->headers);
            instance->headers = NULL;
        }
        curl_easy_reset(instance->handle);
        instance->upload_stream = NULL;
        
        if (TYPE(handler) == T_STRING) {
            return handler;
        }
    }
    return Qnil;
}

// Ruby Extension initializer
void Init_streamly_ext() {
    mStreamly = rb_define_class("Streamly", rb_cObject);
    
    rb_define_singleton_method(mStreamly, "new", rb_streamly_new, 0);
    rb_define_method(mStreamly, "initialize", rb_streamly_init, 0);
    rb_define_method(mStreamly, "head", rb_streamly_head, -1);
    rb_define_method(mStreamly, "get", rb_streamly_get, -1);
    rb_define_method(mStreamly, "post", rb_streamly_post, -1);
    rb_define_method(mStreamly, "put", rb_streamly_put, -1);
    rb_define_method(mStreamly, "delete", rb_streamly_delete, -1);
    
    eStreamlyError = rb_define_class_under(mStreamly, "Error", rb_eStandardError);
    eUnsupportedProtocol = rb_define_class_under(mStreamly, "UnsupportedProtocol", rb_eStandardError);
    eURLFormatError = rb_define_class_under(mStreamly, "URLFormatError", rb_eStandardError);
    eHostResolutionError = rb_define_class_under(mStreamly, "HostResolutionError", rb_eStandardError);
    eConnectionFailed = rb_define_class_under(mStreamly, "ConnectionFailed", rb_eStandardError);
    ePartialFileError = rb_define_class_under(mStreamly, "PartialFileError", rb_eStandardError);
    eTimeoutError = rb_define_class_under(mStreamly, "TimeoutError", rb_eStandardError);
    eTooManyRedirects = rb_define_class_under(mStreamly, "TooManyRedirects", rb_eStandardError);
}
