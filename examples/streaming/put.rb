# encoding: UTF-8
$LOAD_PATH.unshift File.expand_path(File.dirname(__FILE__) + '/../../lib')

require 'streamly'

raise Exception, "You must pass a URL to request" if ARGV[0].nil?
raise Exception, "You must pass a payload to PUT" if ARGV[1].nil?

url = ARGV[0]
payload = ARGV[1]
resp = ''
Streamly.put(url, payload) do |chunk|
  STDOUT.write chunk
  STDOUT.flush
end
puts