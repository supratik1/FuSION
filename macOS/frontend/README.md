# FuSION Frontend (macOS)

How to compile and run the FuSION Java frontend on macOS.

## Prerequisites

- **JDK 21 or newer** — verify with `java -version` (tested with 25). Install via
  `brew install openjdk` if needed.
- **The `fusion` executable must already be built.** The frontend runs it to
  perform the analysis. From the `macOS` directory, run `make`. See
  [../README.md](../README.md).

## Directory layout

`out/` is created by compilation; `sessions/` is created on first save. Neither is
tracked in git.

```
FuSION/
└── macOS/
    ├── fusion                 (built by `make`, run by the frontend)
    ├── advtempscript.txt      (batch script template, read at runtime)
    ├── output_script.txt      (generated per run)
    └── frontend/
        ├── lib/               (json-20250517.jar)
        ├── out/               (created after compilation)
        ├── resources/         (icons)
        ├── sessions/          (created automatically)
        └── src/
            ├── LoginPage.java (main class)
            └── ...
```

## Compile and run

> Run every command from the **`macOS` directory**, not from `macOS/frontend`.
>
> The frontend opens `./fusion`, `advtempscript.txt`, and `output_script.txt` as
> paths relative to the working directory. Launching from `frontend/` — or from
> anywhere else — means it will not find the executable or the template.

### 1. Change into the macOS directory

```bash
cd FuSION/macOS
```

### 2. Compile the sources

Compiles every `.java` under `frontend/src` into `frontend/out`, with the JSON
library on the classpath.

```bash
javac -d frontend/out -cp "frontend/lib/json-20250517.jar" frontend/src/*.java
```

Re-run this after any change under `frontend/src`. The application loads compiled
classes from `frontend/out`; editing a `.java` file alone has no effect.

### 3. Launch

```bash
java -cp "frontend/out:frontend/resources:frontend/lib/json-20250517.jar" LoginPage
```

The login page should appear.

## How a run works

1. You fill in the input fields across the wizard panels.
2. `ScriptFile` reads the `advtempscript.txt` template, substitutes your values
   into its `%__...__%` placeholders, and writes `output_script.txt`.
3. The GUI runs `./fusion -b output_script.txt`.
4. Results are written to the working directory you selected, and `gnuplot`
   renders the Pareto plot.

Because step 2 is a plain text substitution, `output_script.txt` is the single
best place to check what the backend actually received. If a run misbehaves, open
it and confirm your inputs appear as you expect.

## Notes

- **The regulatory thresholds only register when you press `Filter`.** On the
  threshold panel, `Next` does not read the up/down fields. Other panels commit
  their typed values on `return`, focus loss, or `Next`.
- **The coexpression CSV is optional.** Leaving it empty means no coexpression
  pairs are loaded, so that constraint contributes nothing. The frozen-edge
  constraints are independent of it and still apply, driven by the frozen
  threshold and the edge `experiment_score` values in the pathway XML.
- **Sessions** are stored as JSON under `sessions/` and reloaded on login, so
  fields carry over between runs. Values you never touch keep whatever the saved
  session held.
