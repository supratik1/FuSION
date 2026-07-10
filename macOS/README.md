# FuSION on macOS

Build and run instructions for the macOS port. Verified on macOS 26.5.1 (Tahoe),
Apple Silicon (arm64).

All commands below are run in Terminal. To open it, press `command + space`, type
`Terminal`, and press `return`.

---

## 1. Prerequisites

### Xcode Command Line Tools

These supply `clang`/`g++`, `make`, `bison`, and `flex`. Install them first:

```bash
xcode-select --install
```

You do **not** need Homebrew's `bison`. Apple ships bison 2.3, and the grammars in
this tree are written to stay compatible with it (see [Troubleshooting](#troubleshooting)).

### Homebrew

Install Homebrew, which provides the remaining libraries:

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
brew --version   # verify
```

### Libraries

```bash
brew install libxml2 libxml++ z3 gnuplot openjdk
```

`libxml++` automatically pulls in `glibmm@2.66`, `libsigc++@2`, and `glib`, which
the tool also links against. Verify with:

```bash
z3 --version         # tested with 4.15.4
gnuplot --version    # tested with 6.0
java -version        # JDK 21 or newer; tested with 25
brew list --versions libxml++ libxml2 glibmm@2.66 libsigc++@2 glib
```

`gnuplot` is required — the tool shells out to it to render the Pareto plots.
Without it the analysis still runs, but no `.png` is produced.

---

## 2. Build the `fusion` executable

From the `macOS` directory:

```bash
cd macOS
make
```

This produces the executable `fusion` in the `macOS` directory.

The `Makefile` assumes Homebrew's Apple Silicon prefix (`/opt/homebrew`). On an
Intel Mac, Homebrew installs to `/usr/local` instead, so override the prefix:

```bash
make BREW_HOME=$(brew --prefix)
```

To rebuild from scratch:

```bash
make clean && make
```

---

## 3. Run the graphical frontend

The frontend is a Java application under `macOS/frontend`. Compile it once:

```bash
javac -d frontend/out -cp "frontend/lib/json-20250517.jar" frontend/src/*.java
```

Then launch it:

```bash
java -cp "frontend/out:frontend/resources:frontend/lib/json-20250517.jar" LoginPage
```

> **Run both commands from the `macOS` directory, not from `macOS/frontend`.**
> The frontend resolves `./fusion`, `advtempscript.txt`, and `output_script.txt`
> as paths relative to the working directory. Launching from anywhere else will
> fail to find the executable or the script template.

Build `fusion` (step 2) before launching the frontend — the GUI invokes it to run
the analysis.

Recompile the frontend whenever you change anything under `frontend/src`; the GUI
loads the compiled `.class` files from `frontend/out`, not the sources.

See [frontend/README.md](frontend/README.md) for more detail.

### The older `GUIForExecutable` launcher

`GUIForExecutable.java` in this directory is an earlier, standalone GUI. It still
works for basic runs, but it does not populate the coexpression CSV, coexpression
threshold, or frozen-edge threshold inputs, so analyses launched from it always
run with the new constraints disabled. Prefer the `LoginPage` frontend above.

---

## 4. Run from the command line

`fusion` reads a batch script:

```bash
./fusion -b <batch_script>
./fusion --help          # list available commands
```

The frontend simply generates such a script (`output_script.txt`) from the values
you enter, then runs `./fusion -b output_script.txt`.

---

## 5. Verify your install with the model example

`model_example/` contains a complete worked analysis (EGFR → CXCL8, testing JUN
for significance). It is the fastest way to confirm the tool is working.

- **[model_example/RUNNING.md](model_example/RUNNING.md)** — how to run it, from
  the command line and from the frontend, with every parameter value listed.
- [model_example/README.md](model_example/README.md) — what the example means and
  how the reduced master graph was built.

`run_model_example.batch` ships with a placeholder working directory, so you must
edit its `let $WORK_DIR` line before running:

```
let $WORK_DIR /ABSOLUTE/PATH/TO/FuSION/macOS/model_example
```

Change it to an absolute path of your choosing (it must contain the example's
`inputs/` directory). Then, from the `macOS` directory:

```bash
./fusion -b model_example/run_model_example.batch
```

Results land in `$WORK_DIR/run/`. A correct install reproduces this Pareto
frontier:

```
with JUN    : (2,0) (1,1) (0,2)
without JUN : (3,0) (2,1) (1,2) (0,3)
```

and writes an overlaid plot, `PO_hsa1956_to_hsa3576_fsc_hsa3725_..._b10.png`.

---

## Troubleshooting

**`ld: library not found for -lxml++-2.6` (or `-lz3`)**
Homebrew is installed under a prefix the `Makefile` does not expect. Rebuild with
`make BREW_HOME=$(brew --prefix)`.

**The GUI reports it cannot find `./fusion`**
You launched Java from the wrong directory, or you have not run `make`. See step 3.

**No `.png` plot is produced**
`gnuplot` is not installed, or not on your `PATH`.
