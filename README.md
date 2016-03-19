# Быстрая адаптация кода на C образца 95 года для реального режима под DOS к 2016 году.

```
gcc -O2 -g -lm -funsigned-char fe.c -o fe
iconv -f utf-8 -t cp866 file.txt -o file.866.txt
./fe -a file.866.txt
iconv -f cp866 -t utf-8 fresheye.log -o fresheye2.log
cat fresheye2.log
```
