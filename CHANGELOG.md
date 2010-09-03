# Changelog

## 0.1.4 (September 2nd, 2010)
* Streamly now respects Encoding.default_internal on 1.9
* fixed a random segfault when applying a NULL auth field
* take advantage of rb_thread_blocking_region on 1.9 and emulate it's behavior on 1.8 for Ruby-aware thread-saftey

## 0.1.3 (August 19th, 2009)
* Fixed a bug where a username or password was specified, but not both

## 0.1.2 (July 28th, 2009)
* removing call to set CURLOPT_USERPWD to NULL as it would crash in linux

## 0.1.1 (July 27th, 2009)
* Version bump for Github's gem builder

## 0.1.0 (July 27th, 2009)
* Initial release