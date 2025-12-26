FROM erlang:26-alpine

# Instalar build tools: make, gcc, musl-dev (y otras dependencias)
RUN apk add --no-cache make build-base

WORKDIR /app

# Copiar código y Makefile
COPY ./src /app/src
COPY ./Makefile /app/Makefile
COPY ./compartida /app/compartida
COPY ./descargas /app/descargas

# Compilar módulos
RUN make

# Exponer los dos puertos que usa tu servidor
EXPOSE 12345 12346

# Debug
RUN echo "Archivos en /app:" && ls -l /app
RUN echo "Archivos en /app/src:" && ls -l /app/src

# Arrancar tu aplicación
CMD ["erl","-pa", ".", "-noshell"]

