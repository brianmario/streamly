# -*- encoding: utf-8 -*-

Gem::Specification.new do |s|
  s.name = %q{streamly}
  s.version = "0.1.0"

  s.required_rubygems_version = Gem::Requirement.new(">= 0") if s.respond_to? :required_rubygems_version=
  s.authors = ["Brian Lopez"]
  s.date = %q{2009-07-27}
  s.email = %q{seniorlopez@gmail.com}
  s.extensions = ["ext/extconf.rb"]
  s.extra_rdoc_files = [
    "README.rdoc"
  ]
  s.files = [
    ".gitignore",
     "CHANGELOG.md",
     "MIT-LICENSE",
     "README.rdoc",
     "Rakefile",
     "VERSION.yml",
     "benchmark/basic_request.rb",
     "benchmark/streaming_json_request.rb",
     "examples/basic/delete.rb",
     "examples/basic/get.rb",
     "examples/basic/head.rb",
     "examples/basic/post.rb",
     "examples/basic/put.rb",
     "examples/streaming/delete.rb",
     "examples/streaming/get.rb",
     "examples/streaming/head.rb",
     "examples/streaming/post.rb",
     "examples/streaming/put.rb",
     "ext/extconf.rb",
     "ext/streamly.c",
     "ext/streamly.h",
     "lib/streamly.rb",
     "spec/rcov.opts",
     "spec/requests/request_spec.rb",
     "spec/sinatra.rb",
     "spec/spec.opts",
     "spec/spec_helper.rb"
  ]
  s.homepage = %q{http://github.com/brianmario/streamly}
  s.rdoc_options = ["--charset=UTF-8"]
  s.require_paths = ["lib", "ext"]
  s.rubyforge_project = %q{streamly}
  s.rubygems_version = %q{1.3.5}
  s.summary = %q{A streaming REST client for Ruby, in C.}
  s.test_files = [
    "spec/requests/request_spec.rb",
     "spec/sinatra.rb",
     "spec/spec_helper.rb",
     "examples/basic/delete.rb",
     "examples/basic/get.rb",
     "examples/basic/head.rb",
     "examples/basic/post.rb",
     "examples/basic/put.rb",
     "examples/streaming/delete.rb",
     "examples/streaming/get.rb",
     "examples/streaming/head.rb",
     "examples/streaming/post.rb",
     "examples/streaming/put.rb"
  ]

  if s.respond_to? :specification_version then
    current_version = Gem::Specification::CURRENT_SPECIFICATION_VERSION
    s.specification_version = 3

    if Gem::Version.new(Gem::RubyGemsVersion) >= Gem::Version.new('1.2.0') then
    else
    end
  else
  end
end
