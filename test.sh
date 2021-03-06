#!/bin/bash
assert(){
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s
  cc -o tmp tmp.s a.o
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}


assert 1 'main(){return 1;}'

assert 10 'foof(){return 5;}main(){a=5;return a+foof();}'

assert 15 'foof(a, b){return a*2+b;}main(){a=1;return a+foof(2,10);} '

assert 3 'main(){x=3;y=5;z= &y+8; return *z;}'

# 以下は以前のやつ。main対応もいつかやる


# assert 42 '42;'

assert 41 'main(){return 12 + 34 - 5 ;}'

assert 47 'main(){return 5+6*7;return 1;}'
#assert 15 '5*(9-6);'
## assert 4 '(4+5)/2;'

#assert 10 '-10+20;'
## assert 1 '+2 * -2 + 5;'

#assert 0 '0==1;'
#assert 1 '42==42;'
#assert 1 '0!=1;'
#assert 0 '42!=42;'
#assert 1 '0<1;'
#assert 0 '1<1;'
#assert 0 '2<1;'
#assert 0 '1<=0;'
#assert 1 '1<=1;'
#assert 1 '1<=2;'
#assert 0 '0>1;'
#assert 0 '1>1;'
#assert 1 '2>1;'
#assert 0 '0>=1;'
#assert 1 '1>=1;'
## assert 1 '2>=1;'

#assert 4 'a=4;'
## assert 14 'a = 3;b = 5 * 6 - 8;a + b / 2;'

#assert 10 'foo=10;bar=20;bar-foo;'

## assert 42 'return 42;return 5;'
## assert 6 'a=3;b=a*2;return b;'

## assert 4 'a=3;if (a==4) return a;return 4;'
## assert 4 'a=3;if (a==3) if (a==3)a=4;return a;'

#assert 5 'if(4==1+2) return 3; else return 5;'
#assert 6 'a=1;while(a!=6)a=a+1;return a;'
## assert 5 'for(i=0;i<5;i=i+1) a=1; return i;'

## assert 5 'a=1;b=2;if(b==a+1){a=2;b=3;}return a+b;'

## assert 1 'foo();return 1;'
# assert 64 'foo2(1 , 2);'
## assert 1 'foo3(1, 2, 3);return 1'
## assert 5 'a=5;a= bar(1, 5);return a;'
assert 5 'main(){a=1;a=bar(1, 5);return a;}'

echo OK


