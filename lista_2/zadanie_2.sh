if [ $1 == "-a" ]
then
	files=$(ls -a -1 | wc -l)
else
	files=$(ls -1 | wc -l)
fi
echo Liczba plików w katalogu: $files
