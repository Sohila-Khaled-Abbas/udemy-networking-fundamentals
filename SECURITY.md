# Security Policy

## Scope

This repository contains intentionally small networking demonstrations for learning. It is **not** a hardened application or a production server.

The examples should remain local by default and use the loopback address `127.0.0.1`. Do not use them to probe systems without permission or expose them to untrusted networks.

## Handling local lab credentials

The Section 6 HTTPS exercise generates a local test CA and a localhost server certificate under `section-06-protocols/certs/`.

- Keep `dev-ca-key.pem` and `localhost-key.pem` private.
- Do not commit private keys, environment secrets, or real credentials.
- Do not install the lab CA as a general-purpose trusted root.
- Do not disable TLS certificate validation to make a lab pass.
- Regenerate and keep development credentials separate from any real environment.

The repository ignores generated credentials and common build artifacts. An ignore rule does not remove a file that has already been committed, so review staged changes before pushing.

## Reporting a concern

For a security concern in this public learning repository, contact the repository owner privately through their [GitHub profile](https://github.com/Sohila-Khaled-Abbas). Do not publish private keys, tokens, or exploit details in a public comment.

## Safe-use boundary

Only capture packets from your own lab traffic or traffic you are authorized to inspect. Keep the sample servers on loopback unless you have deliberately reviewed the consequences of changing the bind address. Do not treat a passing example or CI check as evidence that a service is production-secure.
