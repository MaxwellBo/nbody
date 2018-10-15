#!/bin/bash
#SBATCH --partition=smp
#SBATCH --job-name=mbody
#SBATCH --nodes=4
#SBATCH --ntasks=4
#SBATCH --cpus-per-task=4
#SBATCH --error=stderr.log

DATE=$(date +"%Y%m%d%H%M")
echo "time started "$DATE
export OMP_NUM_THREADS=${SLURM_CPUS_PER_TASK}
echo "This is job ’$SLURM_JOB_NAME’ (id: $SLURM_JOB_ID) running on the following nodes:"
echo $SLURM_NODELIST
echo "running with OMP_NUM_THREADS=$OMP_NUM_THREADS "
echo "running with SLURM_TASKS_PER_NODE=$SLURM_TASKS_PER_NODE "
echo "--------------------:"
time mpirun -n 4 ./nbody 5 1 1 inputs/in_12_body_counterclockwise
