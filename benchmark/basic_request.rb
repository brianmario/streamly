# encoding: UTF-8
$LOAD_PATH.unshift File.expand_path(File.dirname(__FILE__) + '/../lib')

require 'net/http'
require 'streamly'
require 'benchmark'

Benchmark.bm do |x|
  puts "shell out to curl"
  x.report do
    (ARGV[0] || 1).to_i.times do
      `curl -s --compressed www.google.com`
    end
  end
  
  puts "streamly"
  x.report do
    (ARGV[0] || 1).to_i.times do
      Streamly.get 'www.google.com'
    end
  end
  
  puts "net/http"
  x.report do
    (ARGV[0] || 1).to_i.times do
      req = Net::HTTP::Get.new('www.google.com')
      res = Net::HTTP.start('google.com', 80)  do |http|
        http.request(req)
      end
      res.body
    end
  end
end