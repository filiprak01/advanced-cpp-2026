FROM debian:bookworm-slim AS build

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        build-essential \
        cmake \
        make \
        libssl-dev \
        nlohmann-json3-dev \
        ca-certificates && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /opt/app
COPY . /opt/app

RUN mkdir -p build && cd build && \
    cmake .. && \
    cmake --build . --target PWChat -- -j$(nproc)

FROM debian:bookworm-slim
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        libssl3 \
        ca-certificates && \
    rm -rf /var/lib/apt/lists/*

RUN useradd -r -u 1000 -m appuser
WORKDIR /opt/app

COPY --from=build /opt/app/build/server/PWChat /opt/app/PWChat
RUN chmod +x /opt/app/PWChat && \
    chown -R appuser:appuser /opt/app

RUN mkdir -p /opt/app/config /opt/app/repo && \
    chown -R appuser:appuser /opt/app/config /opt/app/repo

USER appuser
EXPOSE 8090
ENTRYPOINT ["/opt/app/PWChat"]
