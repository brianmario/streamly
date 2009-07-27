# encoding: UTF-8
require File.expand_path(File.dirname(__FILE__) + '/../spec_helper.rb')

describe "Streamly's REST API" do
  
  before(:all) do
    @response = "Hello, brian".strip
  end
  
  context "HEAD" do
    it "should perform a basic request" do
      resp = Streamly.head('localhost:4567')
      resp.should_not be_empty
    end
    
    it "should perform a basic request and stream the response to the caller" do
      streamed_response = ''
      resp = Streamly.head('localhost:4567') do |chunk|
        chunk.should_not be_empty
        streamed_response << chunk
      end
      resp.should be_nil
      streamed_response.should_not be_nil
    end
  end
  
  context "GET" do
    it "should perform a basic request" do
      resp = Streamly.get('localhost:4567/?name=brian')
      resp.should eql(@response)
    end
    
    it "should perform a basic request and stream the response to the caller" do
      streamed_response = ''
      resp = Streamly.get('localhost:4567/?name=brian') do |chunk|
        chunk.should_not be_empty
        streamed_response << chunk
      end
      resp.should be_nil
      streamed_response.should eql(@response)
    end
  end
  
  context "POST" do
    it "should perform a basic request" do
      resp = Streamly.post('localhost:4567', 'name=brian')
      resp.should eql(@response)
    end
    
    it "should perform a basic request and stream the response to the caller" do
      streamed_response = ''
      resp = Streamly.post('localhost:4567', 'name=brian') do |chunk|
        chunk.should_not be_empty
        streamed_response << chunk
      end
      resp.should be_nil
      streamed_response.should eql(@response)
    end
  end
  
  context "PUT" do
    it "should perform a basic request" do
      resp = Streamly.put('localhost:4567', 'name=brian')
      resp.should eql(@response)
    end
    
    it "should perform a basic request and stream the response to the caller" do
      streamed_response = ''
      resp = Streamly.put('localhost:4567', 'name=brian') do |chunk|
        chunk.should_not be_empty
        streamed_response << chunk
      end
      resp.should be_nil
      streamed_response.should eql(@response)
    end
  end
  
  context "DELETE" do
    it "should perform a basic request" do
      resp = Streamly.delete('localhost:4567/?name=brian').should eql(@response)
    end
    
    it "should perform a basic request and stream the response to the caller" do
      streamed_response = ''
      resp = Streamly.delete('localhost:4567/?name=brian') do |chunk|
        chunk.should_not be_empty
        streamed_response << chunk
      end
      resp.should be_nil
      streamed_response.should eql(@response)
    end
  end
end