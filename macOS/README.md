# Installing FuSION on macOS (tested on macOS Sonoma 14.4.1)

The following instruction need to be executed a Terminal window. To open Terminal on your mac, go to your applications,open the "Other" folder and click on the terminal app. Alternatively, you may press `command + space`, type in `Terminal` in the spotlight search and press `return`.

1. Install Homebrew (We have used version 4.3.2), from where the remaining libraries need to be installed to run the program. To install Homebrew use the following command in terminal (in the home directory):

    `/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"` // installation.
    
    `brew --version` // verify installation.

2. Download libxml2 and libxml++-2.6 to run the program. Download files in WSL (Debian) or Linux (Ubuntu) environment using the following commands :

   ​ For libxml2 :-

   ​ `brew install libxml2` // installation.

   ​ `brew info libxml2` // verify installation.
   ​   ​ 
   ​ For libxml++

   ​ `brew install libxml++` // installation.

   ​ `brew info libxml++` // verify installation
   ​ 
   ​ The brew info commands also list out the dependencies. Make sure all the dependencies(python_setuptools, python@3.11 and python@3.12 for libxml and meson, ninja, pkg-config for libxml++) listed as part of the info are installed (indicated by a green tick mark). 
   ​ In case a certain dependency is not present(indicated by a red cross), use command `brew install <dependancy name>` in terminal.
   ​ 
   ​ example: If you wish to install dependency "python-setuptools". Enter command `brew install python-setuptools` in terminal.


3. After successful installation, we need to download the SMT solver Z3. We have used version 4.8.12.0 of Z3 for FuSION. Z3 can  be downloaded from Homebrew using the following command:

   ​ `brew install z3` // installation.

   ​ `z3 --version` // verify installation
   ​ 
   ​ `brew info z3` // verify dependencies
   ​ 
   ​ The brew info commands also list out the dependencies. Make sure all the dependencies(cmake, python@3.12) listed as part of the info are installed (indicated by a green tick mark). 
   ​ In case a certain dependency is not present(indicated by a red cross), use command `brew install <dependancy name>` in terminal.
   ​ 
   ​ example: If you wish to install dependency "cmake". Enter command `brew install cmake` in terminal.

4. Check if bison is installed in the computer by running `bison --version` . If it is shown not to be found then run the following command for installation and verification :

   ​ `brew install bison` // installation

   ​ `bison --version` // verify installation
   ​ 
5. Check if java is installed in the computer by running `java --version`. If it is not shown to be found then you can either download it from [Here]{https://www.oracle.com/in/java/technologies/downloads/#jdk22-mac} as any other app from the internet or run the following command in terminal:

    `brew install openjdk` // installation
    
    `brew info openjdk` // verify installation
    
6. Install gnuplot for seeing the graph plots output by the tool by running the following commads in terminal:

    `brew install gnuplot` // installation

    `brew info gnuplot` // verification

7. Navigate to the `macOS` directory in the terminal and run the following command to build the tool.  

   ​ `make`

  The executable called `fusion` is generated in the `macOS` directory.
  
8. To run the tool, navigate to the `macOS` directory under FuSION, then enter the following commands:

    `javac GUIForExecutable.java` // compiles the java application
    
    `java GUIForExecutable` // to launch the app

