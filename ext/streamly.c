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

void streamly_instance_mark(struct curl_instance * instance) {
    rb_gc_mark(instance->request_method);
    rb_gc_mark(instance->request_payload_handler);
    rb_gc_mark(instance->response_header_handler);
    rb_gc_mark(instance->response_body_handler);
    rb_gc_mark(instance->options);
}

void streamly_instance_free(struct curl_instance * instance) {
    curl_easy_cleanup(instance->handle);
    free(instance);
}

// Initially borrowed from Patron - http://github.com/toland/patron
// slightly modified for Streamly
static VALUE each_http_header(VALUE header, VALUE self) {
    struct curl_instance * instance;
    GetInstance(self, instance);
    VALUE name, value, header_str = Qnil;

    name = rb_obj_as_string(rb_ary_entry(header, 0));
    value = rb_obj_as_string(rb_ary_entry(header, 1));

    header_str = rb_str_plus(name, rb_str_new2(": "));
    header_str = rb_str_plus(header_str, value);

    instance->request_headers = curl_slist_append(instance->request_headers, RSTRING_PTR(header_str));
    rb_gc_mark(name);
    rb_gc_mark(value);
    rb_gc_mark(header_str);
    return Qnil;
}

// Initially borrowed from Patron - http://github.com/toland/patron
// slightly modified for Streamly
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

/*
 * Document-class: Streamly::Request
 *
 * A streaming REST client for Ruby that uses libcurl to do the heavy lifting.
 * The API is almost exactly like rest-client, so users of that library should find it very familiar.
 */
/*
 * Document-method: new
 *
 * call-seq: new(args)
 *
 * +args+ should be a Hash and is required
 *  This Hash should at least contain +:url+ and +:method+ keys.
 *  You may also provide the following optional keys:
 *    +:headers+ - should be a Hash of name/value pairs
 *    +:response_header_handler+ - can be a string or object that responds to #call
 *      If an object was passed, it's #call method will be called and passed the current chunk of data
 *    +:response_body_handler+ - can be a string or object that responds to #call
 *      If an object was passed, it's #call method will be called and passed the current chunk of data
 *    +:payload+ - If +:method+ is either +:post+ or +:put+ this will be used as the request body
 *
 */
VALUE rb_streamly_new(int argc, VALUE * argv, VALUE klass) {
    struct curl_instance * instance;
    VALUE obj = Data_Make_Struct(klass, struct curl_instance, streamly_instance_mark, streamly_instance_free, instance);
    rb_obj_call_init(obj, argc, argv);
    return obj;
}

/*
 * Document-method: initialize
 *
 * call-seq: initialize(args)
 *
 * +args+ should be a Hash and is required
 *  This Hash should at least contain +:url+ and +:method+ keys.
 *  You may also provide the following optional keys:
 *    +:headers+ - should be a Hash of name/value pairs
 *    +:response_header_handler+ - can be a string or object that responds to #call
 *      If an object was passed, it's #call method will be called and passed the current chunk of data
 *    +:response_body_handler+ - can be a string or object that responds to #call
 *      If an object was passed, it's #call method will be called and passed the current chunk of data
 *    +:payload+ - If +:method+ is either +:post+ or +:put+ this will be used as the request body
 *
 */
VALUE rb_streamly_init(int argc, VALUE * argv, VALUE self) {
    struct curl_instance * instance;
    char * credential_sep = ":";
    VALUE args, url, payload, headers, username, password, credentials;
    
    GetInstance(self, instance);
    instance->handle = curl_easy_init();
    instance->request_headers = NULL;
    instance->request_method = Qnil;
    instance->request_payload_handler = Qnil;
    instance->response_header_handler = Qnil;
    instance->response_body_handler = Qnil;
    instance->options = Qnil;
    
    rb_scan_args(argc, argv, "10", &args);
    
    // Ensure our args parameter is a hash
    Check_Type(args, T_HASH);
    
    instance->request_method = rb_hash_aref(args, sym_method);
    url = rb_hash_aref(args, sym_url);
    payload = rb_hash_aref(args, sym_payload);
    headers = rb_hash_aref(args, sym_headers);
    username = rb_hash_aref(args, sym_username);
    password = rb_hash_aref(args, sym_password);
    instance->response_header_handler = rb_hash_aref(args, sym_response_header_handler);
    instance->response_body_handler = rb_hash_aref(args, sym_response_body_handler);
    
    // First lets verify we have a :method key
    if (NIL_P(instance->request_method)) {
        rb_raise(eStreamlyError, "You must specify a :method");
    } else {
        // OK, a :method was specified, but if it's POST or PUT we require a :payload
        if (instance->request_method == sym_post || instance->request_method == sym_put) {
            if (NIL_P(payload)) {
                rb_raise(eStreamlyError, "You must specify a :payload for POST and PUT requests");
            }
        }
    }
    
    // Now verify a :url was provided
    if (NIL_P(url)) {
        rb_raise(eStreamlyError, "You must specify a :url to request");
    }
    
    if (NIL_P(instance->response_header_handler)) {
        instance->response_header_handler = rb_str_new2("");
    }
    if (instance->request_method != sym_head && NIL_P(instance->response_body_handler)) {
        instance->response_body_handler = rb_str_new2("");
    }
    
    if (!NIL_P(headers)) {
        Check_Type(headers, T_HASH);
        rb_iterate(rb_each, headers, each_http_header, self);
        curl_easy_setopt(instance->handle, CURLOPT_HTTPHEADER, instance->request_headers);
    }
    
    // So far so good, lets start setting up our request
    
    // Set the type of request
    if (instance->request_method == sym_head) {
        curl_easy_setopt(instance->handle, CURLOPT_NOBODY, 1);
    } else if (instance->request_method == sym_get) {
        curl_easy_setopt(instance->handle, CURLOPT_HTTPGET, 1);
    } else if (instance->request_method == sym_post) {
        curl_easy_setopt(instance->handle, CURLOPT_POST, 1);
        curl_easy_setopt(instance->handle, CURLOPT_POSTFIELDS, RSTRING_PTR(payload));
        curl_easy_setopt(instance->handle, CURLOPT_POSTFIELDSIZE, RSTRING_LEN(payload));
        
        // (multipart)
        // curl_easy_setopt(instance->handle, CURLOPT_HTTPPOST, 1);
        
        // TODO: get streaming upload working
        // curl_easy_setopt(instance->handle, CURLOPT_READFUNCTION, &put_data_handler);
        // curl_easy_setopt(instance->handle, CURLOPT_READDATA, &instance->upload_stream);
        // curl_easy_setopt(instance->handle, CURLOPT_INFILESIZE, len);
    } else if (instance->request_method == sym_put) {
        curl_easy_setopt(instance->handle, CURLOPT_CUSTOMREQUEST, "PUT");
        curl_easy_setopt(instance->handle, CURLOPT_POSTFIELDS, RSTRING_PTR(payload));
        curl_easy_setopt(instance->handle, CURLOPT_POSTFIELDSIZE, RSTRING_LEN(payload));
        
        // TODO: get streaming upload working
        // curl_easy_setopt(instance->handle, CURLOPT_UPLOAD, 1);
        // curl_easy_setopt(instance->handle, CURLOPT_READFUNCTION, &put_data_handler);
        // curl_easy_setopt(instance->handle, CURLOPT_READDATA, &instance->upload_stream);
        // curl_easy_setopt(instance->handle, CURLOPT_INFILESIZE, len);
    } else if (instance->request_method == sym_delete) {
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
    if (instance->request_method != sym_head) {
        curl_easy_setopt(instance->handle, CURLOPT_ENCODING, "identity, deflate, gzip");
        curl_easy_setopt(instance->handle, CURLOPT_WRITEFUNCTION, &data_handler);
        curl_easy_setopt(instance->handle, CURLOPT_WRITEDATA, instance->response_body_handler);
    }
    
    curl_easy_setopt(instance, CURLOPT_USERPWD, NULL);
    if (!NIL_P(username) || !NIL_P(password)) {
        credentials = rb_str_new2("");
        rb_str_buf_cat(credentials, RSTRING_PTR(username), RSTRING_LEN(username));
        rb_str_buf_cat(credentials, credential_sep, 1);
        rb_str_buf_cat(credentials, RSTRING_PTR(password), RSTRING_LEN(password));
        curl_easy_setopt(instance->handle, CURLOPT_HTTPAUTH, CURLAUTH_BASIC | CURLAUTH_DIGEST);
        curl_easy_setopt(instance->handle, CURLOPT_USERPWD, RSTRING_PTR(credentials));
        rb_gc_mark(credentials);
    }
    
    curl_easy_setopt(instance->handle, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(instance->handle, CURLOPT_SSL_VERIFYHOST, 0);
    
    curl_easy_setopt(instance->handle, CURLOPT_ERRORBUFFER, instance->error_buffer);
    
    return self;
}

/*
 * Document-method: rb_streamly_execute
 *
 * call-seq: rb_streamly_execute
 */
VALUE rb_streamly_execute(int argc, VALUE * argv, VALUE self) {
    CURLcode res;
    struct curl_instance * instance;
    GetInstance(self, instance);

    // Done setting up, lets do this!
    res = curl_easy_perform(instance->handle);
    if (CURLE_OK != res) {
        rb_raise(select_error(res), instance->error_buffer);
    }
    
    // Cleanup
    if (instance->request_headers != NULL) {
        curl_slist_free_all(instance->request_headers);
        instance->request_headers = NULL;
    }
    curl_easy_reset(instance->handle);
    instance->request_payload_handler = Qnil;

    if (instance->request_method == sym_head && TYPE(instance->response_header_handler) == T_STRING) {
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
    rb_define_singleton_method(cRequest, "new", rb_streamly_new, -1);
    rb_define_method(cRequest, "initialize", rb_streamly_init, -1);
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
    sym_username = ID2SYM(rb_intern("username"));
    sym_password = ID2SYM(rb_intern("password"));
    sym_response_header_handler = ID2SYM(rb_intern("response_header_handler"));
    sym_response_body_handler = ID2SYM(rb_intern("response_body_handler"));
}
