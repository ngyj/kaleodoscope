# Reference : Syntax

## Lexer
### Literals
Char & String Literal
```
CHAR_LIT   := `'` (!`\` | ESCAPES) `'`
STRING_LIT := `"` (![ " \ ] | ESCAPES)*`"`
ESCAPES    := [ \\ \" \n \r \0 \t ]
```

Numbers
```
INT_LIT := [ 0-9 ]+
DEC_LIT := [ 0-9 ]+ `.` [ 0-9 ]+
HEX_LIT := `0x` [ 0-9 a-f A-F ]+
OCT_LIT := `0o` [ 0-7 ]+
BIN_LIT := `0b` [ 0 1 ]+
```
### Comments
```
LINECOMMENT := // (!`\n`)*
```

### Reserved keywords
* extern
* let
* in
* do
* type
* deriving
* via
* for
* infixr
* infixl
* infix
* mod
* use
* open

### Operators
```
special  := [ [ ] { } ( ) , ; ' " ` \ ]
opsym    := [ ! # $ % * + . / < = > ? @ ^ | - ~ : & ]

OPERATOR     := opsym (opsym | )*
RESERVED_OP := [ -> : & ! = @ ?? ]
```

### Identifiers
```
syms := [ a-z A-Z 0-9 _ ]
IDENTIFIER := TYID | ID
TYID  := [A-Z] syms*
TYVID := [a-z] [a-z 0-9 _]*
ID    := [a-z] syms*

KW_INFIX := infixr | infixl | infix
```

## Parsing
### Source files
TODO: "virtual brackets" + "explicit brackets"
```
(TODO)
File := ModuleDeclaration? [1]
        Imports? [2]
        Item*

ModuleDeclaration := (TODO)

Item := TermItem
      | TypeItem

TermItem := TypeSig
          | TermDecl
          | ConstDecl
          | OpDecl

TypeItem := TypeDecl
          | TypeSyn
          | KindSig


OpDec := KW_INFIX OPERATOR [0-9]


Function := FnPrototype `=` Term
FnPrototype := FnName FnParams?
FnName := `(` OPERATOR `)` | ID
FnParams := Patterns*


TypeSignature := ID `:` Type

TypeDecl := `data` TyPrototype `=` Constructors
TyPrototype := TyName TYVID*
TyName := TYID
Constructors := DataCon (`|` DataCon)*
DataCon := TYID Type*

TypeSyn := `type` TyPrototype `=` Type

Type := `(` `)`
      | `(` Type `)`
      | TyTuple
      | TyFunction
      | TYID (Type)*
      | TyVar        /* typeVar */
      | `_`          /* inferred type */
TyTuple := `(` Type (`,` Type)+ `)`
TyFunction := Type `->` Type
TyVar := TYVID (`:` Kind)?

Kind := `*`
      | `*` `->` Kind
KindSig := Type `:` Kind

Term := Exp `:` Type

Expr := Literal
      | Var
      | `\` (Pattern `:` Type) `->` Term
      | Infix
      | Expr Exp
      | LetExp
      | CaseExp

LetExp := `let` TermItem* `in` Term

CaseExp := `case` Term `of` CaseBr+

CaseBr := Pattern `->` Term
```
