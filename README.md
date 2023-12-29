## FuSION: Functional Significance In nOisy Networks

**FuSiON** was developed at IIT Bombay jointly by S. Akshay, Sukanya Basu and Supratik Chakraborty (IIT Bombay), 
Rangapriya Sundararajan and Prasanna Venkatraman from ACTREC (Tata Memorial Centre), 
with help from Jahnabi Roy (IIT Delhi).

## What is it about?
Given a collection of gene regulatory networks (currently KEGG networks), microarray data from wet-lab experiments, 
and source (stimulus), target (observation) and implicated (potentially functionally significant) actors in the
regulatory network, **FuSION** uses SMT solving and Pareto theory to help decide if the implicated actor plays a 
functionally signficant role in triggering the observation due to the stimulus. The tool reasons about functional 
significance of the implicated actor in the presence of bounded noise (aka errors or relaxations) in gene regulatory network
edges and in microarray data. Thus, one can specify a bounded number of edges in the networks being in error, and
also a bounded number of microarray data points being in error.  

The tool generates a pair of curves, one with and one without the implicated actor in the regulatory network, each describing
how noise affects explaining the stimulus-to-observation signaling chain.  In case the two curves are separated, the
implicated actor is deemed to be functionally significant for the noise bounds provided.  In case the curves coincide,
one cannot infer the functional significance of the implicated actor for the given noise bounds.


## Tool version 
Currently, FuSION runs on Linux (tested on Ubuntu 22.04).  Please check out the linux sub-directory of the repository for 
the code and installation instructions. 

We should soon have a version for Windows 11.  So watch out for updates!

## License and citation
FuSION is provided under MIT license.

In case you use the tool, please cite it as follows:

S. Akshay, S. Basu, S. Chakraborty, R. Sundararajan and P. Venkataraman, *Functional Significance Checking in Noisy Gene Regulatory Networks*, 
in Proc. of International Conference on Principles and Practice of Constraint Programming (CP), pages 767-785, Oct 2019

S. Basu, *Functional Significance Checking in Biological Networks: Theory and Implementation*, Ph.D. Dissertation, Department of Computer 
Science and Engineering, IIT Bombay, 2019
