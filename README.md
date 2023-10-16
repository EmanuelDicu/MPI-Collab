# Collaborative Computers in Distributed Systems using MPI

## Overview

In this project, we explore the concept of collaborative computing in distributed systems using MPI (Message Passing Interface). The system consists of multiple clusters interconnected in an arbitrary graph. Each cluster is composed of a coordinator and multiple workers. The main objective of this project is to facilitate collaboration between clusters, ensuring an equitable distribution of workload among their respective workers. The workload is initially assigned to Coordinator 0, which then delegates it to other coordinators, who, in turn, distribute it among their workers. The results are subsequently gathered back at Coordinator 0, where they are displayed in the terminal.

## Code Structure

The project is organized into several key components:

1. **`mpi_collab.c`**: This file contains the implementations of two functions, one for coordinators and another for workers.

2. **`helper/`**: This directory includes various files with useful procedures that support different functionalities within the code. These files cover:
   - **`heartbeat.c`**: Implementation of the heartbeat algorithm for identifying the leader. The leader is considered to be the node with the lowest rank in the subgraph.
   - **`helper_procedures.c`**: Procedures for constructing the topology from the input and determining whether a node is a coordinator or a worker.
   - **`log_messages.c`**: Useful procedures for debugging and logging messages to the terminal.
   - **`mpi_wrappers.c`**: These files contain procedures that encapsulate MPI_Send and MPI_Recv procedures from MPI while also logging messages to the terminal.

3. **`array/`**: This directory contains various files that simplify operations with arrays used in the code. These files cover:
   - **`array_helpers.c`**: Procedures used for sending and receiving arrays of unspecified length in MPI.
   - **`array_procedures.c`**: Procedures used to send different vectors, such as parent nodes or the sizes of subtrees, between coordinators.

4. **`cluster/`**: Within this directory, there are files containing procedures to simplify operations with clusters, which are arrays of lists that store workers for specific coordinators. This section includes:
   - **`cluster_helpers.c`**: Multiple helper procedures, including ordering clusters, concatenating clusters, reading clusters from files, printing clusters, and freeing memory used by clusters.
   - **`cluster_procedures.c`**: Procedures for transmitting clusters between coordinators and reconstructing clusters from information provided by children.

5. **`workload/`**: This directory contains files with procedures that simplify operations with the array used for processing. These procedures include:
   - **`workload_helpers.c`**: Procedures to generate the array, print the array, and aid in debugging.
   - **`workload_procedures.c`**: Procedures for transmitting workload between coordinators and workers, as well as reconstructing workload from information received from children.

## Workflow / Algorithm

The project follows a well-defined workflow and algorithm:

1. Initially, each coordinator identifies its neighbors.

2. Through the heartbeat algorithm, each coordinator determines the master node within its subgraph. The master is defined as the lowest-ranking node, which is always assigned rank zero.

3. A spanning tree is determined within each subgraph. This is achieved by establishing a vector of parent nodes using the wave-echo algorithm, as explained in Laboratory 10. Each coordinator now possesses the vector of parents, providing insights into how coordinators are interconnected within their subgraph.

4. The next step involves the determination of the overall topology. Each coordinator reads the file assigned to its cluster (workers), notifies each worker that it is their coordinator, and receives clusters from their children in the subtrees. These clusters are then concatenated with the coordinator's own cluster. Once all clusters from the subtree are collected, the coordinator sends them to its parent. The root coordinator ultimately possesses the entire topology, which is sent recursively to all children. Each coordinator transmits the topology of its workers within the subtree.

5. The project further involves determining an array that records the size of all subtrees for each coordinator. This array represents the total number of workers for all coordinators within the subtree of the current node. The algorithm resembles the wave-echo algorithm used earlier, with the key difference being that only the sizes of subtrees of direct children are of interest for a given node, eliminating the need to propagate the complete array from the root to the leaves.

6. The second part of the project entails Node 0 generating an array as specified in the statement and sending it in pieces to the subtree. If a coordinator has been designated as the leader but is non-zero (typically Node 1, which is disconnected from the topology in the statement), it performs no action. Other coordinators receive a portion of the workload from their parent, which they must evenly distribute within the subtree. Each coordinator is responsible for a workload of a specific size.
