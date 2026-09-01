# Installing FuSION on Linux (tested on Ubuntu 22.04 and 24.04)

## Prerequisites

### 1. libxml2 and libxml++-2.6

```bash
# Install
sudo apt install libxml2 libxml2-dev

# Verify
pkg-config --modversion libxml-2.0
```

```bash
# Install
sudo apt install libxml++2.6-dev

# Verify
pkg-config --modversion libxml++-2.6
```

### 2. Z3 SMT Solver

```bash
# Install
sudo apt install z3 libz3-dev

# Verify
z3 --version
```

### 3. Bison

Check if already installed:
```bash
bison --version
```
If not found:
```bash
sudo apt install bison
bison --version
```

### 4. Flex

Check if already installed:
```bash
flex --version
```
If not found:
```bash
sudo apt install flex
flex --version
```

### 5. Java Development Kit (JDK 17 or higher)

The graphical front end requires Java 17+.

Check if already installed:
```bash
java -version
```
If not found or version is below 17:
```bash
sudo apt install openjdk-17-jdk
java -version
```

---

## Building FuSION

Navigate to the `Linux` directory and run:

```bash
make
```

This generates the `fusion` executable in the `Linux` directory.

---

## Running the Front End

FuSION ships with a graphical user interface for configuring and running analyses. The front end handles sessions, working directories, gene IDs, pathway configuration, and pipeline execution.

> **Requires JDK 21 or higher.** Check your version with `java -version` before proceeding.

### Step 1 — Compile the front end

From inside the `Linux` directory, run:

```bash
javac -d frontend/out -cp "frontend/lib/json-20250517.jar" frontend/src/*.java
```

### Step 2 — Launch the application

java -cp "frontend/out:frontend/resources:frontend/lib/json-20250517.jar" LoginPage


The application opens with a login/signup screen. Create an account or log in to start a new analysis session.

---

## Un-installing FuSION

To remove the compiled `fusion` executable:

```bash
make clean
```

To also remove the libraries installed during setup:

```bash
sudo apt remove libxml2 libxml2-dev libxml++2.6-dev z3 libz3-dev bison flex
```
