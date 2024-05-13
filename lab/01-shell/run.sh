#!/bin/bash

FILE=$1
OPREATION=$2
NEW_SCRIPT="doit.sh"
SELF_SCRIPT="run.sh"
README="README.md"

find . -type f -not -name "$SELF_SCRIPT" -not -name "$README" -delete

if (($# != 2))
then
    echo "usage: ./$SELF_SCRIPT <target filename> <operation>"
    exit 1
fi


cat > "$NEW_SCRIPT" << 'EOF'
#!/bin/bash

FILE=$1
OPERATION=$2

if [[ $OPERATION == "read" ]]; then
    cat "$FILE"
elif [[ $OPERATION == "write" ]]; then
    echo "547 MYFILE" >> "$FILE"
else
    echo "Invalid operation"
    exit 2
fi
EOF

chmod +x "$NEW_SCRIPT"
bash "$NEW_SCRIPT" "$FILE" "$OPREATION"

exit 0