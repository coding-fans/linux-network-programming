/*
 * Author: fasion
 * Created time: 2019-05-10 08:51:45
 * Last Modified by: fasion
 * Last Modified time: 2019-05-10 10:24:27
 */

package main

import (
	"context"
	"fmt"
	"io"
	"log"

	empty "github.com/golang/protobuf/ptypes/empty"

	"google.golang.org/grpc"
)

const (
	address 	= "locals.fasionchan.com:55555"
)

func main() {
	conn, err := grpc.Dial(address, grpc.WithInsecure())
	if err != nil {
		log.Fatalf("failed to connect: %v\n", err)
	}
	defer conn.Close()

	c := NewTickServiceClient(conn)

	handler, err := c.Listen(context.Background(), &empty.Empty{})
	if err != nil {
		log.Fatalf("failed to call listen :%s\n", err)
	}

	for {
		tick, err := handler.Recv()
		if err != nil {
			if err != io.EOF {
				log.Fatalf("failed to Recv: %v\n", err)
			}
			break
		}

		fmt.Println(tick.GetTimestamp())
	}
}