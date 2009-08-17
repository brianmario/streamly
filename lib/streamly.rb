# encoding: UTF-8
require 'streamly_ext'

module Streamly
  VERSION = "0.1.2"
  
  class Request
    # A helper method to make your fire-and-forget requests easier
    #
    # Parameters:
    # +args+ should be a Hash and is required
    #   This Hash should at least contain +:url+ and +:method+ keys.
    #   You may also provide the following optional keys:
    #     +:headers+ - should be a Hash of name/value pairs
    #     +:response_header_handler+ - can be a string or object that responds to #call
    #       If an object was passed, it's #call method will be called and passed the current chunk of data
    #     +:response_body_handler+ - can be a string or object that responds to #call
    #       If an object was passed, it's #call method will be called and passed the current chunk of data
    #     +:payload+ - If +:method+ is either +:post+ or +:put+ this will be used as the request body
    #
    def self.execute(args)
      new(args).execute
    end
  end
  
  # A helper method to make HEAD requests a dead-simple one-liner
  #
  # Example:
  #   Streamly.head("www.somehost.com/some_resource/1")
  #
  #   Streamly.head("www.somehost.com/some_resource/1") do |header_chunk|
  #     # do something with _header_chunk_
  #   end
  #
  # Parameters:
  # +url+ should be a String, the url to request
  # +headers+ should be a Hash and is optional
  # 
  # This method also accepts a block, which will stream the response headers in chunks to the caller
  def self.head(url, headers=nil, &block)
    opts = {:method => :head, :url => url, :headers => headers}
    opts.merge!({:response_header_handler => block}) if block_given?
    Request.execute(opts)
  end
  
  # A helper method to make HEAD requests a dead-simple one-liner
  #
  # Example:
  #   Streamly.get("www.somehost.com/some_resource/1")
  #
  #   Streamly.get("www.somehost.com/some_resource/1") do |chunk|
  #     # do something with _chunk_
  #   end
  #
  # Parameters:
  # +url+ should be a String, the url to request
  # +headers+ should be a Hash and is optional
  # 
  # This method also accepts a block, which will stream the response body in chunks to the caller
  def self.get(url, headers=nil, &block)
    opts = {:method => :get, :url => url, :headers => headers}
    opts.merge!({:response_body_handler => block}) if block_given?
    Request.execute(opts)
  end
  
  # A helper method to make HEAD requests a dead-simple one-liner
  #
  # Example:
  #   Streamly.post("www.somehost.com/some_resource", "asset[id]=2&asset[name]=bar")
  #
  #   Streamly.post("www.somehost.com/some_resource", "asset[id]=2&asset[name]=bar") do |chunk|
  #     # do something with _chunk_
  #   end
  #
  # Parameters:
  # +url+ should be a String (the url to request) and is required
  # +payload+ should be a String and is required
  # +headers+ should be a Hash and is optional
  # 
  # This method also accepts a block, which will stream the response body in chunks to the caller
  def self.post(url, payload, headers=nil, &block)
    opts = {:method => :post, :url => url, :payload => payload, :headers => headers}
    opts.merge!({:response_body_handler => block}) if block_given?
    Request.execute(opts)
  end
  
  # A helper method to make HEAD requests a dead-simple one-liner
  #
  # Example:
  #   Streamly.put("www.somehost.com/some_resource/1", "asset[name]=foo")
  #
  #   Streamly.put("www.somehost.com/some_resource/1", "asset[name]=foo") do |chunk|
  #     # do something with _chunk_
  #   end
  #
  # Parameters:
  # +url+ should be a String (the url to request) and is required
  # +payload+ should be a String and is required
  # +headers+ should be a Hash and is optional
  # 
  # This method also accepts a block, which will stream the response body in chunks to the caller
  def self.put(url, payload, headers=nil, &block)
    opts = {:method => :put, :url => url, :payload => payload, :headers => headers}
    opts.merge!({:response_body_handler => block}) if block_given?
    Request.execute(opts)
  end
  
  # A helper method to make HEAD requests a dead-simple one-liner
  #
  # Example:
  #   Streamly.delete("www.somehost.com/some_resource/1")
  #
  #   Streamly.delete("www.somehost.com/some_resource/1") do |chunk|
  #     # do something with _chunk_
  #   end
  #
  # Parameters:
  # +url+ should be a String, the url to request
  # +headers+ should be a Hash and is optional
  # 
  # This method also accepts a block, which will stream the response body in chunks to the caller
  def self.delete(url, headers={}, &block)
    opts = {:method => :delete, :url => url, :headers => headers}
    opts.merge!({:response_body_handler => block}) if block_given?
    Request.execute(opts)
  end
end