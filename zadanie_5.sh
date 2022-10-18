#!/bin/bash
# skrypt wysyłający maile o danej tresci do adresatów zapisanych
# w pliku tekstowym podanym jako argument skryptu
if [ -f $1 ] and [ -s $1]
then
    while read -r line
    do
        mail -s 'Test skryptu zadanie_5.sh' $line <<< 'Mail wysłany w celach testowych'
    done < $1   
else
    echo "Nie mozna odczytac takiego pliku."
fi