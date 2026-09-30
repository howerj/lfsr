\ Simple test bench for LFSR Forth
: .( 29 parse type ;
: ahoy cr ." HELLO, WORLD" cr ;
: t1 if 99 . cr then cr ;
: t2 for r@ . next cr ;

: decimal $A base ! ;
: hex $10 base ! ;

decimal
: csi 27 emit 91 emit ; ( -- )
: sgr csi 0 <# #s #> type 109 emit ; ( ansi -- )
: fg 30 + sgr ; ( color[0-7] -- : foreground color )
: bg 40 + sgr ; ( color[0-7] -- : background color )
: reset 0 sgr ; ( -- : reset ANSI terminal colors )

: colors ( -- : display table of ANSI colors )
  cr
  7 for 
    9 emit r@ bg
    7 for
      r@ dup fg .
    next reset cr
  next ;

.( ===== ANSI Color codes ===== ) cr
colors
.( ===== ANSI Color codes ===== ) cr

hex
words
cr
ahoy
cr .( Nothing:  ) 0 t1
cr .( Print '99':  ) 2 t1
cr .( Sequence [hex]:  ) 10 t2
cr bye
