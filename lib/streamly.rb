# encoding: UTF-8
require 'streamly_ext'

module Streamly
  VERSION = "0.1.0"
  
  class Request
    def self.execute(args)
      new.execute(args)
    end
  end
  
  def self.head(url, headers=nil, &block)
    opts = {:method => :head, :url => url, :headers => headers}
    opts.merge!({:response_header_handler => block}) if block_given?
    Request.execute(opts)
  end
  
  def self.get(url, headers=nil, &block)
    opts = {:method => :get, :url => url, :headers => headers}
    opts.merge!({:response_body_handler => block}) if block_given?
    Request.execute(opts)
  end

  def self.post(url, payload, headers=nil, &block)
    opts = {:method => :post, :url => url, :payload => payload, :headers => headers}
    opts.merge!({:response_body_handler => block}) if block_given?
    Request.execute(opts)
  end

  def self.put(url, payload, headers=nil, &block)
    opts = {:method => :put, :url => url, :payload => payload, :headers => headers}
    opts.merge!({:response_body_handler => block}) if block_given?
    Request.execute(opts)
  end

  def self.delete(url, headers={}, &block)
    opts = {:method => :delete, :url => url, :headers => headers}
    opts.merge!({:response_body_handler => block}) if block_given?
    Request.execute(opts)
  end
end