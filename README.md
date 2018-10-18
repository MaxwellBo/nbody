## Serial - MS1

- `./run.sh` [filename from `./inputs`]

e.g. `./run.sh in_8_body_counterclockwise`


## Parallel - MS2

- On Goliath: `module load mpi` 
- `make`
- `sbatch go.sh`

## Parallel - MS3

- On Dogmatix: `module load openmpi-1.8-x86_64 && module load gnu/6.3.0 && module load python`
- `make`
- `mkdir batches batcherr batchin batchout`
- `python batch.py`
- `for filename in batches/*; do sbatch $filename; done`


