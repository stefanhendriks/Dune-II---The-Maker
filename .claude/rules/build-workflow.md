# Build Workflow

## After any code change: verify compilation

After making code changes, run `./rebuildmacos.sh` to confirm the project compiles before considering the task done. Skip only for purely non-code changes (docs, config, scripts).

## After a successful build: run create_release.sh

A successful build alone is not enough to test the game. Always follow the build with `./create_release.sh` to package the binary with game assets into `bin/`.

The standard compound command:

```sh
./rebuildmacos.sh && ./create_release.sh
```

On Windows (from an MSYS2 UCRT64 shell) the equivalent is `rebuildwin.bat` followed by `create_release.bat`.

The game binary is `bin/d2tm` (`bin/d2tm.exe` on Windows). Run it with `bin/` as the working directory: data paths such as `settings.ini` and `skirmish/` are resolved relative to it.

## Unit tests

`./rebuildmacos.sh` (and `rebuildwin.bat`) already runs the ctest suite (`tests/`) via `ctest --output-on-failure` as its last step. A build isn't done until these pass too, not just the compile.

## Manual smoke-testing

When launching the packaged binary to manually verify a change, always pass `-nomusic -nosound`:

```sh
cd bin && ./d2tm -nomusic -nosound
```
