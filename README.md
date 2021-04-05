# Fresh Eye

Быстрая адаптация кода на C образца 95 года для реального режима под DOS к 2016 году.

## Build

```
make
```
or
```
gcc -O2 -g -funsigned-char src/fe.c -o fe -lm
```

## Use

```
iconv -c -f utf-8 -t cp866 file.txt -o file.866.txt
./fe -a file.866.txt file.866.txt.fe.log
iconv -c -f cp866 -t utf-8 file.866.txt.fe.log -o file.txt.fe.log
cat file.txt.fe.log
```

