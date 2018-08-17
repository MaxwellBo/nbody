hash=`git rev-parse --short HEAD`
name="$1-$hash"

# numTimeSteps outputInterval deltaT inputFile
time=10
fps=60

numTimeSteps=1000000
outputInterval=`python3 -c  "print(($numTimeSteps / $time) / $fps)"`
deltaT=`python3 -c  "print($time / $numTimeSteps)"`

make && ./nbody $numTimeSteps $outputInterval $deltaT  "inputs/$1" > "outputs/$name.data" && echo "Exported data to ./outputs/$name.data"
python3 viz.py "outputs/$name.data" "outputs/$name.mp4" && open "outputs/$name.mp4"

