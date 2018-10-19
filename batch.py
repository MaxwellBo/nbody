from __future__ import print_function
from itertools import product
import json

BODIES = [4, 16, 64, 256, 1024, 4096]
NODES = [1, 2, 4, 8, 12]
TASKS_PER_NODE = [1, 2, 4, 8]
CPUS_PER_TASK = [1, 2, 4, 8]
ENABLE_BARNES_HUT = [True, False]

BATCH_ARGS = """#!/bin/bash
#SBATCH --partition=cosc
#SBATCH --job-name={name}
#SBATCH --nodes={nodes}
#SBATCH --ntasks-per-node={tasks_per_node}
#SBATCH --cpus-per-task={cpus_per_task}
#SBATCH --error=./batcherr/{name}.log
#SBATCH --output=./batchout/{name}.out
#SBATCH --time=0:00:30
"""

LOGGING = """DATE=$(date +"%Y%m%d%H%M")
export OMP_NUM_THREADS=${SLURM_CPUS_PER_TASK}
echo "DATE=$DATE"
echo "SLURM_JOB_NAME=$SLURM_JOB_NAME"
echo "SLURM_JOB_ID=$SLURM_JOB_ID"
echo "SLURM_JOB_NUM_NODES=$SLURM_JOB_NUM_NODES"
echo "SLURM_NODELIST=$SLURM_NODELIST"
echo "SLURM_NTASKS=$SLURM_NTASKS"
echo "SLURM_TASKS_PER_NODE=$SLURM_TASKS_PER_NODE"
echo "SLURM_CPUS_PER_TASK=$SLURM_CPUS_PER_TASK"
echo "OMP_NUM_THREADS=$OMP_NUM_THREADS"
echo '----------------'
"""

# numTimeSteps outputInterval deltaT inputFile
INVOCATION = "time mpirun -n ${{SLURM_NPROCS}} ./nbody {numTimeSteps} {outputInterval} {deltaT} {inputFile} {enableBarnesHut}\n"

num_time_steps = 10
time = 10
fps = 1

output_interval = (num_time_steps / time) / fps
delta_t = time / num_time_steps

def merge_two_dicts(x, y):
    z = x.copy()
    z.update(y)
    return z

def generate_inputs(bodies):
    with open("batchin/{bodies}.in".format(bodies=bodies), "w+") as f:

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
    tasks_per_node,
    cpus_per_task,
    enable_barnes_hut
):
    name = "mbody-b{bodies}-n{nodes}-tpn{tasks_per_node}-cpt{cpus_per_task}".format(
        bodies=bodies,
        nodes=nodes,
        tasks_per_node=tasks_per_node,
        cpus_per_task=cpus_per_task
    )

    if enable_barnes_hut:
        name += "-bh"

    input_file = "batchin/{bodies}.in".format(bodies=bodies)

    batch_args = BATCH_ARGS.format(
        nodes=nodes,
        tasks_per_node=tasks_per_node,
        cpus_per_task=cpus_per_task,
        name=name
    )

    invocation = INVOCATION.format(
        numTimeSteps=num_time_steps,
        outputInterval=output_interval,
        deltaT=delta_t,
        inputFile=input_file,
        enableBarnesHut=(1 if enable_barnes_hut else 0)
    )

    return name, batch_args + LOGGING + invocation

def batch():
    for n in BODIES:
        generate_inputs(n)

    for (bodies, nodes, tasks_per_node, cpus_per_task, enable_barnes_hut)\
    in product(BODIES, NODES, TASKS_PER_NODE, CPUS_PER_TASK, ENABLE_BARNES_HUT):
        if cpus_per_task * tasks_per_node > 24:
            continue

        name, batch = make_batch(
            bodies=bodies,
            nodes=nodes,
            tasks_per_node=tasks_per_node,
            cpus_per_task=cpus_per_task,
            enable_barnes_hut=enable_barnes_hut
        )

        with open("batches/{name}.sh".format(name=name), "w+") as f:
            f.write(batch)

def collate_data():
    data = []

    for (bodies, nodes, tasks_per_node, cpus_per_task, enable_barnes_hut)\
    in product(BODIES, NODES, TASKS_PER_NODE, CPUS_PER_TASK, ENABLE_BARNES_HUT):
        if cpus_per_task * tasks_per_node > 24:
            continue

        name, batch = make_batch(
            bodies=bodies,
            nodes=nodes,
            tasks_per_node=tasks_per_node,
            cpus_per_task=cpus_per_task,
            enable_barnes_hut=enable_barnes_hut
        )

        entry = {
            "name": name,
            "bodies": bodies,
            "nodes": nodes,
            "tasksPerNode": tasks_per_node,
            "cpusPerTask": cpus_per_task,
            "enable_barnes_hut": enable_barnes_hut
        }

        try:
            with open("./batchout/{name}.out".format(name=name)) as o:
                text = o.read()

                slurm_details = text.split("----------------")[0]

                for line in slurm_details.split('\n'):
                    if line != "":
                        k, v = line.split('=')
                        entry[k] = v
        except Exception as err:
            print("Failed ./batcherr/{name}.log with {err}".format(name=name, err=err))
            pass

        try:
            with open("./batcherr/{name}.log".format(name=name)) as e:
                text = e.read()
                info, time = text.split('\n\n')

                for line in time.split('\n'):
                    if line != "":
                        k, v = line.split()
                        entry[k] = v
                        m, s = v[:-1].split('m')
                        total_seconds = (float(m) * 60) + float(s)
                        entry[k] = total_seconds

                entry["time"] = entry["user"] + entry["sys"]
                # :-1 because there's a trailing comma

                info_dict = json.loads("{" + info[:-1] + "}")

                data.append(merge_two_dicts(entry, info_dict))

                print("Success with {name}".format(name=name))
                continue
        except Exception as err:
            print("Failed ./batcherr/{name}.log with {err}".format(name=name, err=err))
            data.append(entry)
            continue

    with open("data.json", "w+") as f:
        print("Dumping data to data.json")
        f.write(json.dumps(data))

if __name__ == "__main__":
    answer = raw_input("(b)atch or (c)ollate data? ")

    if answer == "b":
        batch()
    elif answer == "c":
        collate_data()
