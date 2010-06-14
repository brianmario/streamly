# encoding: UTF-8
require File.expand_path(File.dirname(__FILE__) + '/../spec_helper.rb')

describe "Streamly's REST API" do

  before(:all) do
    @response = "Hello, brian".strip
  end

  context "HEAD" do
    context "basic" do
      it "should perform a basic request" do
        resp = Streamly.head('localhost:4567')
        resp.should_not be_empty
      end

      if RUBY_VERSION =~ /^1.9/
        it "should default to utf-8 if Encoding.default_internal is nil" do
          Encoding.default_internal = nil
          Streamly.head('localhost:4567').encoding.should eql(Encoding.find('utf-8'))
        end

        it "should use Encoding.default_internal" do
          Encoding.default_internal = Encoding.find('utf-8')
          Streamly.head('localhost:4567').encoding.should eql(Encoding.default_internal)
          Encoding.default_internal = Encoding.find('us-ascii')
          Streamly.head('localhost:4567').encoding.should eql(Encoding.default_internal)
        end
      end
    end

    context "streaming" do
      it "should perform a basic request and stream header chunks to the caller" do
        streamed_response = ''
        resp = Streamly.head('localhost:4567') do |chunk|
          chunk.should_not be_empty
          streamed_response << chunk
        end
        resp.should be_nil
        streamed_response.should_not be_nil
      end

      if RUBY_VERSION =~ /^1.9/
        it "should default to utf-8 if Encoding.default_internal is nil" do
          Encoding.default_internal = nil
          Streamly.head('localhost:4567') do |chunk|
            chunk.encoding.should eql(Encoding.find('utf-8'))
          end
        end

        it "should use Encoding.default_internal" do
          Encoding.default_internal = Encoding.find('utf-8')
          Streamly.head('localhost:4567') do |chunk|
            chunk.encoding.should eql(Encoding.default_internal)
          end
          Encoding.default_internal = Encoding.find('us-ascii')
          Streamly.head('localhost:4567') do |chunk|
            chunk.encoding.should eql(Encoding.default_internal)
          end
        end
      end
    end
  end

  context "GET" do
    context "basic" do
      it "should perform a basic request" do
        resp = Streamly.get('localhost:4567/?name=brian')
        resp.should eql(@response)
      end

      if RUBY_VERSION =~ /^1.9/
        it "should default to utf-8 if Encoding.default_internal is nil" do
          Encoding.default_internal = nil
          Streamly.get('localhost:4567').encoding.should eql(Encoding.find('utf-8'))
        end

        it "should use Encoding.default_internal" do
          Encoding.default_internal = Encoding.find('utf-8')
          Streamly.get('localhost:4567').encoding.should eql(Encoding.default_internal)
          Encoding.default_internal = Encoding.find('us-ascii')
          Streamly.get('localhost:4567').encoding.should eql(Encoding.default_internal)
        end
      end
    end

    context "streaming" do
      it "should perform a basic request and stream the response to the caller" do
        streamed_response = ''
        resp = Streamly.get('localhost:4567/?name=brian') do |chunk|
          chunk.should_not be_empty
          streamed_response << chunk
        end
        resp.should be_nil
        streamed_response.should eql(@response)
      end

      if RUBY_VERSION =~ /^1.9/
        it "should default to utf-8 if Encoding.default_internal is nil" do
          Encoding.default_internal = nil
          Streamly.get('localhost:4567') do |chunk|
            chunk.encoding.should eql(Encoding.find('utf-8'))
          end
        end

        it "should use Encoding.default_internal" do
          Encoding.default_internal = Encoding.find('utf-8')
          Streamly.get('localhost:4567') do |chunk|
            chunk.encoding.should eql(Encoding.default_internal)
          end
          Encoding.default_internal = Encoding.find('us-ascii')
          Streamly.get('localhost:4567') do |chunk|
            chunk.encoding.should eql(Encoding.default_internal)
          end
        end
      end
    end
  end

  context "POST" do
    context "basic" do
      it "should perform a basic request" do
        resp = Streamly.post('localhost:4567', 'name=brian')
        resp.should eql(@response)
      end

      if RUBY_VERSION =~ /^1.9/
        it "should default to utf-8 if Encoding.default_internal is nil" do
          Encoding.default_internal = nil
          Streamly.post('localhost:4567', 'name=brian').encoding.should eql(Encoding.find('utf-8'))
        end

        it "should use Encoding.default_internal" do
          Encoding.default_internal = Encoding.find('utf-8')
          Streamly.post('localhost:4567', 'name=brian').encoding.should eql(Encoding.default_internal)
          Encoding.default_internal = Encoding.find('us-ascii')
          Streamly.post('localhost:4567', 'name=brian').encoding.should eql(Encoding.default_internal)
        end
      end
    end

    context "streaming" do
      it "should perform a basic request and stream the response to the caller" do
        streamed_response = ''
        resp = Streamly.post('localhost:4567', 'name=brian') do |chunk|
          chunk.should_not be_empty
          streamed_response << chunk
        end
        resp.should be_nil
        streamed_response.should eql(@response)
      end

      if RUBY_VERSION =~ /^1.9/
        it "should default to utf-8 if Encoding.default_internal is nil" do
          Encoding.default_internal = nil
          Streamly.post('localhost:4567', 'name=brian') do |chunk|
            chunk.encoding.should eql(Encoding.find('utf-8'))
          end
        end

        it "should use Encoding.default_internal" do
          Encoding.default_internal = Encoding.find('utf-8')
          Streamly.post('localhost:4567', 'name=brian') do |chunk|
            chunk.encoding.should eql(Encoding.default_internal)
          end
          Encoding.default_internal = Encoding.find('us-ascii')
          Streamly.post('localhost:4567', 'name=brian') do |chunk|
            chunk.encoding.should eql(Encoding.default_internal)
          end
        end
      end
    end
  end

  context "PUT" do
    context "basic" do
      it "should perform a basic request" do
        resp = Streamly.put('localhost:4567', 'name=brian')
        resp.should eql(@response)
      end

      if RUBY_VERSION =~ /^1.9/
        it "should default to utf-8 if Encoding.default_internal is nil" do
          Encoding.default_internal = nil
          Streamly.put('localhost:4567', 'name=brian').encoding.should eql(Encoding.find('utf-8'))
        end

        it "should use Encoding.default_internal" do
          Encoding.default_internal = Encoding.find('utf-8')
          Streamly.put('localhost:4567', 'name=brian').encoding.should eql(Encoding.default_internal)
          Encoding.default_internal = Encoding.find('us-ascii')
          Streamly.put('localhost:4567', 'name=brian').encoding.should eql(Encoding.default_internal)
        end
      end
    end

    context "streaming" do
      it "should perform a basic request and stream the response to the caller" do
        streamed_response = ''
        resp = Streamly.put('localhost:4567', 'name=brian') do |chunk|
          chunk.should_not be_empty
          streamed_response << chunk
        end
        resp.should be_nil
        streamed_response.should eql(@response)
      end

      if RUBY_VERSION =~ /^1.9/
        it "should default to utf-8 if Encoding.default_internal is nil" do
          Encoding.default_internal = nil
          Streamly.put('localhost:4567', 'name=brian') do |chunk|
            chunk.encoding.should eql(Encoding.find('utf-8'))
          end
        end

        it "should use Encoding.default_internal" do
          Encoding.default_internal = Encoding.find('utf-8')
          Streamly.put('localhost:4567', 'name=brian') do |chunk|
            chunk.encoding.should eql(Encoding.default_internal)
          end
          Encoding.default_internal = Encoding.find('us-ascii')
          Streamly.put('localhost:4567', 'name=brian') do |chunk|
            chunk.encoding.should eql(Encoding.default_internal)
          end
        end
      end
    end
  end

  context "DELETE" do
    context "basic" do
      it "should perform a basic request" do
        resp = Streamly.delete('localhost:4567/?name=brian').should eql(@response)
      end

      if RUBY_VERSION =~ /^1.9/
        it "should default to utf-8 if Encoding.default_internal is nil" do
          Encoding.default_internal = nil
          Streamly.delete('localhost:4567/?name=brian').encoding.should eql(Encoding.find('utf-8'))
        end

        it "should use Encoding.default_internal" do
          Encoding.default_internal = Encoding.find('utf-8')
          Streamly.delete('localhost:4567/?name=brian').encoding.should eql(Encoding.default_internal)
          Encoding.default_internal = Encoding.find('us-ascii')
          Streamly.delete('localhost:4567/?name=brian').encoding.should eql(Encoding.default_internal)
        end
      end
    end

    context "streaming" do
      it "should perform a basic request and stream the response to the caller" do
        streamed_response = ''
        resp = Streamly.delete('localhost:4567/?name=brian') do |chunk|
          chunk.should_not be_empty
          streamed_response << chunk
        end
        resp.should be_nil
        streamed_response.should eql(@response)
      end

      if RUBY_VERSION =~ /^1.9/
        it "should default to utf-8 if Encoding.default_internal is nil" do
          Encoding.default_internal = nil
          Streamly.delete('localhost:4567/?name=brian') do |chunk|
            chunk.encoding.should eql(Encoding.find('utf-8'))
          end
        end

        it "should use Encoding.default_internal" do
          Encoding.default_internal = Encoding.find('utf-8')
          Streamly.delete('localhost:4567/?name=brian') do |chunk|
            chunk.encoding.should eql(Encoding.default_internal)
          end
          Encoding.default_internal = Encoding.find('us-ascii')
          Streamly.delete('localhost:4567/?name=brian') do |chunk|
            chunk.encoding.should eql(Encoding.default_internal)
          end
        end
      end
    end
  end
end