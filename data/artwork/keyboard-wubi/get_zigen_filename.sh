#! /bin/bash

set -e

for name in a b c d e f g h i j k l m n o p q r s t u v w x y
do
    upper=$(echo $name | tr a-z A-Z)
    echo "  case '$name':"
    echo "  case '$upper':"
#    echo "    ret = ZIGEN_FILENAME (${upper}, i);"
    echo "    ret = ZIGEN_SIZE (${upper});"
    echo "    break;"
done
