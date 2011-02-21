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
TIMES = (ARGV[1] || 1).to_i

rest_client_opts = {"Accept-Encoding" => "identity, deflate, gzip"}

Benchmark.bmbm do |x|
  parser = Yajl::Parser.new
  parser.on_parse_complete = lambda {|obj| }
  x.report "Streamly" do
    TIMES.times do
      Streamly.get(url) do |chunk|
        parser << chunk
      end
    end
  end

  parser = Yajl::Parser.new
  parser.on_parse_complete = lambda {|obj| }
  x.report "Shell out to curl" do
    TIMES.times do
      parser.parse `curl -s --compressed #{url}`
    end
  end

  parser = Yajl::Parser.new
  parser.on_parse_complete = lambda {|obj| }
  x.report "rest-client" do
    TIMES.times do
      parser.parse RestClient.get(url, rest_client_opts)
    end
  end
end