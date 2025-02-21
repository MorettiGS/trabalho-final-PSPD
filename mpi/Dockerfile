# Etapa de construção
FROM mpioperator/openmpi-builder:v0.6.0 AS builder

COPY conta_palavrasMPI.c conta_palavrasMPI.c

RUN mpicc -fopenmp -o /conta_palavrasMPI /conta_palavrasMPI.c

FROM mpioperator/openmpi:v0.6.0

RUN apt-get update && apt-get install -y libgomp1

COPY --from=builder /conta_palavrasMPI /home/mpiuser/conta_palavrasMPI

RUN chmod +x /home/mpiuser/conta_palavrasMPI

ENTRYPOINT ["mpirun", "--allow-run-as-root", "-np", "4", "/home/mpiuser/conta_palavrasMPI"]
