package main

import (
	"bufio"
	"fmt"
	"net"
	"os"
	"strings"
	"time"
)

func main() {
	if len(os.Args) < 2 {
		fmt.Fprintln(os.Stderr, "Usage: go run client.go 127.0.0.1:5500")
		os.Exit(2)
	}

	serverAddr, err := net.ResolveUDPAddr("udp4", os.Args[1])
	if err != nil {
		fmt.Fprintf(os.Stderr, "Invalid UDP address: %v\n", err)
		os.Exit(1)
	}

	conn, err := net.DialUDP("udp4", nil, serverAddr)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Could not start UDP client: %v\n", err)
		os.Exit(1)
	}
	defer conn.Close()

	fmt.Printf("UDP target: %s\n", conn.RemoteAddr())
	reader := bufio.NewReader(os.Stdin)
	buffer := make([]byte, 1024)

	for {
		fmt.Print(">> ")
		line, readErr := reader.ReadString('\n')
		if readErr != nil && len(line) == 0 {
			return
		}

		message := strings.TrimSpace(line)
		if _, err := conn.Write([]byte(message)); err != nil {
			fmt.Fprintf(os.Stderr, "Could not send datagram: %v\n", err)
			return
		}

		if message == "exit" {
			fmt.Println("UDP client exiting.")
			return
		}

		if err := conn.SetReadDeadline(time.Now().Add(5 * time.Second)); err != nil {
			fmt.Fprintf(os.Stderr, "Could not set receive timeout: %v\n", err)
			return
		}

		n, err := conn.Read(buffer)
		if err != nil {
			fmt.Fprintf(os.Stderr, "No UDP response received: %v\n", err)
			return
		}

		fmt.Printf("Reply: %s\n", string(buffer[:n]))
		if readErr != nil {
			return
		}
	}
}
