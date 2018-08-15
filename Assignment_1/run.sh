touch big.in && rm big.in && python3 ./generate.py > big.in && make && ./nbody $1 out
