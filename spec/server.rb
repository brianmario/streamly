require 'rubygems'
require 'sinatra'

get '/' do
  "Hello, #{params[:name]}".strip
end

post '/' do
  "Hello, #{params[:name]}".strip
end

put '/' do
  "Hello, #{params[:name]}".strip
end

delete '/' do
  "Hello, #{params[:name]}".strip
end