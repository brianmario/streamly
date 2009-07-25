// static size_t put_data_handler(char * stream, size_t size, size_t nmemb, VALUE upload_stream) {
//     size_t result = 0;
//     
//     // TODO
//     // Change upload_stream back to a VALUE
//     // if TYPE(upload_stream) == T_STRING - read at most "len" continuously
//     // if upload_stream is IO-like, read chunks of it
//     // OR
//     // if upload_stream responds to "each", use that?
//     
//     TRAP_BEG;
//     // if (upload_stream != NULL && *upload_stream != NULL) {
//     //     int len = size * nmemb;
//     //     char *s1 = strncpy(stream, *upload_stream, len);
//     //     result = strlen(s1);
//     //     *upload_stream += result;
//     // }
//     TRAP_END;
//     
//     return result;
// }

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

void streamly_instance_mark(struct curl_instance *curl) {
    rb_gc_mark(curl->request_payload_handler);
    rb_gc_mark(curl->response_header_handler);
    rb_gc_mark(curl->response_body_handler);
    rb_gc_mark(curl->options);
}

void streamly_instance_free(struct curl_instance *curl) {
    curl_easy_cleanup(curl->handle);
    free(curl);
}

static VALUE each_http_header(VALUE header, VALUE self) {
    struct curl_instance * instance;
    Data_Get_Struct(self, struct curl_instance, instance);

    VALUE name = rb_obj_as_string(rb_ary_entry(header, 0));
    VALUE value = rb_obj_as_string(rb_ary_entry(header, 1));

    VALUE header_str = Qnil;
    header_str = rb_str_plus(name, rb_str_new2(": "));
    header_str = rb_str_plus(header_str, value);

    instance->request_headers = curl_slist_append(instance->request_headers, RSTRING_PTR(header_str));
    return Qnil;
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

// Our constructor for ruby
VALUE rb_streamly_new(VALUE klass) {
    struct curl_instance* curl;
    VALUE obj = Data_Make_Struct(klass, struct curl_instance, streamly_instance_mark, streamly_instance_free, curl);
    rb_obj_call_init(obj, 0, 0);
    return obj;
}

VALUE rb_streamly_init(VALUE self) {
    struct curl_instance *instance;
    Data_Get_Struct(self, struct curl_instance, instance);
    instance->handle = curl_easy_init();
    instance->request_payload_handler = Qnil;
    instance->response_header_handler = Qnil;
    instance->response_body_handler = Qnil;
    instance->options = Qnil;
    return self;
}

VALUE rb_streamly_execute(int argc, VALUE * argv, VALUE self) {
    CURLcode res;
    struct curl_instance * instance;
    GetInstance(self, instance);
    VALUE args, method, url, payload, headers;
    
    rb_scan_args(argc, argv, "10", &args);
    
    // Ensure our args parameter is a hash
    Check_Type(args, T_HASH);
    
    method = rb_hash_aref(args, sym_method);
    url = rb_hash_aref(args, sym_url);
    payload = rb_hash_aref(args, sym_payload);
    headers = rb_hash_aref(args, sym_headers);
    instance->response_header_handler = rb_hash_aref(args, sym_response_header_handler);
    instance->response_body_handler = rb_hash_aref(args, sym_response_body_handler);
    
    // First lets verify we have a :method key
    if (NIL_P(method)) {
        rb_raise(eStreamlyError, "You must specify a :method");
    } else {
        // OK, a :method was specified, but if it's POST or PUT we require a :payload
        if (method == sym_post || method == sym_put) {
            if (NIL_P(payload)) {
                rb_raise(eStreamlyError, "You must specify a :payload for POST and PUT requests");
            }
        }
    }
    
    // Now verify a :url was provided
    if (NIL_P(url)) {
        rb_raise(eStreamlyError, "You must specify a :url to request");
    }
    
    instance->response_header_handler = rb_str_new2("");
    if (method != sym_head && NIL_P(instance->response_body_handler)) {
        instance->response_body_handler = rb_str_new2("");
    }
    
    if (!NIL_P(headers)) {
        Check_Type(headers, T_HASH);
        rb_iterate(rb_each, headers, each_http_header, self);
        curl_easy_setopt(instance->handle, CURLOPT_HTTPHEADER, instance->request_headers);
    }
    
    // So far so good, lets start setting up our request
    
    // Set the type of request
    if (method == sym_head) {
        curl_easy_setopt(instance->handle, CURLOPT_NOBODY, 1);
    } else if (method == sym_get) {
        curl_easy_setopt(instance->handle, CURLOPT_HTTPGET, 1);
    } else if (method == sym_post) {
        curl_easy_setopt(instance->handle, CURLOPT_POST, 1);
        curl_easy_setopt(instance->handle, CURLOPT_POSTFIELDS, RSTRING_PTR(payload));
        curl_easy_setopt(instance->handle, CURLOPT_POSTFIELDSIZE, RSTRING_LEN(payload));
        // (multipart)
        // curl_easy_setopt(instance->handle, CURLOPT_HTTPPOST, 1);
        // TODO: get streaming upload working
        // curl_easy_setopt(instance->handle, CURLOPT_READFUNCTION, &put_data_handler);
        // curl_easy_setopt(instance->handle, CURLOPT_READDATA, &instance->upload_stream);
        // curl_easy_setopt(instance->handle, CURLOPT_INFILESIZE, len);
    } else if (method == sym_put) {
        curl_easy_setopt(instance->handle, CURLOPT_CUSTOMREQUEST, "PUT");
        curl_easy_setopt(instance->handle, CURLOPT_POSTFIELDS, RSTRING_PTR(payload));
        curl_easy_setopt(instance->handle, CURLOPT_POSTFIELDSIZE, RSTRING_LEN(payload));
        // TODO: get streaming upload working
        // curl_easy_setopt(instance->handle, CURLOPT_UPLOAD, 1);
        // curl_easy_setopt(instance->handle, CURLOPT_READFUNCTION, &put_data_handler);
        // curl_easy_setopt(instance->handle, CURLOPT_READDATA, &instance->upload_stream);
        // curl_easy_setopt(instance->handle, CURLOPT_INFILESIZE, len);
    } else if (method == sym_delete) {
        curl_easy_setopt(instance->handle, CURLOPT_CUSTOMREQUEST, "DELETE");
    }
    
    // Other common options
    curl_easy_setopt(instance->handle, CURLOPT_URL, RSTRING_PTR(url));
    curl_easy_setopt(instance->handle, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(instance->handle, CURLOPT_MAXREDIRS, 3);
    
    // Response header handling
    curl_easy_setopt(instance->handle, CURLOPT_HEADERFUNCTION, &header_handler);
    curl_easy_setopt(instance->handle, CURLOPT_HEADERDATA, instance->response_header_handler);
    
    // Response body handling
    if (method != sym_head) {
        curl_easy_setopt(instance->handle, CURLOPT_ENCODING, "identity, deflate, gzip");
        curl_easy_setopt(instance->handle, CURLOPT_WRITEFUNCTION, &data_handler);
        curl_easy_setopt(instance->handle, CURLOPT_WRITEDATA, instance->response_body_handler);
    }
    
    curl_easy_setopt(instance->handle, CURLOPT_ERRORBUFFER, instance->error_buffer);
    
    
    // Done setting up, lets do this!
    res = curl_easy_perform(instance->handle);
    if (CURLE_OK != res) {
        rb_raise(select_error(res), instance->error_buffer);
    }
    
    // Cleanup
    if (!NIL_P(headers)) {
        curl_slist_free_all(instance->request_headers);
        instance->request_headers = NULL;
    }
    curl_easy_reset(instance->handle);
    instance->request_payload_handler = Qnil;

    if (method == sym_head && TYPE(instance->response_header_handler) == T_STRING) {
        return instance->response_header_handler;
    } else if (TYPE(instance->response_body_handler) == T_STRING) {
        return instance->response_body_handler;
    } else {
        return Qnil;
    }
}

// Ruby Extension initializer
void Init_streamly_ext() {
    mStreamly = rb_define_module("Streamly");
    
    cRequest = rb_define_class_under(mStreamly, "Request", rb_cObject);
    rb_define_singleton_method(cRequest, "new", rb_streamly_new, 0);
    rb_define_method(cRequest, "initialize", rb_streamly_init, 0);
    rb_define_method(cRequest, "execute", rb_streamly_execute, -1);
    
    eStreamlyError = rb_define_class_under(mStreamly, "Error", rb_eStandardError);
    eUnsupportedProtocol = rb_define_class_under(mStreamly, "UnsupportedProtocol", rb_eStandardError);
    eURLFormatError = rb_define_class_under(mStreamly, "URLFormatError", rb_eStandardError);
    eHostResolutionError = rb_define_class_under(mStreamly, "HostResolutionError", rb_eStandardError);
    eConnectionFailed = rb_define_class_under(mStreamly, "ConnectionFailed", rb_eStandardError);
    ePartialFileError = rb_define_class_under(mStreamly, "PartialFileError", rb_eStandardError);
    eTimeoutError = rb_define_class_under(mStreamly, "TimeoutError", rb_eStandardError);
    eTooManyRedirects = rb_define_class_under(mStreamly, "TooManyRedirects", rb_eStandardError);
    
    sym_method = ID2SYM(rb_intern("method"));
    sym_url = ID2SYM(rb_intern("url"));
    sym_payload = ID2SYM(rb_intern("payload"));
    sym_headers = ID2SYM(rb_intern("headers"));
    sym_head = ID2SYM(rb_intern("head"));
    sym_get = ID2SYM(rb_intern("get"));
    sym_post = ID2SYM(rb_intern("post"));
    sym_put = ID2SYM(rb_intern("put"));
    sym_delete = ID2SYM(rb_intern("delete"));
    sym_response_header_handler = ID2SYM(rb_intern("response_header_handler"));
    sym_response_body_handler = ID2SYM(rb_intern("response_body_handler"));
}
