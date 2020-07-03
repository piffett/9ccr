#!/bin/bash
assert(){
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s
  cc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert 42 '42;'

assert 41 ' 12 + 34 - 5 ;'

assert 47 '5+6*7;'
assert 15 '5*(9-6);'
assert 4 '(4+5)/2;'

assert 10 '-10+20;'
assert 1 '+2 * -2 + 5;'

assert 0 '0==1;'
assert 1 '42==42;'
assert 1 '0!=1;'
assert 0 '42!=42;'
assert 1 '0<1;'
assert 0 '1<1;'
assert 0 '2<1;'
assert 0 '1<=0;'
assert 1 '1<=1;'
assert 1 '1<=2;'
assert 0 '0>1;'
assert 0 '1>1;'
assert 1 '2>1;'
assert 0 '0>=1;'
assert 1 '1>=1;'
assert 1 '2>=1;'

assert 4 'a=4;'
assert 3 'b=3;a=3;b;'
assert 14 'a = 3;b = 5 * 6 - 8;a + b / 2;'

assert 10 'foo=10;bar=20;bar-foo;'

assert 42 'return 42;return 5;'
assert 6 'a=3;b=a*2;return b;'

assert 4 'a=3;if (a==3) return a+1;'
assert 4 'a=3;if (a==4) return a;return 4;'
assert 4 'a=3;if (a==3) if (a==3)a=4;return a;'
echo OK

