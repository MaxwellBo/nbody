#!/bin/bash
#SBATCH --partition=cosc
#SBATCH --job-name=mbody
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=16
#SBATCH --error=stderr.log

DATE=$(date +"%Y%m%d%H%M")
echo "time started "$DATE
export OMP_NUM_THREADS=${SLURM_CPUS_PER_TASK}
echo "This is job ’$SLURM_JOB_NAME’ (id: $SLURM_JOB_ID) running on the following nodes:"
echo $SLURM_NODELIST
echo "running with OMP_NUM_THREADS=$OMP_NUM_THREADS "
echo "running with SLURM_TASKS_PER_NODE=$SLURM_TASKS_PER_NODE "
echo "--------------------:"
time mpirun -n ${SLURM_NPROCS} ./nbody 1000000 1666.6666666666667 1e-05 inputs/in_8_body_counterclockwise
