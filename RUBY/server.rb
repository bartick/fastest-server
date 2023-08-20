# create a http server in ruby
# run: ruby server.rb
# open browser and type: http://localhost:8080

require 'socket'

server = TCPServer.new 8080

puts "Server running on port 8080\n"

loop do
    
  trap 'INT' do
    puts "\nServer stopped."
    server.close
    exit
  end

  client = server.accept
  request = client.gets
  puts request

  requestPath = request.split(/ /)[1]
  response = "Hello World!\nRequest: #{requestPath}"
  client.puts "HTTP/1.1 200 OK"
  client.puts "Content-Type: text/plain"
  client.puts "Content-Length: #{response.bytesize}"
  client.puts
  client.puts response
  client.close
end