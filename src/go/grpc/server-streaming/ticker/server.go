/*
 * Author: fasion
 * Created time: 2019-05-10 09:13:27
 * Last Modified by: fasion
 * Last Modified time: 2019-05-10 10:26:31
 */

package main

import (
	"log"
	"net"
	"time"

	"google.golang.org/grpc"

	"github.com/golang/protobuf/ptypes"
	empty "github.com/golang/protobuf/ptypes/empty"
)

const (
	address 	= ":55555"
)

type TickService struct{}

func (self *TickService) Listen(_ *empty.Empty, handler TickService_ListenServer) (error) {
	for {
		tick := Tick{
			Timestamp: ptypes.TimestampNow(),
		}

		if err := handler.Send(&tick); err != nil {
			log.Printf("fail to send: %v\n", err)
		}

		time.Sleep(time.Second)
	}

	return nil
}

func main() {
	lstn, err := net.Listen("tcp", address)
	if err != nil {
		log.Fatalf("failed to listen: %v", err)
	}

	service := TickService{}

	s := grpc.NewServer()
	RegisterTickServiceServer(s, &service)

	if err := s.Serve(lstn); err != nil {
		log.Fatalf("failed to serve: %s", err)
	}
}