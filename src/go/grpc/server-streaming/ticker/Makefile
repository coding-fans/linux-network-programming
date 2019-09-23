# Author: fasion
# Created time: 2019-05-10 09:35:28
# Last Modified by: fasion
# Last Modified time: 2019-05-10 10:29:42

default: clean all

tick.pb.go: tick.proto
	protoc --go_out=plugins=grpc:. $^

server: server.go tick.pb.go
	go build $^

client: client.go tick.pb.go
	go build $^

all: server client

clean:
	rm -f tick.pb.go server client