#! /bin/bash

gen()
{
    count=$(ls -1 png/${1}*\.png | wc -l)
    #echo "${1}_num=$count;"
    echo "char *${1}_png[$count] = {"
    for name in png/${1}*.png
    do
	filename=$(echo $name | sed 's/\.png$//' | sed 's/^png\///')
	echo "    \"${filename}.png\","
    done
    echo "};"
}

echo "/* generate by zigen/main.sh */"
echo
echo "#define ZIGEN_PNG_PREFIX    \"zigen/png/\""
echo "#define ZIGEN_NAME(name, index)	name##_png[index]"
echo
for name in A B C D E F G H I J K L M N O P Q R S T U V W X Y
do
    gen $name
done