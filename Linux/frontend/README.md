# FuSION Application Guide

This guide provides instructions on how to compile and run the FuSION Java application.

## Prerequisites

Before you begin, ensure you have the **Java Development Kit (JDK)** version 21 (Check with `java -version`) installed on your system and that the `javac` and `java` commands are accessible from your terminal.

## Directory Structure

The project expects the following directory layout. The `out` directory will be created automatically during compilation.

```
FuSION/
└── Linux/
    └── frontend/
        ├── lib/
        ├── out/      (Created after compilation)
        ├── resources/
        ├── sessions/ (Will be created automatically)
        └── src/
            ├── LoginPage.java
            └── ... (other .java source files)

```

## Compilation and Execution

Follow these steps from your terminal to compile and run the application.

---

### 1. Navigate to the Project Directory

First, change your current directory to the `FuSION/Linux` folder where the project files are located.

```bash
cd FuSION/Linux
```

### 2. Compile the Java Source Code

Next, compile all .java files from the `frontend/src` directory. This command places the compiled .class files into the `frontend/out` directory and includes the necessary JSON library in the classpath.

```bash
javac -d frontend/out -cp "frontend/lib/json-20250517.jar" frontend/src/*.java
```

### 3. Run the Application

Finally, execute the application by running the `LoginPage` main class. This command sets the classpath to include the compiled classes, the resources folder, and the JSON library.

```bash
java -cp "frontend/out:frontend/resources:frontend/lib/json-20250517.jar" LoginPage
```

The application's login page should now be running.
