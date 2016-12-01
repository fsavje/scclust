# scclust

[![Build Status](https://travis-ci.org/fsavje/scclust.svg?branch=master)](https://travis-ci.org/fsavje/scclust)

**scclust** is used for **s**ize **c**onstrained **clust**ering. It solves clustering problems where one wants to partition of a set of data points in to groups subject to constraints on the composition of the groups. In particular, scclust constructs a clustering that satisfies the constraints while trying to minimize the dissimilarity of points assigned to the same clusters. It is possible to specify overall size constraints and constraints specific to types of data points. For example, one can require that each cluster should contain at least five data points in total and at least two "red" data points.

scclust is made with large data sets in mind, and it can cluster millions of data points in less than a minute. It's also fairly easy to adapt scclust to run with exotic databases or other distance metrics as all data management can be accessed and changed at runtime.

scclust is written in C99 and compiles only with the standard C library.

*The library is currently in alpha. Breaking changes to the API might happen.*


## How to compile scclust

The following code downloads and compiles the current development version of scclust:

```bash
wget https://github.com/fsavje/scclust/archive/master.zip
unzip master.zip
cd scclust-master
./configure
make
```

scclust compiles into a static library.


## How to use scclust

Details on how to use scclust is best found in the documentation, the library header (`include/scclust.h`) and the examples distributed with the code (`examples`). For those that are impatient, the following snippet shows a simple use case: we have a set of data points and want to construct clusters that contain at least three points each.

```c
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <scclust.h>

int main(void) {

	// Data: 10 two-dimensional points
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

	// Error code variable
	scc_ErrorCode ec;

	// Construct scclust data set object
	scc_DataSet* data_set;
	ec = scc_init_data_set(10,           // # data points
	                       2,            // # dimensions
	                       20,           // Length of data matrix
	                       raw_data,     // Data matrix
	                       &data_set);   // Data set to initialize
	// Check error code
	if(ec != SCC_ER_OK) return 1;

	// Make empty clustering object
	scc_Clabel cluster_labels[10];
	scc_Clustering* clustering;
	ec = scc_init_empty_clustering(10,               // Data points
	                               cluster_labels,   // Clustering labels
	                               &clustering);     // Clustering to initialize
	if(ec != SCC_ER_OK) return 1;

	// Set clustering options (start with defaults)
	scc_ClusterOptions options = scc_default_cluster_options;
	// At least 3 data points in each cluster
	options.size_constraint = 3;

	// Make clustering
	ec = scc_make_clustering(data_set, clustering, &options);
	if(ec != SCC_ER_OK) return 1;

	// Print clustering
	printf("Cluster assignment:\n");
	for(size_t i = 0; i < 10; ++i) {
		printf("%d ", cluster_labels[i]);
	}
	printf("\n");

	// Free clustering and data set objects
	scc_free_clustering(&clustering);
	scc_free_data_set(&data_set);
}
```

This is the output the program generates:

```
Cluster assignment:
0 2 1 0 2 1 2 0 0 1
```

That is, the first point is assigned to cluster "0", the second to cluster "2" and so on.

The example is distributed with the library. After compiling scclust itself, you can compile and run it by calling the following in the `examples/simple` folder.

```bash
make
./simple_example.out
```

## Compilation options

scclust accepts several compilation options as flags to the `configure` script:

```
Usage: ./configure [OPTIONS]...

  -h, --help                display this help and exit
  --version                 display version information and exit
  -q, --quiet               do not print messages

  --enable-FEATURE          include FEATURE
  --disable-FEATURE         do not include FEATURE

  --enable-assert           enable ASSERT checking [default=off]
  --enable-digraph-debug    enable debug functions for digraphs [default=off]
  --enable-cmocka-headers   use cmocka allocation functions [default=off]
  --enable-documentation    make documentation [default=off]
  --enable-all-docs         make documentation for internal methods [default=off]

  --with-clabel=[ARG]       cluster label type [default=uint32_t]
  --with-clabel-na=[ARG]    cluster label NA value [default=default]
  --with-typelabel=[ARG]    type label type [default=uint_fast16_t]
  --with-pointindex=[ARG]   data point ID type [default=uint32_t]
  --with-arcindex=[ARG]     digraph arc type [default=uint32_t]
```


### `--[enable/disable]-assert`

Default: `--disable-assert`

Compiles with debug asserts. This will slow down execution, but is helpful to catch bugs.


### `--[enable/disable]-digraph-debug`

Default: `--disable-digraph-debug`

Compiles with debug support for the internal digraph object. This is only useful when debugging.


### `--[enable/disable]-cmocka-headers`

Default: `--disable-cmocka-headers`

Compiles with support for [cmocka](https://cmocka.org)'s internal allocation and assert functions which watches for overflows and memory leaks. This is only useful when testing during development and is not recommended for production use.

Requires the [cmocka](https://cmocka.org) library.


### `--[enable/disable]-documentation`

Default: `--disable-documentation`

Compiles documentation.

Requires the [doxygen](http://www.stack.nl/~dimitri/doxygen/) library.


### `--[enable/disable]-all-docs`

Default: `--disable-all-docs`

By default, scclust only makes documentation for the public header methods. With this option, one can make documentation for internal methods as well. This can be useful during development.


### `--with-clabel=[ARG]`

Allowed values: `uint32_t uint64_t int`

Default: `uint32_t`

Change the data type that stores cluster labels. scclust uses this type to report cluster assignment.


### `--with-clabel-na=[ARG]`

Allowed values: `max -1 -2 -3 ...`

Default: `max`

Change the value used to denote unassigned data points.

| `--with-clabel-na=`  | Value                                         |
| -------------------- | --------------------------------------------- |
| `max`                | Use maximum value storable in `--with-clabel` |
| `-1 -2 -3 ...`       | Use [ARG], `--with-clabel` must be signed     |


### `--with-typelabel=[ARG]`

Allowed values: `uint_fast16_t  int`

Default: `uint_fast16_t`

Change the data type that stores data point type labels. This type is used to denote which type data points are.


### `--with-pointindex=[ARG]`

Allowed values: `uint32_t  uint64_t  int`

Default: `uint32_t`

Change the data type that stores point indices. This choice restricts the maximum number of points in any clustering problem solved by scclust. A wider type allows for larger problems but requires more memory. The maximum number of data points is given by:

| `--with-pointindex=`  | Max data points |
| --------------------- | --------------  |
| `uint32_t`            | 2^32 − 1        |
| `uint64_t`            | 2^64 − 1        |
| `int`                 | 2^31 − 1        |


### `--with-arcindex=[ARG]`

Allowed values: `uint32_t  uint64_t`

Default: `uint32_t`

Change the data type that stores arc indices. This choice restricts the size of the graphs used by scclust to solve clustering problems. A wider type allows for larger problems but requires more memory. A rough estimate of the number of the maximum graph size is given by $\{\text{number of points}\} \times \{\text{minimum size of clusters}\}$. The maximum number of arcs are given by:

| `--with-arcindex=`  | Max arcs  |
| ------------------- | --------- |
| `uint32_t`          | 2^32 − 1  |
| `uint64_t`          | 2^64 − 1  |


## Service Provider Interface (SPI)

scclust is based on functions that access the data points and derive distances between them. It is possible to change these functions at runtime. This can be useful when extending scclust to accept other databases or if one wants to use particular functions to calculate the distances. In particular, scclust ships with a simple nearest neighbor search algorithm; performance can often be improved drastically by using a dedicated nearest neighbor search library.

See `include/scclust_spi.h` and `src/dist_search.h` for the distance functions that can be exchanged. Note that if the new functions accepts the `scc_DataSet` struct as input (see `src/data_set_struct.h`), one can swap only parts of the distance functions.

See `examples/ann/` for an example where the [ANN library](https://www.cs.umd.edu/~mount/ANN/) is used for nearest neighbor searching. (It is recommended to compile scclust with the `--with-pointindex=int` option when using the ANN wrapper. This avoids costly type translations between the libraries.)


## How to contribute

Thank you for considering contributing to scclust!

There are many ways to help out: report bugs, suggest new features and submitting code that implements enhancements and bug fixes. If possible, use Github's internal tools for to do so: [issues](https://github.com/fsavje/scclust/issues) for bug reports and suggestions, and [pull requests](https://github.com/fsavje/scclust/pulls) for code. If you're new to Github, read this [guide](https://guides.github.com/activities/contributing-to-open-source/) to learn more.

If you're filing a bug, please include all information needed to reproduce it. Besides the code you're trying to run, your platform, toolchain and compilation options is often useful information.


## License

scclust is distributed under the [GNU Lesser General Public License v2.1](https://github.com/fsavje/scclust/blob/master/LICENSE).
