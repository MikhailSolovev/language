package lexer

import (
	"testing"

	"github.com/MikhailSolovev/interpretator/token"
)

func TestNextToken(t *testing.T) {
	// Test basic token functionality
	input := `
		let five = 5;
		let ten = 10;

		let add = fn(x, y) {
  			x + y;
		};

		let result = add(five, ten);
		!-/*5;
		5 < 10 > 5;

		if (5 < 10) {
			return true;
		} else {
			return false;
		}

		10 == 10;
		10 != 9;
	`

	tests := []token.Token{
		{token.LET, "let", token.Position{2, 3}},
		{token.IDENT, "five", token.Position{2, 7}},
		{token.ASSIGN, "=", token.Position{2, 12}},
		{token.INT, "5", token.Position{2, 14}},
		{token.SEMICOLON, ";", token.Position{2, 15}},
		{token.LET, "let", token.Position{3, 3}},
		{token.IDENT, "ten", token.Position{3, 7}},
		{token.ASSIGN, "=", token.Position{3, 11}},
		{token.INT, "10", token.Position{3, 13}},
		{token.SEMICOLON, ";", token.Position{3, 15}},
		{token.LET, "let", token.Position{5, 3}},
		{token.IDENT, "add", token.Position{5, 7}},
		{token.ASSIGN, "=", token.Position{5, 11}},
		{token.FUNCTION, "fn", token.Position{5, 13}},
		{token.LPAREN, "(", token.Position{5, 15}},
		{token.IDENT, "x", token.Position{5, 16}},
		{token.COMMA, ",", token.Position{5, 17}},
		{token.IDENT, "y", token.Position{5, 19}},
		{token.RPAREN, ")", token.Position{5, 20}},
		{token.LBRACE, "{", token.Position{5, 22}},
		{token.IDENT, "x", token.Position{6, 6}},
		{token.PLUS, "+", token.Position{6, 8}},
		{token.IDENT, "y", token.Position{6, 10}},
		{token.SEMICOLON, ";", token.Position{6, 11}},
		{token.RBRACE, "}", token.Position{7, 3}},
		{token.SEMICOLON, ";", token.Position{7, 4}},
		{token.LET, "let", token.Position{9, 3}},
		{token.IDENT, "result", token.Position{9, 7}},
		{token.ASSIGN, "=", token.Position{9, 14}},
		{token.IDENT, "add", token.Position{9, 16}},
		{token.LPAREN, "(", token.Position{9, 19}},
		{token.IDENT, "five", token.Position{9, 20}},
		{token.COMMA, ",", token.Position{9, 24}},
		{token.IDENT, "ten", token.Position{9, 26}},
		{token.RPAREN, ")", token.Position{9, 29}},
		{token.SEMICOLON, ";", token.Position{9, 30}},
		{token.BANG, "!", token.Position{10, 3}},
		{token.MINUS, "-", token.Position{10, 4}},
		{token.SLASH, "/", token.Position{10, 5}},
		{token.ASTERISK, "*", token.Position{10, 6}},
		{token.INT, "5", token.Position{10, 7}},
		{token.SEMICOLON, ";", token.Position{10, 8}},
		{token.INT, "5", token.Position{11, 3}},
		{token.LT, "<", token.Position{11, 5}},
		{token.INT, "10", token.Position{11, 7}},
		{token.GT, ">", token.Position{11, 10}},
		{token.INT, "5", token.Position{11, 12}},
		{token.SEMICOLON, ";", token.Position{11, 13}},
		{token.IF, "if", token.Position{13, 3}},
		{token.LPAREN, "(", token.Position{13, 6}},
		{token.INT, "5", token.Position{13, 7}},
		{token.LT, "<", token.Position{13, 9}},
		{token.INT, "10", token.Position{13, 11}},
		{token.RPAREN, ")", token.Position{13, 13}},
		{token.LBRACE, "{", token.Position{13, 15}},
		{token.RETURN, "return", token.Position{14, 4}},
		{token.TRUE, "true", token.Position{14, 11}},
		{token.SEMICOLON, ";", token.Position{14, 15}},
		{token.RBRACE, "}", token.Position{15, 3}},
		{token.ELSE, "else", token.Position{15, 5}},
		{token.LBRACE, "{", token.Position{15, 10}},
		{token.RETURN, "return", token.Position{16, 4}},
		{token.FALSE, "false", token.Position{16, 11}},
		{token.SEMICOLON, ";", token.Position{16, 16}},
		{token.RBRACE, "}", token.Position{17, 3}},
		{token.INT, "10", token.Position{19, 3}},
		{token.EQ, "==", token.Position{19, 6}},
		{token.INT, "10", token.Position{19, 9}},
		{token.SEMICOLON, ";", token.Position{19, 11}},
		{token.INT, "10", token.Position{20, 3}},
		{token.NOT_EQ, "!=", token.Position{20, 6}},
		{token.INT, "9", token.Position{20, 9}},
		{token.SEMICOLON, ";", token.Position{20, 10}},
		{token.EOF, "", token.Position{21, 2}},
	}

	l := New(input)

	for i, expected := range tests {
		tok := l.NextToken()
		if tok.Type != expected.Type {
			t.Fatalf("tests[%d] - tokentype wrong. expected=%q, got=%q",
				i, expected.Type, tok.Type)
		}
		if tok.Literal != expected.Literal {
			t.Fatalf("tests[%d] - literal wrong. expected=%q, got=%q",
				i, expected.Literal, tok.Literal)
		}
		if tok.Position.Line != expected.Position.Line {
			t.Fatalf("tests[%d] - line wrong. expected=%d, got=%d",
				i, expected.Position.Line, tok.Position.Line)
		}
		if tok.Position.Column != expected.Position.Column {
			t.Fatalf("tests[%d] - column wrong. expected=%d, got=%d",
				i, expected.Position.Column, tok.Position.Column)
		}
	}
}
