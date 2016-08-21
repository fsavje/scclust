# scclust

[![Build Status](https://travis-ci.org/fsavje/scclust.svg?branch=master)](https://travis-ci.org/fsavje/scclust)

**scclust** is a C99 library for size constrained clustering. Subject to user-specified conditions on the minimum size and composition of the clusters, scclust derives a partition of a set of data points so that the similarity of points assigned to the same cluster is maximized.

scclust is made with large data sets in mind, and it can cluster hundreds of millions of data points within minutes on an ordinary desktop computer. It's also fairly easy to adapt scclust to run with exotic databases or other distance metrics as all data management is separated from the rest of the library.

The library is currently in alpha, and breaking changes to the API might happen.

## How to compile scclust

This snippet downloads and compiles scclust:
```bash
wget https://github.com/fsavje/scclust/archive/master.zip
unzip master.zip 
cd scclust-master && make library
```

scclust compiles into a static library by default. 

## How to use scclust

The library can be used in several different ways, for different purposes. We recommend looking at the examples distributed with the code and the documentation for the full picture. Here we give you a minimal working example showing the simple use case: we have a set of data points and want to construct clusters that contain at least three points each.

```c
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include "../include/scclust.h"
#include "../include/scc_data_obj.h"

int main(void) {
	// Variable for error reporting
	scc_ErrorCode ec;

	// Our data set: 10 two-dimensional points
	double data_set[20] = { 0.088316633,  0.807443027,
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
	scc_DataSetObject* data_set_object;
	ec = scc_get_data_set_object(10, 2, 20, data_set, false, &data_set_object);
	if(ec != SCC_ER_OK) return 1; // Check error

	// Make empty clustering to populate
	scc_Clabel cluster_labels[10];
	scc_Clustering* cl;
	ec = scc_init_empty_clustering(10, cluster_labels, &cl);
	if(ec != SCC_ER_OK) return 1;

	// Derive a clustering where each cluster contains at least 3 points
	ec = scc_nng_clustering(cl, data_set_object, 3, SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED,
	                        false, 0.0, 0, NULL, SCC_UM_IGNORE, false, 0.0);
	if(ec != SCC_ER_OK) return 1;

	printf("Cluster assignment:\n");
	for(size_t i = 0; i < 10; ++i) {
		printf("%d ", cluster_labels[i]);
	}
	printf("\n");

	// Free cluster and data set objects
	scc_free_clustering(&cl);
	scc_free_data_set_object(&data_set_object);
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
