#!/usr/bin/env ruby

executable = ARGV[0]
unless executable
  STDERR.puts "Usage: #{__FILE__} [bin_file in bin/ dir]"
  STDERR.puts "Example: #{__FILE__} simulation            # runs bin/simulation and stores data in plotdata/simulation.plot"
  exit(1)
end

data = `bin/simulation 2>&1 | cut -d " " -f 3,4,5`

server_data = {:received => [], :sent => []}
client_data = {:received => [], :sent => []}

data.split("\n").each do |line|
  time, who, what = line.split(" ")
  what = what.to_sym
  time = time[0..-2].to_f

  case who
    when "server"
      server_data[what] << time
    when "client"
      client_data[what] << time
  end
end

puts "Client: " + server_data.inspect
puts "Server: " + client_data.inspect

File.open("plotdata/#{executable}.plot", "w+") do |f|
  f.puts "# PacketNumber Client(Sent) Client(Received) Server(Sent) Server(Received)"

  server_data[:sent].size.times do |i|
    f.puts "#{i + 1} #{client_data[:sent][i]} #{client_data[:received][i]} #{server_data[:sent][i]} #{server_data[:received][i]}"
  end
end
