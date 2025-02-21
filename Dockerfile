FROM alpine:latest AS builder
RUN apk add --no-cache build-base
COPY conta_palavras.c .
RUN gcc -static -o /conta_palavras conta_palavras.c

FROM alpine:latest
COPY --from=builder /conta_palavras /conta_palavras
ENTRYPOINT ["/conta_palavras"]
