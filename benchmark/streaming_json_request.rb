# encoding: UTF-8
$LOAD_PATH.unshift File.expand_path(File.dirname(__FILE__) + '/..')
$LOAD_PATH.unshift File.expand_path(File.dirname(__FILE__) + '/../lib')

require 'rubygems'
require 'net/http'
require 'rest_client'
require 'streamly'
require 'yajl'
require 'benchmark'

url = ARGV[0]

Benchmark.bmbm do |x|
  parser = Yajl::Parser.new
  parser.on_parse_complete = lambda {|obj| }
  x.report do
    puts "Streamly"
    (ARGV[1] || 1).to_i.times do
      Streamly.get(url) do |chunk|
        parser << chunk
      end
    end
  end

  parser = Yajl::Parser.new
  parser.on_parse_complete = lambda {|obj| }
  x.report do
    puts "Shell out to curl"
    (ARGV[1] || 1).to_i.times do
      parser.parse `curl -s --compressed #{url}`
    end
  end

  parser = Yajl::Parser.new
  parser.on_parse_complete = lambda {|obj| }
  x.report do
    puts "rest-client"
    (ARGV[1] || 1).to_i.times do
      parser.parse RestClient.get(url, {"Accept-Encoding" => "identity, deflate, gzip"})
    end
  end
end