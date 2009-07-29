# encoding: UTF-8
$LOAD_PATH.unshift File.expand_path(File.dirname(__FILE__) + '/../lib')

require 'rubygems'
require 'net/http'
require 'rest_client'
require 'streamly'
require 'benchmark'

url = ARGV[0]

Benchmark.bm do |x|
  puts "Streamly"
  x.report do
    (ARGV[1] || 1).to_i.times do
      Streamly.get(url)
    end
  end
  
  puts "Shell out to curl"
  x.report do
    (ARGV[1] || 1).to_i.times do
      `curl -s --compressed #{url}`
    end
  end
  
  puts "rest-client"
  x.report do
    (ARGV[1] || 1).to_i.times do
      RestClient.get(url, {"Accept-Encoding" => "identity, deflate, gzip"})
    end
  end
end