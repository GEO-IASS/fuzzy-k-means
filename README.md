# fuzzy-k-means
C++ Implementation of Fuzzy k-means clustering algorithm 

## Overview

The program read an adjacency matrix of a graph and clustering it to calculate the pertinence of the ponts to each cluster.

This program generates two groups of files to be imported to Gephi software:
* Edges: Text file with edges based on adjacency matrix of graph
* Nodes: Text file with nodes. Will be generated one file for each centroid, each node has its pertinence for the centroid.

## Compile the program

 1. Create build subdirectory from main directory of the project: `mkdir build`
 2. Go to the new directory: `cd build`
 3. Create make file: `cmake ..`
 4. Run make to compile: `make`

## Command line

To run the program: `fkm <options>`

### Options

 Option       | Description
 -------------|------------
 `-h`         | show instructions
 `-k <value>` | number of clusters or centroids
 `-m <value>` | fuzzifier (must be greater than 1)
 `-d <value>` | dimension of adjacency matrix
 `-i <value>` | max number of iterations
 `-if <file>` | input file with sequencial values of adjacency matrix
 `-nf <file>` | node suffix file. Will be generated one file for each cluster
 `-ef <file>` | edge file
