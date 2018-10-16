from itertools import product

BODIES = [16, 64, 256, 1024, 4096]
NODES = [1, 4, 8, 12]
TASKS = [1, 4, 8, 12]
CPUS_PER_TASK = [1, 8, 24]
ENABLE_BARNES_HUT = [True, False]

BATCH_ARGS = """#!/bin/bash
#SBATCH --partition=cosc
#SBATCH --job-name=mbody
#SBATCH --nodes={nodes}
#SBATCH --ntasks={ntasks}
#SBATCH --cpus-per-task={cpus_per_task}
#SBATCH --error=batcherr/{name}.log
#SBATCH --output=batchout/{name}.out
"""

LOGGING = """DATE=$(date +"%Y%m%d%H%M")
export OMP_NUM_THREADS=${SLURM_CPUS_PER_TASK}
echo "SLURM_JOB_NAME=$SLURM_JOB_NAME"
echo "SLURM_JOB_ID=$SLURM_JOB_ID"
echo "SLURM_NODELIST=$SLURM_NODELIST"
echo "DATE=$DATE"
echo "OMP_NUM_THREADS=$OMP_NUM_THREADS"
echo "SLURM_TASKS_PER_NODE=$SLURM_TASKS_PER_NODE"
echo '----------------'
"""

# numTimeSteps outputInterval deltaT inputFile
INVOCATION = "time mpirun -n ${{SLURM_NPROCS}} ./nbody {numTimeSteps} {outputInterval} {deltaT} {inputFile}"

num_time_steps = 10
time = 10
fps = 1

output_interval = (num_time_steps / time) / fps
delta_t = time / num_time_steps

def generate_inputs(bodies):
    with open("batchin/" + str(bodies), "w+") as f:

        print(bodies, file=f)

        print(str(bodies * 10) + "e14", file=f) # sun 

        for i in range(1, bodies):
            print(str(i) + "e14", file=f) # small bodies start on the inside

        print(0.00, 10, file=f)

        print(0, 0, 0, 0, file=f) # sun

        for i in range(1, bodies):
            inverse = bodies - i

            print(
                0,
                0 + i * 10,
                0 + inverse + 10,
                0,
                file=f
            )

def make_batch(
    bodies,
    nodes,
    tasks,
    cpus_per_task,
    enable_barnes_hut
):
    name = "mbody-b{bodies}-n{nodes}-t{tasks}-cpt{cpus_per_task}".format(
        bodies=bodies,
        nodes=nodes,
        tasks=tasks,
        cpus_per_task=cpus_per_task
    )

    if enable_barnes_hut:
        name += "-bh"

    input_file = "batchin/{bodies}.in".format(bodies=bodies)

    batch_args = BATCH_ARGS.format(
        nodes=nodes,
        ntasks=tasks,
        cpus_per_task=cpus_per_task,
        name=name
    )

    invocation = INVOCATION.format(
        numTimeSteps=num_time_steps,
        outputInterval=output_interval,
        deltaT=delta_t,
        inputFile=input_file
    )

    return name, batch_args + LOGGING + invocation

def main():
    for (bodies, nodes, tasks, cpus_per_task, enable_barnes_hut)\
    in product(BODIES, NODES, TASKS, CPUS_PER_TASK, ENABLE_BARNES_HUT):
        name, batch = make_batch(
            bodies=bodies,
            nodes=nodes,
            tasks=tasks,
            cpus_per_task=cpus_per_task,
            enable_barnes_hut=enable_barnes_hut
        )

        with open("batches/" + name, "w+") as f:
            f.write(batch)

        
if __name__ == "__main__":
    main()