## FuSION: Functional Significance In nOisy Networks

**FuSION** was developed at IIT Bombay jointly by 
* S. Akshay, Sukanya Basu and Supratik Chakraborty (IIT Bombay), 
* Rangapriya Sundararajan and Prasanna Venkatraman (ACTREC, Tata Memorial Centre), 
* With help from Jahnabi Roy (IIT Delhi).

## What is it about?
Given a collection of gene regulatory networks (currently KEGG networks), microarray data from wet-lab experiment, 
and source (stimulus), target (observation) and implicated (potentially functionally significant) actors in the
wet-lab experiment, **FuSION** uses a fusion of biological domain knowledge encoded in gene regulatory networks,
state-of-the-art SMT solving and Pareto theory to help determine if the implicated actors play a functionally 
signficant role in the stimuls-to-observation signaling chains encoded in the networks, in a manner that is
consistent with the microarray data.

Since microarray data is susceptible to some degree of noise, and since publicly available gene regulatory networks may have 
errors or imprecisely captured relations, it is important that such noise/errors are factored in before we implicate a 
candidate actor as playing a functionally significant role.  Having said the above, it is also important to realize that
microarray technology is fairly advanced today, and publicly available gene regulatory networks are periodically curated 
and revised to remove errors.  Therefore, while we must allow the possibility of noise/errors, it is unreasonable to expect
overwhelmingly large noise in microarray data or massive errors in publicly available gene regulatory networks. In cognizance
of this, **FuSION** allows the user to specify two kinds of relaxations: 
* A bounded number of edges in the networks may be considered to be in error.  The user need not specify which specific edges are
  in error, and **FuSION** searches over all bounded subsets of edges that can potentially be in error.  If the user knows that
  some edges in the networks are definitely not in error, she/he/they can specify this explicitly to **FuSION**.  In this case,
  such edges are never considered as erroneous in the search conducted by **FuSION**.
* A bounded number of microarray reads may be considered to be in error.  The current version of **FuSION** works by thresholding
  these reads using user-provided thresholds to classify actors as *differentially expressed*, *differentially repressed* or *unknown*.
  A relaxation of microarray reads allows a bounded number of *differentially expressed* or *differentially repressed* actors to
  have their classification changed.  The user need not specify which specific reads are in error, and **FuSION** searchers over
  all bounded subsets of differentially expressed reads that can potentially be in error. If the user knows that some differentially
  expressed reads are definitely not in error, she/he.they can specify this explicitly to **FuSION**.  In this case, such reads
  are never considered as erroneous in the search conducted by **FuSION**.
  
**FuSION** tries to determine whether stimulus-to-observation signaling circuits can be extracted from the provided gene regulatory 
networks, while being consistent with the microarray reads (modulo bounded relaxations, as discussed above). For example, **FuSION**
may find that removing an implicated actor from the regulatory networks makes it impossible to extract stimulus-to-observation 
signaling circuits unless larger bounds of relaxations are used for network edges and/or microarray reads.  In such a case, the
implicated actor potentially plays a functionally significant positive role in the stimulus-to-observation signaling.  Similarly,
if removing the actor makes it possible to extract stimulus-to-observation signaling circuits with smaller bounds of relaxations
than those necessary when the actor was present, the actor potentially plays a functionally significant negative role in the
stimulus-to-observation signaling.

**FuSION** presents the results of its reasoning as a pair of Pareto curves to the user.  One of these curves is constructed with 
the implicated actor present in the regulatory networks, while the other is constructed without the actor. Each curve describes the
minimum relaxations necessary to allow the discovery of signaling circuits from stimulus to observation.  If the generated pair of 
curves don't overlap or intersect, one infers that the implicated actor is likely to be functionally significant in the signaling 
circuits from stimulus to observation, given the noise bounds provided.  In case the curves coincide or intersect, one cannot 
infer the functional significance of the implicated actor within the given noise bounds.

For more details of the theory, please go through the citations at the end of this page.

## Tool version 
Currently, FuSION runs on Linux (tested on Ubuntu 22.04).  Please check out the ```Linux``` sub-directory of the repository for 
the code and installation instructions. 

We should soon have a version for Windows 11.  So watch out for updates!

## License and citation
FuSION is provided under MIT license.

In case you use the tool, please cite as follows:

S. Akshay, S. Basu, S. Chakraborty, R. Sundararajan and P. Venkataraman, *Functional Significance Checking in Noisy Gene Regulatory Networks*, 
in Proc. of International Conference on Principles and Practice of Constraint Programming (CP), pages 767-785, Oct 2019

S. Basu, *Functional Significance Checking in Biological Networks: Theory and Implementation*, Ph.D. Dissertation, Department of Computer 
Science and Engineering, IIT Bombay, 2020
