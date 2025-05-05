package main

import (
	"fmt"
	"os"
	"os/user"

	"github.com/MikhailSolovev/interpretator/repl"
)

func main() {
	user, err := user.Current()
	if err != nil {
		panic(err)
	}

	fmt.Printf("Hello %s! This is the simple interpretator of the language!\n", user.Username)
	fmt.Printf("Feel free to type in commands\n")

	repl.Start(os.Stdin, os.Stdout)
}
