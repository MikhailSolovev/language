package repl

import (
	"bufio"
	"fmt"
	"io"

	"github.com/MikhailSolovev/interpretator/lexer"
	"github.com/MikhailSolovev/interpretator/token"
)

const PROMPT = ">> "

func Start(in io.Reader, out io.Writer) {
	scanner := bufio.NewScanner(in)

	for {
		fmt.Printf(PROMPT)
		scanned := scanner.Scan()
		if !scanned {
			return
		}
		line := scanner.Text()

		l := lexer.New(line)
		for tok := l.NextToken(); tok.Type != token.EOF; tok = l.NextToken() {
			fmt.Printf("Type: %s, Literal: %s, Position: [line:%d, column:%d]\n", tok.Type, tok.Literal, tok.Position.Line, tok.Position.Column)
		}
	}
}
