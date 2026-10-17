package main

import (
	"bufio"
	"fmt"
	"io"
	"net"
	"os"
	"strings"
)

func main() {
	if len(os.Args) < 2 {
		fmt.Fprintln(os.Stderr, "Usage: go run client.go 127.0.0.1:5500")
		os.Exit(2)
	}

	conn, err := net.Dial("tcp", os.Args[1])
	if err != nil {
		fmt.Fprintf(os.Stderr, "Could not connect to server: %v\n", err)
		os.Exit(1)
	}
	defer conn.Close()

	stdin := bufio.NewReader(os.Stdin)
	serverReader := bufio.NewReader(conn)

	for {
		fmt.Print(">> ")
		line, readErr := stdin.ReadString('\n')
		if readErr != nil && len(line) == 0 {
			if readErr != io.EOF {
				fmt.Fprintf(os.Stderr, "Could not read input: %v\n", readErr)
			}
			return
		}

		message := strings.TrimRight(line, "\r\n")
		if _, err := fmt.Fprintln(conn, message); err != nil {
			fmt.Fprintf(os.Stderr, "Could not send message: %v\n", err)
			return
		}

		// The sample server closes the connection after receiving "exit",
		// so don't wait for a reply to that command.
		if strings.TrimSpace(message) == "exit" {
			fmt.Println("TCP client exiting.")
			return
		}

		response, err := serverReader.ReadString('\n')
		if err != nil {
			fmt.Fprintf(os.Stderr, "Could not read server response: %v\n", err)
			return
		}
		fmt.Print("->: " + response)

		if readErr == io.EOF {
			return
		}
	}
}
