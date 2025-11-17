# ---- Build Stage ----
FROM --platform=linux/arm64 debian:bullseye-slim AS build
RUN apt-get update && apt-get install -y --no-install-recommends \
    gcc \
    libc6-dev \
    libgpiod-dev \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /app
COPY src/main.c .
COPY src/ili9488_driver.c .
COPY src/ili9488_driver.h .
COPY logo.h .
RUN gcc -O2 -o main main.c ili9488_driver.c -lgpiod

# ---- Runtime Stage ----
FROM --platform=linux/arm64 debian:bullseye-slim
RUN apt-get update && apt-get install -y --no-install-recommends \
    libgpiod2 \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /app
COPY --from=build /app/main .
CMD ["./main"]
