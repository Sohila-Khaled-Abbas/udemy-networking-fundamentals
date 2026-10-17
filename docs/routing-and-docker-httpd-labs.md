# Routing, host networking, and Docker networking labs (Windows 11)

These hands-on labs connect core routing concepts to a local Apache HTTP server running from the official `httpd` container image. Commands are PowerShell-friendly and designed for a developer workstation.

## Learning outcomes

By the end, you should be able to:

- inspect the Windows routing table and understand route selection at a high level;
- distinguish a listening service from a route to that service;
- run Apache HTTP Server in Docker and publish it only on host loopback;
- inspect Docker networks, container addresses, gateways, and published ports;
- understand the difference between host routing, Docker bridge networking, and port publishing;
- clean up the lab without changing system-wide network configuration.

> **Safety defaults:** this lab publishes Apache on `127.0.0.1` only. Do not replace it with `0.0.0.0` unless you intentionally want to expose it to other interfaces and have reviewed the firewall implications. Do not add static host routes or change firewall defaults for these exercises.

## Prerequisites

- Windows 11 with PowerShell.
- Docker Desktop installed and running with Linux containers enabled.
- Internet access to pull the official `httpd` image the first time.
- Optional: `curl.exe` (included with current Windows versions).

Check Docker availability:

```powershell
docker version
docker info
```

If Docker commands cannot connect to the daemon, open Docker Desktop and wait until its engine is running. These labs do not require administrator privileges unless your Docker installation or environment specifically requires them.

---

## Lab 1 — Inspect Windows routing before running a container

### 1.1 View the IPv4 routing table

```powershell
route print -4
```

Look for:

- `0.0.0.0/0` (shown as network destination `0.0.0.0`, mask `0.0.0.0`): a default route, typically used when no more-specific route matches;
- directly connected subnet routes: routes for networks attached to a local interface;
- interface addresses and metrics.

A routing table is not a list of open ports. It describes where packets should be forwarded; a firewall and a listening application are separate concerns.

### 1.2 Inspect active routes with PowerShell

```powershell
Get-NetRoute -AddressFamily IPv4 | Sort-Object DestinationPrefix, RouteMetric | Format-Table DestinationPrefix, NextHop, InterfaceAlias, RouteMetric, State -AutoSize
```

Inspect local interface addresses:

```powershell
Get-NetIPConfiguration | Format-List InterfaceAlias, IPv4Address, IPv4DefaultGateway, DNSServer
```

### 1.3 Observe route selection without changing routes

Use a public IP only as a diagnostic destination; this does not test application availability:

```powershell
Test-NetConnection 1.1.1.1 -DiagnoseRouting -InformationLevel Detailed
```

If that option is unsupported by your PowerShell version, use `route print -4` and `Get-NetRoute` instead. Do not add or delete routes just to complete this lab.

**Record:** the selected interface, source address, next hop, and the most-specific matching route if reported. A successful route decision does not guarantee that the destination responds or that a firewall permits the connection.

---

## Lab 2 — Run Apache using the official httpd image

### 2.1 Pull the image

```powershell
docker pull httpd:2.4
```

The `2.4` tag tracks the Apache HTTP Server 2.4 line. For reproducible long-term experiments, record the image digest shown by `docker image inspect httpd:2.4 --format '{{index .RepoDigests 0}}'` after pulling it.

### 2.2 Start the server on loopback only

```powershell
docker run -d --name networking-httpd -p 127.0.0.1:8088:80 httpd:2.4
```

What the port mapping means:

- `127.0.0.1`: bind the published host port to loopback, rather than all host interfaces;
- `8088`: host-side port chosen for this lab;
- `80`: Apache's HTTP port inside the container.

This does not assume that port 8088 is available. If Docker reports that the port is already allocated, inspect the conflicting process/container and choose another unused host port; do not stop an unrelated service blindly.

### 2.3 Verify HTTP from the Windows host

```powershell
docker ps --filter name=networking-httpd
Test-NetConnection 127.0.0.1 -Port 8088
curl.exe -i http://127.0.0.1:8088/
```

Expected result: the TCP check succeeds and the HTTP response is normally `HTTP/1.1 200 OK`, with the Apache welcome page HTML. If TCP succeeds but HTTP fails, investigate the HTTP response and container logs rather than changing routes immediately.

### 2.4 Inspect container logs and port publishing

```powershell
docker logs networking-httpd
docker port networking-httpd
docker inspect networking-httpd --format '{{json .NetworkSettings.Ports}}'
```

The host's published endpoint and the container's internal listener are different endpoints. Docker Desktop's Linux containers run inside its managed Linux environment, so do not assume the container IP is directly reachable from Windows in the same way as a native Windows process.

---

## Lab 3 — Inspect Docker's bridge network and container addressing

### 3.1 Inspect the network attached to Apache

```powershell
docker inspect networking-httpd --format '{{json .NetworkSettings.Networks}}'
docker network ls
```

The default network is commonly named `bridge`, unless a custom network was selected. The returned address is a container-network address, not the published host endpoint.

### 3.2 Create an isolated user-defined bridge network

```powershell
docker network create networking-lab
```

Create another Apache container attached to this user-defined network, publishing it to a different loopback port:

```powershell
docker run -d --name networking-httpd-lab --network networking-lab -p 127.0.0.1:8089:80 httpd:2.4
```

Test it from Windows:

```powershell
curl.exe -i http://127.0.0.1:8089/
docker network inspect networking-lab
```

Inspect the network output and identify its subnet, gateway, and connected container. Docker assigns these values based on the available Docker network configuration; do not hardcode or assume a subnet in scripts.

### 3.3 Compare default bridge and user-defined bridge

```powershell
docker inspect networking-httpd --format '{{json .NetworkSettings.Networks}}'
docker inspect networking-httpd-lab --format '{{json .NetworkSettings.Networks}}'
```

Discuss in your notes:

1. Which network is each container attached to?
2. Which address is internal to the Docker network?
3. Which host address and port does a Windows client use?
4. Does the container IP need to be exposed to the Windows LAN for this loopback-published test to work?

For user-defined bridge networks, Docker provides network-scoped container name resolution. This is useful for multi-container application stacks. The two Apache containers in this lab are not configured as a reverse proxy or application cluster; they simply demonstrate network attachment and port publishing.

### 3.4 Inspect the container's network configuration

```powershell
docker inspect networking-httpd-lab --format 'IP={{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}} Gateway={{range .NetworkSettings.Networks}}{{.Gateway}}{{end}}'
docker inspect networking-httpd-lab --format '{{json .HostConfig.PortBindings}}'
```

Compare the container address and gateway with the Docker network's subnet and gateway from `docker network inspect networking-lab`. These addresses belong to Docker's internal network; they are not necessarily Windows host routes.

---

## Lab 4 — Understand the path from client to Apache

Use this simplified path for the request to `http://127.0.0.1:8088/`:

1. The client sends a TCP connection to Windows loopback port 8088.
2. Docker Desktop's port-publishing mechanism forwards the connection to port 80 of the container.
3. Apache accepts the HTTP request and returns a response.
4. The response travels back through the published-port path to the client.

This is a conceptual flow, not a packet-level guarantee of every implementation detail. Docker Desktop networking internals vary by platform/version.

Compare these commands:

```powershell
Test-NetConnection 127.0.0.1 -Port 8088
curl.exe -i http://127.0.0.1:8088/
docker ps --format 'table {{.Names}}\t{{.Ports}}'
```

- `Test-NetConnection` tests TCP reachability to the endpoint.
- `curl.exe` tests an HTTP exchange.
- `docker ps` shows the configured published-port mapping.

These checks prove different things.

## Lab 5 — Troubleshooting matrix

| Symptom | First checks | Avoid doing this first |
| --- | --- | --- |
| Docker daemon unavailable | Open Docker Desktop; run `docker info` | Reinstall Docker immediately |
| Host port allocation error | `docker ps`, `Get-NetTCPConnection -State Listen` | Stop unknown processes blindly |
| TCP test fails | `docker ps`, `docker port`, `docker logs networking-httpd` | Add a static route |
| TCP works but HTTP fails | `curl.exe -i`, `docker logs` | Disable Windows Firewall globally |
| Container has no expected network | `docker inspect`, `docker network inspect` | Hardcode an assumed container IP |
| Host/LAN cannot reach the service | Confirm the intended bind is `127.0.0.1` | Change the bind to `0.0.0.0` without an explicit need |

## Cleanup

Remove the lab containers and network when finished:

```powershell
docker rm -f networking-httpd networking-httpd-lab
docker network rm networking-lab
```

The network removal may report that the network does not exist if you did not create it. That is harmless. The pulled `httpd:2.4` image remains cached; remove it only if you no longer need it and no other container depends on it.

## Acceptance checklist

- [ ] Captured the Windows IPv4 routing table without modifying it.
- [ ] Identified a default route and a connected route.
- [ ] Started Apache using the official `httpd:2.4` image.
- [ ] Published ports only on `127.0.0.1`.
- [ ] Verified TCP and HTTP independently.
- [ ] Inspected Docker network, container address, gateway, and port bindings.
- [ ] Explained why a container IP differs from the host's published endpoint.
- [ ] Removed containers and the custom network.

## References

- [Docker networking overview](https://docs.docker.com/engine/network/)
- [Docker bridge driver](https://docs.docker.com/engine/network/drivers/bridge/)
- [Docker port publishing](https://docs.docker.com/engine/network/port-publishing/)
- [Official Apache HTTP Server image](https://hub.docker.com/_/httpd)
- [Microsoft: Get-NetRoute](https://learn.microsoft.com/en-us/powershell/module/nettcpip/get-netroute)
- [Microsoft: Test-NetConnection](https://learn.microsoft.com/en-us/powershell/module/nettcpip/test-netconnection)
