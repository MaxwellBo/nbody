from itertools import product

numTimeSteps = 1000
time = 10
fps = 1

outputInterval = (numTimeSteps / time) / fps
deltaT = time / numTimeSteps

batch_args = """#!/bin/bash
#SBATCH --partition=cosc
#SBATCH --job-name=mbody
#SBATCH --nodes={nodes}
#SBATCH --ntasks={ntasks}
#SBATCH --cpus-per-task={cpus_per_task}
#SBATCH --error=batcherr/{name}.log
#SBATCH --output=batchout/{name}.out
"""

logging = """DATE=$(date +"%Y%m%d%H%M")
export OMP_NUM_THREADS=${{SLURM_CPUS_PER_TASK}}
echo "SLURM_JOB_NAME=$SLURM_JOB_NAME "
echo "SLURM_JOB_ID=$SLURM_JOB_ID "
echo "SLURM_NODELIST=$SLURM_NODELIST "
echo "DATE=$DATE "
echo "OMP_NUM_THREADS=$OMP_NUM_THREADS "
echo "SLURM_TASKS_PER_NODE=$SLURM_TASKS_PER_NODE "
echo "----------------"
"""

def make_batch(
    bodies,
    nodes,
    tasks,
    cpus_per_task,
    enable_barnes_hut
):
    name = "mbody-b{boies}-n{nodes}-t{tasks}-cpt{cpus_per_task}"

    if enable_barnes_hut:
        name += "-bh"

BODIES = [16, 64, 256, 1024, 4096]
NODES = [1, 4, 8, 12]
TASKS = [1, 4, 8, 12]
CPUS_PER_TASK = [1, 8, 24]
ENABLE_BARNES_HUT = [True, False]

def main():
    for (bodies, nodes, tasks, cpus_per_task, enable_barnes_hut) 
    in product(BODIES, NODES, TASKS, CPUS_PER_TASK, ENABLE_BARNES_HUT):
        make_batch
        


# numTimeSteps outputInterval deltaT inputFile
INVOCATION = """time mpirun -n ${{SLURM_NPROCS}} ./nbody {numTimeSteps} {outputInterval} {deltaT} {inputFile} 
""".format(
    numTimeSteps=numTimeSteps,
    outputInterval=outputInterval,
    deltaT=deltaT,
    inputFile=inputFile
)

merged = batch_args + logging + invocation

if __name__ == "__main__":
    main()