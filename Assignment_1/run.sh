hash=`git rev-parse --short HEAD`
name="$1-$hash"

make && ./nbody 6000000 0.016666666666666666 10 "inputs/$1" > "outputs/$name.data" 
echo "Exported data to ./outputs/$name.data"
python3 viz.py "outputs/$name.data" "outputs/$name.mp4" && open "outputs/$name.mp4"

