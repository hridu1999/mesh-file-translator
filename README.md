# Mesh File Translator (MeshFT)

A lightweight C program that reads a two-dimensional Gmsh mesh and
converts its node and boundary information into files for a
Cartesian-grid or PDE-solver workflow.

The translator preserves the original Gmsh node tags while sorting and
renumbering nodes. It uses the original tags to match line elements
to their endpoints, then writes connectivity using the new node numbers.

## Features

- Reads node coordinates and elements from Gmsh 4.1 ASCII mesh files.
- Sorts nodes by x-coordinate and then by y-coordinate within each
  column.
- Assigns new node numbers after sorting.
- Maps original Gmsh node tags to the new node numbers.
- Calculates west, east, north, and south neighbors using a KD-tree
  nearest-neighbor search.
- Reads first-order line elements to construct boundary connectivity.
- Traces separate closed boundary loops and writes their connectivity
  in counterclockwise order.
- Identifies boundary and interior nodes in the output.

## Input

The program reads a **Gmsh 4.1 ASCII `.msh` file** containing:

- A `$Nodes` section with node tags and coordinates.
- An `$Elements` section with first-order, two-node line elements
  defining the boundaries.

The current version uses the x- and y-coordinates of the mesh.

Set the mesh filename in `main()`:

```c
const char *mesh_filename = "circular3.msh";
```

Place the input file in the program's working directory, or specify
its path.

## Output Files

### `nodes0.dat`

Contains the total number of nodes followed by their new node numbers,
coordinates, and output labels.

```text
4
1      0.0000       0.0000       3
2      0.0000       1.0000       3
3      1.0000       0.0000       3
4      1.0000       1.0000       3
```

| Label | Meaning |
|------:|---------|
| `3` | Boundary node, including corner nodes |
| `1` | Interior node |

### `connectivity0.dat`

Contains directional neighbors for each node:

```text
node   w     e     n     s
1      0     3     2     0
2      0     4     0     1
3      1     0     4     0
4      2     0     0     3
```

A value of `0` means that no neighbor was selected in that direction.

Directional neighbors are selected from a KD-tree nearest-neighbor
search. They should be checked for suitability for the mesh and solver,
especially near boundaries or on irregular meshes.

### `bndry_connectivity0.dat`

Contains the previous and next boundary nodes for each boundary node:

```text
curr   prev   next
1      2      3
3      1      4
4      3      2
2      4      1
```

The `next` column follows each boundary loop counterclockwise in the
x-y plane. Separate closed loops are traced independently.

## How It Works

1. Read nodes and preserve their original Gmsh node tags.
2. Sort nodes and assign new node numbers.
3. Read line elements from the `$Elements` section.
4. Match each line element's endpoint tags to the sorted nodes.
5. Trace each closed boundary loop and determine its orientation.
6. Calculate directional neighbors using a KD-tree.
7. Write the output files using the new node numbers.

The original Gmsh tag stays with each node during sorting. This allows
the program to find the correct boundary connections even though the
output node numbers have changed.

## Build and Run

### Visual Studio Code

1. Open the project folder in Visual Studio Code.
2. Update the mesh filename in `main()`.
3. Build the C source files using your configured C compiler.
4. Run the compiled executable from the terminal.

The input mesh must be accessible from the program's working directory.
The output files are written to that directory.

## Current Limitations

- Input is limited to Gmsh 4.1 ASCII meshes.
- Boundary extraction supports first-order, two-node line elements.
- Line elements are expected to form separate, non-branching closed loops.
- All 1D line elements are treated as boundary edges; internal curves
  and branching interfaces require additional handling.
- Physical groups and boundary-condition labels are not yet exported.

## Future Improvements

- Read Gmsh physical groups to identify inlets, outlets, and walls.
- Export boundary edges with curve and physical-group tags.
- Support higher-order line elements and internal interfaces.
- Improve directional connectivity for irregular meshes.
- Add validation and visualization tools.
- Support additional mesh formats.

## Technologies

- C
- Visual Studio
- C standard library
