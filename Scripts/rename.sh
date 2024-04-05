#!/bin/bash

# Recursive sed script to replace Hazel with Snowstorm and HZ with SS in all files

# Function to recursively process files in a directory
recursive_sed() {
    local dir="$1"
    local file
    # Process each file in the directory
    for file in "$dir"/*; do
        # If it's a directory, recursively call the function
        if [[ -d "$file" ]]; then
		echo "Processing directory $file ..."
            	recursive_sed "$file"
	elif [[ -f "$file" && ("$file" == *.cpp || "$file" == *.h) ]]; then
            	# If it's a file, perform the sed replacements
		echo "Processing file $file ..."
		num_changes=$(sed -i 's/Hazel/Snowstorm/g' "$file" | wc -l)
		num_changes=$((num_changes + $(sed -i 's/HZ/SS/g' "$file" | wc -l)))
		num_changes=$((num_changes + $(sed -i 's/hzpch/pch/g' "$file" | wc -l)))
		num_changes=$((num_changes + $(sed -i 's/OpenGL/Vulkan/g' "$file" | wc -l)))
		echo "Number of changes applied: $num_changes"
        fi
    done
}

# Call the function starting from the current directory
recursive_sed "$(pwd)"
