variable pc 1 pc !
$240 constant polynomial
: lfsr 
  dup 1 and ( mask off feed back )
  swap 1 rshift swap ( state /= 2 )
  if polynomial xor then ; ( xor in poly if feedback set )
: pc++ pc @ dup lfsr pc ! ;
: test for pc++ . cr next ;
