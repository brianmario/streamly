#include "streamly.h"

size_t default_data_handler(char * stream, size_t size, size_t nmemb, VALUE out) {
    fprintf(stdout, stream);
    rb_str_buf_cat(out, stream, size * nmemb);
    return size * nmemb;
}

// head(url, headers = {})
VALUE rb_streamly_head(int argc, VALUE * argv, VALUE klass) {
    return Qnil;
}

// get(url, headers = {})
VALUE rb_streamly_get(int argc, VALUE * argv, VALUE klass) {
    CURL *curl;
    CURLcode res;
    VALUE body_buffer = Qnil;

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "http://search.twitter.com/search.json?q=github");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, (curl_write_callback)&default_data_handler);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)body_buffer);

        res = curl_easy_perform(curl);

      /* always cleanup */
        curl_easy_cleanup(curl);
    }

    return Qnil;
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
    return Qnil;
}

// Ruby Extension initializer
void Init_streamly_ext() {
    mStreamly = rb_define_module("Streamly");
    
    rb_define_module_function(mStreamly, "head", rb_streamly_get, -1);
    rb_define_module_function(mStreamly, "get", rb_streamly_get, -1);
    rb_define_module_function(mStreamly, "post", rb_streamly_post, -1);
    rb_define_module_function(mStreamly, "put", rb_streamly_put, -1);
    rb_define_module_function(mStreamly, "delete", rb_streamly_delete, -1);
}
