##Sprawozdanie

#Zadanie 1

```bash
var=$(date +%u)
if [ $var -gt 5 ]
then
	echo "Miłego weekendu !" 
else
	echo "Witamy w pracy!"
fi
```

#Zadanie 2

```bash
if [ $1 == "-a" ]
then
	files=$(ls -a -1 | wc -l)
else
	files=$(ls -1 | wc -l)
fi
echo Liczba plików w katalogu: $files
```

#Zadanie 3

```bash
for x in "$@"
do
if [ -e "$x" ] && [ ! -d "$x" ]
then
    echo "$x - plik istnieje"
elif [ -d "$x" ]
then
    echo "$x - katalog"
else
    echo "$x - plik nie istnieje"
fi
done
```

#Zadanie 4

```bash
#!/bin/bash
find -name "*.$1" -mtime -$2 -perm -444 -exec tar -rvf $3.tar {} \;
```

#Zadanie 5

```bash
#!/bin/bash
# skrypt wysyłający maile o danej tresci do adresatów zapisanych
# w pliku tekstowym podanym jako argument skryptu
if [ -f $1 ]
then
    while read -r line
    do
        mail -s 'Test skryptu zadanie_5.sh' $line <<< 'Mail wysłany w celach testowych'
    done < $1   
else
    echo "Nie mozna odczytac takiego pliku."
fi

```