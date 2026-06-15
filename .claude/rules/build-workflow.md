# Build Workflow

## After any code change: verify compilation

After making code changes, run `./rebuildmacos.sh` to confirm the project compiles before considering the task done. Skip only for purely non-code changes (docs, config, scripts).

## After a successful build: run create_release.sh

A successful build alone is not enough to test the game. Always follow the build with `./create_release.sh` to package the binary with game assets into `bin/`.

The standard compound command:

```sh
./rebuildmacos.sh && ./create_release.sh
```

The game binary is `bin/d2tm` and must be run from the repo root.
