package main

import (
	"fmt"
	"net"
	"strings"
)

func main() {
	serverAddr := &net.UDPAddr{
		IP:   net.ParseIP("127.0.0.1"),
		Port: 5500,
	}

	conn, err := net.ListenUDP("udp4", serverAddr)
	if err != nil {
		fmt.Printf("Could not start UDP server: %v\n", err)
		return
	}
	defer conn.Close()

	fmt.Println("UDP server listening on 127.0.0.1:5500")
	fmt.Println("Send 'exit' to stop this learning server.")

	buffer := make([]byte, 1024)
	requestCount := 0

	for {
		n, clientAddr, err := conn.ReadFromUDP(buffer)
		if err != nil {
			fmt.Printf("Could not receive datagram: %v\n", err)
			return
		}

		// Work with the actual bytes received. Never trim by slicing n-1 or n-2:
		// UDP datagrams can be empty or may not end with a newline.
		message := strings.TrimSpace(string(buffer[:n]))

		if message == "exit" {
			fmt.Println("Exit command received; stopping UDP server.")
			return
		}

		fmt.Printf("Received %d byte(s) from %s: %q\n", n, clientAddr, message)

		response := fmt.Sprintf("Request number %d to this server", requestCount)
		if _, err := conn.WriteToUDP([]byte(response), clientAddr); err != nil {
			fmt.Printf("Could not send response to %s: %v\n", clientAddr, err)
			continue
		}

		requestCount++
	}
}
