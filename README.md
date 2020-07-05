# 9cc

[低レイヤを知りたい人のためのCコンパイラ作成入門](https://www.sigbus.info/compilerbook)を基にCコンパイラを作っています。現在以下のコードがコンパイルできます

```
foo = 3;
bar = 5 * 6 - 8;
ret = foo + bar / 2;
return ret; 
```

```
a = 3;
if(a==3)
	return a;
```

```
for(i=0; i<5; i=i+1)
	a=1;

return i;
```

```
a=1;
b=2;
if(b==a+1){
	a=2;
	b=3;
}
return a+b;
```
