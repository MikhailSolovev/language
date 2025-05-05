package lexer

import (
	"github.com/MikhailSolovev/interpretator/token"
)

type Lexer struct {
	input        string
	position     int  // current position in input (points to current char)
	readPosition int  // current reading position in input (after current char)
	ch           byte // current char under examination
	line         int  // current line number (starting from 1)
	column       int  // current column number (starting from 1)
}

func New(input string) *Lexer {
	l := &Lexer{
		input:  input,
		line:   1,
		column: 0, // Will become 1 after first readChar()
	}
	l.readChar()
	return l
}

func (l *Lexer) NextToken() token.Token {
	var tok token.Token

	l.skipWhitespace()

	// Create position for this token
	pos := token.Position{
		Line:   l.line,
		Column: l.column,
	}

	switch l.ch {
	case '=':
		if l.peekChar() == '=' {
			ch := l.ch
			l.readChar()
			tok = token.Token{
				Type:     token.EQ,
				Literal:  string(ch) + string(l.ch),
				Position: pos,
			}
		} else {
			tok = token.Token{
				Type:     token.ASSIGN,
				Literal:  "=",
				Position: pos,
			}
		}
	case ';':
		tok = token.Token{
			Type:     token.SEMICOLON,
			Literal:  ";",
			Position: pos,
		}
	case '(':
		tok = token.Token{
			Type:     token.LPAREN,
			Literal:  "(",
			Position: pos,
		}
	case ')':
		tok = token.Token{
			Type:     token.RPAREN,
			Literal:  ")",
			Position: pos,
		}
	case ',':
		tok = token.Token{
			Type:     token.COMMA,
			Literal:  ",",
			Position: pos,
		}
	case '+':
		tok = token.Token{
			Type:     token.PLUS,
			Literal:  "+",
			Position: pos,
		}
	case '-':
		tok = token.Token{
			Type:     token.MINUS,
			Literal:  "-",
			Position: pos,
		}
	case '!':
		if l.peekChar() == '=' {
			ch := l.ch
			l.readChar()
			tok = token.Token{
				Type:     token.NOT_EQ,
				Literal:  string(ch) + string(l.ch),
				Position: pos,
			}
		} else {
			tok = token.Token{
				Type:     token.BANG,
				Literal:  "!",
				Position: pos,
			}
		}
	case '/':
		tok = token.Token{
			Type:     token.SLASH,
			Literal:  "/",
			Position: pos,
		}
	case '*':
		tok = token.Token{
			Type:     token.ASTERISK,
			Literal:  "*",
			Position: pos,
		}
	case '<':
		tok = token.Token{
			Type:     token.LT,
			Literal:  "<",
			Position: pos,
		}
	case '>':
		tok = token.Token{
			Type:     token.GT,
			Literal:  ">",
			Position: pos,
		}
	case '{':
		tok = token.Token{
			Type:     token.LBRACE,
			Literal:  "{",
			Position: pos,
		}
	case '}':
		tok = token.Token{
			Type:     token.RBRACE,
			Literal:  "}",
			Position: pos,
		}
	case 0:
		tok = token.Token{
			Type:     token.EOF,
			Literal:  "",
			Position: pos,
		}
	default:
		if isLetter(l.ch) {
			tok.Literal = l.readIdentifier()
			tok.Type = token.LookupIdent(tok.Literal)
			tok.Position = pos
			return tok
		} else if isDigit(l.ch) {
			tok.Literal = l.readNumber()
			tok.Type = token.INT
			tok.Position = pos
			return tok
		} else {
			tok = token.Token{
				Type:     token.ILLEGAL,
				Literal:  string(l.ch),
				Position: pos,
			}
		}
	}

	l.readChar()
	return tok
}

func (l *Lexer) skipWhitespace() {
	for l.ch == ' ' || l.ch == '\t' || l.ch == '\n' || l.ch == '\r' {
		l.readChar()
	}
}

func (l *Lexer) readChar() {
	if l.ch == '\n' {
		l.line++
		l.column = 0
	}

	if l.readPosition >= len(l.input) {
		l.ch = 0
	} else {
		l.ch = l.input[l.readPosition]
	}

	l.position = l.readPosition
	l.readPosition++
	l.column++
}

func (l *Lexer) readIdentifier() string {
	position := l.position
	for isLetter(l.ch) {
		l.readChar()
	}
	return l.input[position:l.position]
}

func (l *Lexer) readNumber() string {
	position := l.position
	for isDigit(l.ch) {
		l.readChar()
	}
	return l.input[position:l.position]
}

func (l *Lexer) peekChar() byte {
	if l.readPosition >= len(l.input) {
		return 0
	} else {
		return l.input[l.readPosition]
	}
}

func isLetter(ch byte) bool {
	return 'a' <= ch && ch <= 'z' || 'A' <= ch && ch <= 'Z' || ch == '_'
}

func isDigit(ch byte) bool {
	return '0' <= ch && ch <= '9'
}
