FROM debian:bookworm-slim

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        libssl3 \
        ca-certificates && \
    rm -rf /var/lib/apt/lists/*

RUN useradd -r -u 1000 -m appuser

WORKDIR /opt/app

# Copy the pre-built server binary from build/server
COPY build/server/PWChat /opt/app/PWChat

RUN chmod +x /opt/app/PWChat && \
    chown -R appuser:appuser /opt/app

# Directories for volumes (config files and repo data)
RUN mkdir -p /opt/app/config /opt/app/repo && \
    chown -R appuser:appuser /opt/app/config /opt/app/repo

USER appuser

EXPOSE 8090

ENTRYPOINT ["/opt/app/PWChat"]
