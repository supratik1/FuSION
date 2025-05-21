# Installing FuSION on Linux (tested on Ubuntu 22.04)

1. Download libxml2 and libxml++-2.6 to run the program. Download files in WSL (Debian) or Linux (Ubuntu) environment using the following commands :

   ​ For libxml2 :-

   ​ `sudo apt install libxml2 libxml2-dev` // installation.

   ​ `pkg-config --modversion libxml-2.0` // verify installation.

   ​ For libxml++

   ​ `sudo apt install libxml++2.6-dev` // installation.

   ​ `pkg-config --modversion libxml++-2.6` // verify installation

2. Download the Z3 SMT solver and relevant header files for WSL (Debian) or Linux environment using the following command:

   ​ `sudo apt install z3 libz3-dev` // installation.

   ​ `z3 --version` // verify installation

3. Check if bison is installed in the computer by running `bison --version` . If it is shown not to be found then run the following command for installation and verification :

   ​ `sudo apt install bison` // installation

   ​ `bison --version` // verify installation

4. Navigate to the `Linux` directory and run the following command to build the tool.  

   ​ `make`

  The executable called `fusion` is generated in the `Linux` directory.

5. To remove the installation of `fusion`, simply run the following command in the `Linux` directory.

     `make clean`
