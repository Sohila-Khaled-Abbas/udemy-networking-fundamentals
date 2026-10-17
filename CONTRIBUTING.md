# Contributing and Lab Maintenance

Thanks for helping improve this learning repository. The goal is to keep each networking exercise small, understandable, reproducible, and safe to run locally.

## Working agreement

- Preserve the original learning objective. Prefer a focused fix over an unnecessary rewrite.
- Keep operating-system differences explicit: POSIX sockets belong in the POSIX source; native Windows socket examples use Winsock.
- Prefer built-in language modules unless a third-party dependency teaches a specific concept.
- Keep defaults bound to loopback (`127.0.0.1`). Do not introduce public listeners, router changes, credential shortcuts, or destructive setup steps.
- Explain observable behavior, expected output, common failure modes, and what a diagnostic proves.
- Never add generated certificates, private keys, packet captures, executables, or machine-specific paths to commits.

## Before changing a lab

1. Read the module's README and follow the existing command sequence.
2. Make one change at a time and note the expected behavior.
3. Run the available local validation:

   ```powershell
   .\scripts\validate-repo.ps1
   ```

4. For a specific module, run its exact commands from the README in separate terminals.
5. Inspect `git diff --check` and `git status --short` before committing.

The validation helper performs syntax/build checks where the relevant tools are installed. A passing static check is not a substitute for exercising network behavior.

## Style and documentation

- Use clear names, explicit error handling, and comments that explain *why*, not comments that merely restate code.
- Keep examples reproducible from the repository root or clearly state the working directory.
- Use PowerShell for Windows instructions and Bash for Linux/WSL instructions.
- Distinguish a TCP-port check from an application-level HTTP test.
- Record important limitations: these samples are educational and are not production-ready services.
- Prefer descriptive Markdown headings, short paragraphs, tables for comparisons, and fenced code blocks with a language identifier.

## Commit messages

Use concise, action-oriented messages. Conventional Commit prefixes are welcome:

- `docs: clarify the HTTPS verification lab`
- `fix: handle empty UDP datagrams safely`
- `test: validate Go and Node.js examples in CI`
- `chore: remove generated binaries from source control`

## Scope

This repository is a personal learning portfolio built around networking fundamentals for backend and data-platform work. Changes should help a learner explain what was tested, reproduce the result, and connect the concept to APIs, database connectivity, service integration, observability, or reliability.
