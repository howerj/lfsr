\ Simple test bench for LFSR Forth
: .( 29 parse type ;
: ahoy cr ." HELLO, WORLD" cr ;
: t1 if 99 . cr then cr ;
: t2 for r@ . next cr ;
words
cr
ahoy
cr .( Nothing:  ) 0 t1
cr .( Print '99':  ) 2 t1
cr .( Sequence [hex]:  ) 10 t2
cr bye
