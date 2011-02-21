# encoding: UTF-8
$LOAD_PATH.unshift File.expand_path(File.dirname(__FILE__) + '/..')
$LOAD_PATH.unshift File.expand_path(File.dirname(__FILE__) + '/../lib')

require 'rubygems'
require 'net/http'
require 'rest_client'
require 'streamly'
require 'benchmark'

url = ARGV[0]
TIMES = (ARGV[1] || 1).to_i

rest_client_opts = {"Accept-Encoding" => "identity, deflate, gzip"}

Benchmark.bmbm do |x|
  x.report "Streamly" do
    TIMES.times do
      Streamly.get(url)
    end
  end

  x.report "Shell out to curl" do
    TIMES.times do
      `curl -s --compressed #{url}`
    end
  end

  x.report "rest-client" do
    TIMES.times do
      RestClient.get(url, rest_client_opts)
    end
  end
end