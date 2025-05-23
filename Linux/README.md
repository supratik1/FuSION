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

3. Check if `bison` is installed on your computer by running `bison --version` . If `bison` cannot be found, then run the following command for installation and verification :

   ​ `sudo apt install bison` // installation

   ​ `bison --version` // verify installation

4. Check if `flex` is installed on your computer by running `flex --version`.  If `flex` cannot be found, then run the following command for installation and verification:

   `sudo apt install flex` // installation

   `flex --version` // verify installation
   
5. Navigate to the `Linux` directory and run the following command to build the tool.  

   ​ `make`

  The executable called `fusion` is generated in the `Linux` directory.


# Running FuSION on Linux (tested on Ubuntu 22.04)

1. Compile the Java-based front end in the `Linux` directory using the following command

     `javac GUIForExecutable.java` // you must have JDK installed on your machine3. 

2. Next run the front end as follows

      `java GUIForExecutable`
   

# Un-installing FuSION 

1. Run the following command in the `Linux` directory to uninstall `fusion`
   
     `make clean`
   
2. If you also want to un-install the other libraries installed above (while installing `fusion`), you can optionally run the following command

     `sudo apt remove libxml2 libxml2-dev libxml++2.6-dev z3 libz3-dev bison`

   


   
