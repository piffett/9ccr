# 9cc

[低レイヤを知りたい人のためのCコンパイラ作成入門](https://www.sigbus.info/compilerbook)を基にCコンパイラを作っています。現在以下のコードがコンパイルできます

```
main(){
	foo = 3;
	ar = 5 * 6 - 8;
	et = foo + bar / 2;
	return ret; 
}
```

```
main(){
	a = 3;
	if(a==3)
		return a;
}
```

```
main(){
	for(i=0; i<5; i=i+1)
		a=1;

	return i;
}
```

```
foo(a, b){
	return 2*a+b;
}
main(){
	return foo(1,2);
}
```
