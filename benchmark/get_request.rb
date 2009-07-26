# encoding: UTF-8

require 'net/http'
require 'lib/streamly'
require 'benchmark'

Benchmark.bm do |x|
  puts "shell out to curl"
  x.report do
    `curl -s --compressed www.google.com`
  end
  
  puts "streamly"
  x.report do
    Streamly.get 'www.google.com'
  end
  
  puts "net/http"
  req = Net::HTTP::Get.new('www.google.com')
  x.report do
    res = Net::HTTP.start('google.com', 80) {|http|
      http.request(req)
    }
    res.body
  end
end