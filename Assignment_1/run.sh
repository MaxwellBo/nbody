hash=`git rev-parse HEAD`
name="$hash"

make && ./nbody 600000 0.016666666666666666 10 $1 > "outputs/$name.data" && python3 viz.py "outputs/$name.data" "outputs/$name.mp4" && open "outputs/$name.mp4"

