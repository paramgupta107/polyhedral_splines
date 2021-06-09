# This script automatically run ./SemiStructuredSpline on each .obj file
# in /testfile. CMake will copy this file into build folder automatically.

# Get path to obj folder
script_path="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
obj_folder_path="${script_path}/../testfile"

# Search for all .obj files
obj_arr=($(find "$obj_folder_path" -iname *.obj -exec basename {} .obj ';'))

# Make a folder for storing .bv files
output_folder="output"
mkdir $output_folder

# Feed obj to SemiStructuredSpline one by one
for obj_file in "${obj_arr[@]}"
do
   ./SemiStructuredSpline "$obj_folder_path/$obj_file.obj"
   mv "$obj_file.bv" $output_folder
done
