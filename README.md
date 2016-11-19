# scclust

[![Build Status](https://travis-ci.org/fsavje/scclust.svg?branch=master)](https://travis-ci.org/fsavje/scclust)

**scclust** is a C99 library for size constrained clustering. Subject to user-specified conditions on the minimum size and composition of the clusters, scclust derives a partition of a set of data points so that the similarity of points assigned to the same cluster is maximized.

scclust is made with large data sets in mind, and it can cluster hundreds of millions of data points within minutes on an ordinary desktop computer. It's also fairly easy to adapt scclust to run with exotic databases or other distance metrics as all data management is separated from the rest of the library.

The library is currently in alpha, and breaking changes to the API might happen.


## How to use scclust

The library can be used in several different ways, for different purposes. We recommend looking at the examples distributed with the code and the documentation for the full picture. Here we give you a minimal working example showing the simple use case: we have a set of data points and want to construct clusters that contain at least three points each.

```c
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include "../include/scclust.h"

int main(void) {
	// Variable for error reporting
	scc_ErrorCode ec;

	// Our data set: 10 two-dimensional points
	double raw_data[20] = { 0.088316633,  0.807443027,
	                       -1.080004390,  0.969638235,
	                        1.316503268,  1.492648875,
	                        0.140829264, -0.100874408,
	                       -0.832028346,  0.655380698,
	                        1.392129281,  0.885861553,
	                       -2.764608404, -0.449692089,
	                        0.296104717,  0.943822811,
	                       -0.641513488,  0.417153186,
	                        1.509341003, -0.026534633 };

	// Construct scclust data set object
	scc_DataSet* data_set;
	ec = scc_init_data_set(10, 2, 20, raw_data, false, &data_set);
	if(ec != SCC_ER_OK) return 1; // Check error

	// Make empty clustering object
	scc_Clabel cluster_labels[10];
	scc_Clustering* cl;
	ec = scc_init_empty_clustering(10, cluster_labels, &cl);
	if(ec != SCC_ER_OK) return 1;

	// Construct a clustering where each cluster contains at least 3 points
	ec = scc_make_clustering(cl, data_set, 3, 0, NULL, 0, NULL,
	                         SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, false, 0.0,
	                         0, NULL, SCC_UM_IGNORE, false, 0.0);
	if(ec != SCC_ER_OK) return 1;

	printf("Cluster assignment:\n");
	for(size_t i = 0; i < 10; ++i) {
		printf("%d ", cluster_labels[i]);
	}
	printf("\n");

	// Free cluster and data set objects
	scc_free_clustering(&cl);
	scc_free_data_set(&data_set);
}
```

The example produces:

```bash
Cluster assignment:
0 2 1 0 2 1 2 0 0 1
```

That is, the first point is assigned to cluster `0`, the second to cluster `2` and so on.

This example is distributed with the library. You can run it by calling the following in the scclust folder
```bash
cd example && make
./simple_example.out
```


## How to compile scclust

This snippet downloads and compiles scclust:
```bash
wget https://github.com/fsavje/scclust/archive/master.zip
unzip master.zip 
cd scclust-master && make library
```

scclust compiles into a static library by default. 

Calling `make` without the `library` target compiles the documentation as well (requires DoxyGen).


## Compile options

### Debug mode

Compiles the library with a number of debug asserts. This will slow down execution, but is helpful to catch bugs.

To turn on when calling `make`:
```bash
make library DEBUG=Y
```

To turn **off** when compiling individual files (on by default):
```bash
cc -c -DNDEBUG src/digraph_core.c -o digraph_core.o
```


### Test mode

Compiles the library with test suite that watches for overflows and memory leaks. Use only when actively testing the library.

To turn on when calling `make`:
```bash
make library TESTH=Y
```

To turn on when compiling individual files:
```bash
cc -c -include tests/test_suite.h src/digraph_core.c -o digraph_core.o
```

### Cluster label data type

To change the date type used to store cluster labels, change the `typedef` for `scc_Clabel` in `include/scclust.h`. Remember to change `SCC_CLABEL_MAX` and `SCC_CLABEL_NA` to appropriate values. The library has been tested with `scc_Clabel` set to `uint32_t`, `uint64_t` and `int`.


### Type label data type

To change the date type used to store type labels, change the `typedef` for `scc_TypeLabel` in `include/scclust.h`. The library has been tested with `scc_Clabel` set to `uint_fast16_t` and `int`.


### Data point ID data type

The data type used to store data point IDs. This choice restricts the maximum number of points in any clustering problem solved by scclust -- a wider type allows larger problems but requires more memory. The possible types are:

| Flag            | Data type  | Max data points |
| --------------- | ---------- | --------------- |
| SCC_DPID_UINT32 | `uint32_t` | $2^{32} − 1$    |
| SCC_DPID_UINT64 | `uint64_t` | $2^{64} − 1$    |
| SCC_DPID_INT    | `int`      | $2^{31} − 1$    |

The default is `SCC_DPID_UINT32`. 

To change type when calling `make`:
```bash
make library DPID=SCC_DPID_INT
```

To change type when compiling individual files:
```bash
cc -c -DSCC_DPID_INT src/digraph_core.c -o digraph_core.o
```


### Arc reference data type

The data type used to store arc references. This choice restricts the size of the graphs used by scclust to solve clustering problems -- a wider type allows larger problems but requires more memory. A rough estimate of the number of the required maximum arc reference is given by $\{\text{number of points}\} \times \{\text{minimum size of clusters}\}$. The possible types are:

| Flag      | Data type  | Max arc ref   |
| --------- | ---------- | ------------- |
| SCC_ARC32 | `uint32_t` | $2^{32} − 1$  |
| SCC_ARC64 | `uint64_t` | $2^{64} − 1$  |

The default is `SCC_ARC32`. 

To change type when calling `make`:
```bash
make library ARC=SCC_ARC64
```

To change type when compiling individual files:
```bash
cc -c -DSCC_ARC64 src/digraph_core.c -o digraph_core.o
```


### The ANN library for nearest neighbor search

scclust can use the [ANN library](https://www.cs.umd.edu/~mount/ANN/) to do all nearest neighbor searches. This will usually speed up execution considerably, but requires that the library is linked with a C++ linker.

To turn on when calling `make`:
```bash
make library ANN_SEARCH=Y
```

When using the ANN library, it is recommended to compile with `SCC_DPID_INT`. This avoids costly type translations between the libraries. 


### Tree data structure used by ANN

The ANN library can use both k-d trees (`SCC_ANN_KDTREE`) and box-decomposition trees (`SCC_ANN_BDTREE`). The latter is more robust to clustered data points. See the [ANN manual](https://www.cs.umd.edu/~mount/ANN/Files/1.1.2/ANNmanual_1.1.pdf) for more details.

To change tree when calling `make`:
```bash
make library ANN_SEARCH=Y ANN_TREE=SCC_ANN_BDTREE
```

To change tree when compiling individual files:
```bash
cc -c -DSCC_ANN_BDTREE src/digraph_core.c -o digraph_core.o
```

The default is `SCC_ANN_KDTREE`. This option is ignore if the ANN library is not used for nearest neighbor search.


## How to contribute to scclust

Thank you for considering contributing to scclust. This is a free, open source library and it gets better if we all help out. There are many ways to do that: report bugs, suggest useful new features and submitting code that implements enhancements and bug fixes. If possible, use Github's internal tools for to do so: [Issues](https://github.com/fsavje/scclust/issues) for bug reports and suggestions, and [Pull requests](https://github.com/fsavje/scclust/pulls) for code. If you're new to Github, read this [guide](https://guides.github.com/activities/contributing-to-open-source/) to learn more.

If you're filing a bug, please include the information needed to reproduce the bug. Besides the code you're trying to running, information about your platform and compilation options is often useful.


## License

scclust is distributed under the [GNU Lesser General Public License v2.1](https://github.com/fsavje/scclust/blob/master/LICENSE).

