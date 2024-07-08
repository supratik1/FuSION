## FuSION: Functional Significance In nOisy Networks

**FuSION** was developed at IIT Bombay jointly by 
* S. Akshay, Sukanya Basu and Supratik Chakraborty (IIT Bombay), 
* Rangapriya Sundararajan and Prasanna Venkatraman (ACTREC, Tata Memorial Centre), 
* With help from Jahnabi Roy (IIT Delhi) and Neelavo Sen (IIT Madras).

## Why FuSION?
An experimental molecular cell biologist often has to decide if a suspected actor plays a functionally significant role in a
wet-lab experiment that she/he/they just conducted.  The classical and most conclusive way to answer this question is to knock 
out the actor and repeat the wet-lab experiment.  If the outcomes of interest differ significantly in the two experiments, 
the suspected actor can be reasonably presumed to play a significant role in the stimulus-to-observation signaling circuits 
active in the experiment.  Unfortunately, repeating wet-lab experiments incurs non-trivial cost, both time and resource-wise.  

**FuSION** is an attempt to use computational techniques to help the biologist zoom down on likely actors (or filter out 
unlikely actors) that play functionally significant roles in a wet-lab experiment.  Since running a computational experiment 
is typically much cheaper (time and resource-wise) than conducting a wet-lab experiment, we hope **FuSION** will provide
significant value addition to experimental molecular cell biologists.  Initial experiments conducted jointly by the 
developers (including computer scientists and molecular cell biologists) have indeed confirmed this value addition, as reported
in the citations at the end of this page.

## What does FuSION do?
Given a collection of gene regulatory networks (currently KEGG networks), microarray reads from a wet-lab experiment, 
and stimulus, observation-of-interest and a suspected actor in the experiment, **FuSION** uses a combination of biological 
domain knowledge encoded in the networks, state-of-the-art SMT solving and Pareto theory to determine if the suspected
actor plays a functionally signficant role in the stimuls-to-observation signaling circuits present in the given 
regulatory networks, in a manner that is consistent with the microarray reads.  The stimulus and observation-of-interest
must be mapped to activated/deactivated/expressed/repressed states of suitable nodes in the regulatory networks for **FuSION**
to do its reasoning. The tool is best used as a computational filter to zoom down on likely actors (or equivalently, filter 
out unlikely actors) playing significant functional roles in wet-lab experiments.  

Since microarray data is susceptible to some noise-induced errors, and since publicly available gene regulatory networks may have 
some errors or imprecisely captured relations, it is important that such noise/errors are factored in before we implicate a 
suspect actor as playing a functionally significant role.  At the same time, it is also important to realize that microarray and
sequencing technology is fairly advanced today, and publicly available gene regulatory networks are periodically curated 
and revised to remove errors.  Therefore, while we must allow the possibility of some noise/errors, it is unreasonable to expect
overwhelmingly large noise in microarray data or massive errors in publicly available gene regulatory networks. In cognizance
of this, **FuSION** allows the user to specify two kinds of bounded relaxations: 
* A bounded number of edges in the networks may be considered to be in error.  The user need not specify which specific edges are
  in error, and **FuSION** can search over all bounded subsets of edges that can potentially be in error.  If the user knows that
  certain network edges are definitely not in error, she/he/they can specify these explicitly to **FuSION**.  In this case, such
  edges will never be considered as erroneous in the search conducted by **FuSION**.
* A bounded number of microarray reads may be considered to be in error.  The current version of **FuSION** works by thresholding
  microarray reads using user-provided threshold values to classify actors as *differentially expressed*, *differentially repressed*
  or *unknown*. A relaxation of microarray reads allows a bounded number of *differentially expressed* or *differentially repressed*
  actors to have their classification altered.  The user need not specify which specific reads are in error, and **FuSION** searches over
  all bounded subsets of differentially expressed reads that can potentially be in error. If the user knows that some differentially
  expressed reads are definitely not in error, she/he.they can specify these explicitly to **FuSION**.  The specified reads will
  then be excluded from the subsets of potentially erroneous reads considered by **FuSION**.
  
**FuSION** tries to determine whether stimulus-to-observation signaling circuits can be extracted from the provided gene regulatory 
networks, while being consistent with the microarray reads (modulo bounded relaxations, as discussed above). For example, **FuSION**
may find that removing an suspected actor from the regulatory networks makes it impossible to extract stimulus-to-observation 
signaling circuits unless very large (and unlikely) bounds of relaxations are used for network edges and/or microarray reads.  In such a case, the
implicated actor is likely to play a functionally significant *positive* role in the stimulus-to-observation signaling.  Similarly,
if removing the actor makes it possible to extract stimulus-to-observation signaling circuits with smaller bounds of relaxations
than those necessary when the actor was present, the actor potentially plays a functionally significant *negative* role in the
stimulus-to-observation signaling.

## Interpreting FuSION's outputs
**FuSION** presents the results of its reasoning as a pair of Pareto curves to the user.  One of these curves is constructed with 
the suspect actor present in the regulatory networks, while the other is constructed without the suspect actor. Each curve describes the
minimum relaxations necessary to allow the discovery of signaling circuits from stimulus to observation.  If the generated pair of 
curves don't overlap or intersect, it is reasonable to infer that the suspect actor plays a functionally significant role in the 
signaling from stimulus to observation, given the noise/relaxation bounds provided.  In case the curves coincide or intersect, one cannot 
infer the functional significance of the implicated actor within the given noise/relaxation bounds.  In such cases, it is possible that the
suspect actor plays no significant role in the signaling circuits, or that the actor plays a significant role in some signaling circuits,
although compensatory signaling circuits may be operational in the absence of the actor. To probe further in such cases, we may need
to specify that certain other actors/signaling edges must/must not be present in the stimulus-to-observation signaling circuits
(**FuSION** allows such specification) and the computational experiments repeated with and without the suspected actor being present.

For more details of the theory, please go through the citations at the end of this page.

## Tool version 
Currently, FuSION runs on Linux (tested on Ubuntu 22.04) and MacOS.  Please check out the ```Linux``` or ```macOS`` sub-directory of the repository for 
the code and installation instructions. 

We should soon have a version for Windows 11.  So watch out for updates!

## License and citation
FuSION is provided under MIT license.

In case you use the tool, please cite as follows:

S. Akshay, S. Basu, S. Chakraborty, R. Sundararajan and P. Venkataraman, *Functional Significance Checking in Noisy Gene Regulatory Networks*, 
in Proc. of International Conference on Principles and Practice of Constraint Programming (CP), pages 767-785, Oct 2019

S. Basu, *Functional Significance Checking in Biological Networks: Theory and Implementation*, Ph.D. Dissertation, Department of Computer 
Science and Engineering, IIT Bombay, 2020
