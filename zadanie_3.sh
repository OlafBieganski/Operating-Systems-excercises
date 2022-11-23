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
