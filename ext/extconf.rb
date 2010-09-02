# encoding: UTF-8
require 'mkmf'
require 'rbconfig'

# 1.9-only
have_func('rb_thread_blocking_region')

$CFLAGS << ' -DHAVE_RBTRAP' if have_var('rb_trap_immediate', ['ruby.h', 'rubysig.h'])
# add_define 'HAVE_RBTRAP' if have_var('rb_trap_immediate', ['ruby.h', 'rubysig.h'])

# Borrowed from taf2-curb
dir_config('curl')
if find_executable('curl-config')
  $CFLAGS << " #{`curl-config --cflags`.strip}"
  $LIBS << " #{`curl-config --libs`.strip}"
elsif !have_library('curl') or !have_header('curl/curl.h')
  fail <<-EOM
  Can't find libcurl or curl/curl.h

  Try passing --with-curl-dir or --with-curl-lib and --with-curl-include
  options to extconf.
  EOM
end

$CFLAGS << ' -Wall -Wextra -funroll-loops'
# $CFLAGS << ' -O0 -ggdb'

create_makefile("streamly_ext")
