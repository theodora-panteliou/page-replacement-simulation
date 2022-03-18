# Page Replacement Simulation
A page replacement simulation using the LRU (Least Recently Used) and Second Chance algorithms.

- HashedPT: Hashed Page Table implementation
- MMem: Main Memory management
- main: Reads the traces (bzip.trace, gzip.trace) and inserts into main memory using the MMem.h interface.
- .trace files: Contain traces of refereces. Each file contains references to memory each accompanied with R (read) or W (write). 

## Compile & Run
Compile: `make`\
Run: `./main Replacement-Algorithm num_frames q max_references`

input:
- Replacement-Algorithm: LRU (Least Recently Used) or 2C (Second Chace).
- num_frames: Number of frames that memory has.
- q: How many references will be read from each trace alternately.
- max: Total max number of references.

output: 
- Page fault count
- Read from disk count
- Write to disk count
- Number of references read

## Page Replacement Algorithms
### Least Recently Used
Discards the least recently used items first. The `timecounter` variable, defined in MMem.c keeps track of time by increasing every time a new reference is read. A page table entry keeps the timestamp that a page is inserted to memory and the page with the oldest time is being replaced when LRU algorithm is used.

### Second Chance
In the Second Chance page replacement policy, the candidate pages for removal are considered in a round robin matter, and a page that has been accessed between consecutive considerations will not be replaced. The page replaced is the one that, when considered in a round robin matter, has not been accessed since its last consideration. In the implementation, a reference bit is used. Each time a new page is added or hit, the reference bit is set to True. While searching for the victim page, the reference bit is set to False if it was True, or selected for replacement if it was already False.
