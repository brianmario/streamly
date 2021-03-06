# A streaming REST client for Ruby that uses libcurl

## Features
* rest-client like API
* Streaming API allows the caller to be handed chunks of the response while it's being received
* uses Encoding.default_internal (otherwise falls back to utf-8) for strings it hands back in 1.9

## How to install

Nothing special about it, just:

`sudo gem install streamly`

## Example of use

### A basic HEAD request

``` ruby
Streamly.head 'www.somehost.com'
```

Or streaming

``` ruby
Streamly.head 'www.somehost.com' do |header_chunk|
  # do something with header_chunk
end
```

You can also pass a Hash of headers

``` ruby
Streamly.head 'www.somehost.com', {"User-Agent" => "Your Mom"}
```

### A basic GET request

``` ruby
Streamly.get 'www.somehost.com'
```

Or streaming

``` ruby
Streamly.get 'www.somehost.com' do |body_chunk|
  # do something with body_chunk
end
```

You can also pass a Hash of headers

``` ruby
Streamly.get 'www.somehost.com', {"User-Agent" => "Your Mom"}
```

### A basic POST request

``` ruby
Streamly.post 'www.somehost.com', 'blah=foo'
```

Or streaming

``` ruby
Streamly.post 'www.somehost.com', 'blah=foo' do |body_chunk|
  # do something with body_chunk
end
```

You can also pass a Hash of headers

``` ruby
Streamly.post 'www.somehost.com', 'blah=foo', {"User-Agent" => "Your Mom"}
```

### A basic PUT request

``` ruby
Streamly.put 'www.somehost.com', 'blah=foo'
```

Or streaming

``` ruby
Streamly.put 'www.somehost.com', 'blah=foo' do |body_chunk|
  # do something with body_chunk
end
```

You can also pass a Hash of headers

``` ruby
Streamly.put 'www.somehost.com', 'blah=foo', {"User-Agent" => "Your Mom"}
```

### A basic DELETE request

``` ruby
Streamly.delete 'www.somehost.com'
```

Or streaming

``` ruby
Streamly.delete 'www.somehost.com' do |body_chunk|
  # do something with body_chunk
end
```

You can also pass a Hash of headers

``` ruby
Streamly.delete 'www.somehost.com', {"User-Agent" => "Your Mom"}
```

== Benchmarks

Fetching 2,405,005 bytes of JSON from a local lighttpd server

* Streamly: 0.011s
* Shell out to curl: 0.046s
* rest-client: 0.205s

Streaming, and parsing 2,405,005 bytes of JSON from a local lighttpd server

* Streamly: 0.231s
* Shell out to curl: 0.341s
* rest-client: 0.447s

## Other Notes

This library was basically an exercise in dealing with libcurl in C.

## Special Thanks

There are quite a few *extremely* nice REST client libraries out there for Ruby today. I especially owe thanks
to the following projects. Without them I probably would have never had the inspiration to even take the time
to write this library. In Streamly, you'll find snippets of code, API patterns and examples from all 3 of these projects.
I'll do my best to make sure I give credit where it's due *in* the source. Please let me know if I've missed something!

* rest-client - https://github.com/adamwiggins/rest-client
* curb - https://github.com/taf2/curb
* patron - https://github.com/toland/patron

And again, the Github crew for this amazing service!
