var=$(date +%u)
if [ $var -gt 5 ]
then
	echo "Miłego weekendu !" 
else
	echo "Witamy w pracy!"
fi
