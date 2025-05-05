package ast

type Node interface {
	TokenLiteral() string
}

type Statement interface {
	Node
	// dummy method
	statementNode()
}

type Expression interface {
	Node
	// dummy method
	expressionNode()
}
