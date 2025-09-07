#!/bin/bash

# Set locale to UTF-8 for this script
export LC_ALL=C.UTF-8

# Get the directory of the current script
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Directories and files
TEAMCODE_DIR="$SCRIPT_DIR/../../teamcode"
JAVA_OUTPUT_DIR="$SCRIPT_DIR/../../../java/org/firstinspires/ftc/teamcode/opmodes"
TEMPLATE_DIR="$SCRIPT_DIR"

# Generalized regex for searching specific OpMode comments
OP_MODE_ANNOTATION='@(?:TeleOp|Autonomous)(\(([^)]*)\))?'
DISABLED_ANNOTATION='@Disabled'

# Function to create the Java class
create_java_class() {
    local mode=$1
    local displayNameContent=$2
    local group=$3
    local preselectTeleOp=$4
    local className=$5
    local disabled=$6

    local template_file="${TEMPLATE_DIR}/${mode#@}.java.template"
    local java_file="${JAVA_OUTPUT_DIR}/${className}.java"

    if [[ ! -f "$template_file" ]]; then
        echo "Error: Template file $template_file not found!" >&2
        exit 1
    fi

    # Replace placeholders in the template
    sed -e "s/<displayName>/$( [[ -n "$displayNameContent" ]] && echo "name = \"${displayNameContent}\"" )/g" \
        -e "s/<className>/${className}/g" \
        -e "s/<mainMethodName>/${className}/g" \
        -e "s/<group>/$( [[ -n "$group" ]] && echo "$( [[ -n "$displayNameContent" ]] && echo ", " )group = \"${group}\"" )/g" \
        -e "s/<preselectTeleOp>/$( [[ -n "$preselectTeleOp" ]] && echo "$( [[ -n "$displayNameContent" || -n "$group" ]] && echo ", " )preselectTeleOp = \"${preselectTeleOp}\"" )/g" \
        -e "/<disabled>/ { $( [[ "$disabled" == true ]] && echo "s/<disabled>/@Disabled/" || echo "d" ) }" \
        "$template_file" | sed -e 's/,\s*}/}/g' -e 's/{\s*,/{/g' -e 's/name = "", //g' -e 's/, group = "", //g' -e 's/, preselectTeleOp = "", //g' > "$java_file"
}

# Function to process a file
process_file() {
    local file=$1
    content=$(<"$file")

    # Use grep to search for the pattern
    if grep -Pzo "$OP_MODE_ANNOTATION" "$file" >/dev/null 2>&1; then
        mode=$(grep -Po '@(TeleOp|Autonomous)' "$file" | head -1)
        params=$(grep -Po '@(TeleOp|Autonomous)\(([^)]*)\)' "$file" | head -1 | sed 's/@\(TeleOp\|Autonomous\)//' | tr -d '()')

        # Initialize variables
        displayNameContent=""
        group=""
        preselectTeleOp=""

        # Extract parameters
        if [[ -n "$params" ]]; then
            IFS=',' read -ra param_array <<< "$params"
            for param in "${param_array[@]}"; do
                param=$(echo $param | xargs)  # Trim whitespace
                param=$(echo $param | sed 's/ *= */=/g')  # Remove spaces around '='
                case $param in
                name=*) displayNameContent=$(echo $param | cut -d'=' -f2 | tr -d '"') ;;
                group=*) group=$(echo $param | cut -d'=' -f2 | tr -d '"') ;;
                preselectTeleOp=*) preselectTeleOp=$(echo $param | cut -d'=' -f2 | tr -d '"') ;;
                *)
                    echo "Error: Invalid parameter '$param' in file: $file" >&2
                    exit 1
                    ;;
                esac
            done
        fi

        if [[ "$mode" == "@TeleOp" && -n "$preselectTeleOp" ]]; then
            echo "Error: 'preselectTeleOp' is not allowed with '@TeleOp' in file: $file" >&2
            exit 1
        fi

        if grep -q "$DISABLED_ANNOTATION" "$file"; then
            disabled=true
        else
            disabled=false
        fi

        # Extract method name
        method_name=$(grep -Pzo 'extern\s+"C"\s+void\s+\K\w+(?=\(\);)' "$file" | tr -d '\0')

        if [[ -n "$method_name" ]]; then
            create_java_class "$mode" "$displayNameContent" "$group" "$preselectTeleOp" "$method_name" "$disabled"
        else
            echo "Warning: No method name found in file: $file" >&2
        fi
    else
        # Fuzzy search for similar annotations
        similar_annotation=$(grep -Po '@(T[eE]?[lL]?[eE]?[oO]?[pP]?|T[eE][lL][eE][oO][pP][a-zA-Z]?|A[uU]?[tT]?[oO]?[nN]?[oO]?[mM]?[oO]?[uU]?[sS]?|A[uU][tT][oO][nN][oO][mM][oO][uU][sS][a-zA-Z]?)' "$file" | head -1)
        if [[ -n "$similar_annotation" ]]; then
            echo "Warning: Similar annotation '$similar_annotation' found in file: $file" >&2
        fi
    fi
}

# Function to recursively scan .hpp files
scan_files() {
    find "$TEAMCODE_DIR" -type f -name "*.hpp" | while read -r file; do
        process_file "$file"
    done
}

# Function to delete all relevant Java classes
cleanup_java_files() {
    find "$JAVA_OUTPUT_DIR" -type f -name "*.java" | while read -r java_file; do
        if grep -q 'Do_not_remove_this_string' "$java_file"; then
            rm "$java_file"
        fi
    done
}

# Main function
main() {
    cleanup_java_files
    scan_files
}

# Call the main function
main
