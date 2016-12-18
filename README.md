# Kaleidoscope-plus

## Grammer

```
expression ::= primary binoprhs

primary
  ::= identifierexpr
  ::= numberexpr
  ::= parenexpr

binoprhs
  ::= (binop primary)*

identifierexpr
  ::= identifier
  ::= identifier `(` expression* `)`

numberexpr ::= number

parenexpr ::= `(` expression `)`
```

```
definition ::= `def` prototype expression

external ::= 'extern' prototype

prototype ::= identifier `(` identifier* `)`

toplevelexpr ::= expression
```