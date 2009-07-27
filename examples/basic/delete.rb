# encoding: UTF-8
$LOAD_PATH.unshift File.expand_path(File.dirname(__FILE__) + '/../../lib')

require 'streamly'

raise Exception, "You must pass a URL to request" if ARGV[0].nil?

url = ARGV[0]
puts Streamly.delete(url)