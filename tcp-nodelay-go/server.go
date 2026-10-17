package main

import (
	"bufio"
	"fmt"
	"io"
	"log"
	"net"
	"strings"
)

func main() {
	// Keep this performance experiment local; the client targets localhost.
	listener, err := net.Listen("tcp", "127.0.0.1:8000")
	if err != nil {
		log.Fatal(err)
	}
	defer listener.Close()

	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Println(err)
			return
		}

		go handleConnection(conn)
	}
}

func handleConnection(conn net.Conn) {
	defer conn.Close()
	fmt.Printf("Serving %s\n", conn.RemoteAddr())

	reader := bufio.NewReader(conn)
	for {
		message, err := reader.ReadString('\n')
		if err != nil {
			if err != io.EOF {
				log.Printf("Read from %s: %v", conn.RemoteAddr(), err)
			}
			return
		}

		switch strings.TrimSpace(message) {
		case "GOPHER":
			if _, err := conn.Write([]byte("GopherAcademy Advent 2019!")); err != nil {
				log.Printf("Write to %s: %v", conn.RemoteAddr(), err)
				return
			}
		case "EXIT":
			return
		}
	}
}
