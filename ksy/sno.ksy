meta:
  id: sno
  file-extension: sno
  endian: le
seq:
  - id: magic
    contents: SNOD
  - id: version
    type: version
  - id: door_count
    type: u4
  - id: spot_count
    type: u4
  - id: vertex_count
    type: u4
  - id: triangle_count
    type: u4
  - id: texture_count
    type: u4
  - id: bounding_box
    type: bounding_box
  - id: centroid_offset
    type: v3
  - id: tile
    type: u4
  - id: reserved0
    type: u4
  - id: reserved1
    type: u4
  - id: reserved2
    type: u4
  - id: checksum
    type: u4
    if: (version.major > 6) or (version.major == 6 and version.minor >= 2)
  - id: door_array
    type: door
    repeat: expr
    repeat-expr: door_count
  - id: spot_array
    type: spot
    repeat: expr
    repeat-expr: spot_count
  - id: vertex_array
    type: vertex
    repeat: expr
    repeat-expr: vertex_count
  - id: surface_array
    type: surface
    repeat: expr
    repeat-expr: texture_count
  - id: logical_mesh_count
    type: u4
  - id: logical_mesh
    type: logical_mesh
    repeat: expr
    repeat-expr: logical_mesh_count
enums:
  floor:
     0x40000001: floor
     0x80000000: water
     0x20000000: ignored
types:
  v3:
    seq:
      - id: x
        type: f4
      - id: y
        type: f4
      - id: z
        type: f4
  bounding_box:
    seq:
      - id: min
        type: v3
      - id: max
        type: v3
  triangle:
    seq:
      - id: a
        type: v3
      - id: b
        type: v3
      - id: c
        type: v3
  color:
    seq:
      - id: r
        type: u1
      - id: g
        type: u1
      - id: b
        type: u1
      - id: a
        type: u1
  tcoords:
    seq:
      - id: u
        type: f4
      - id: v
        type: f4
  version:
    seq:
      - id: major
        type: u4
      - id: minor
        type: u4
  face:
    seq:
      - id: a
        type: u2
      - id: b
        type: u2
      - id: c
        type: u2
  door:
    seq:
      - id: id
        type: u4
      - id: center
        type: v3
      - id: x_axis
        type: v3
      - id: y_axis
        type: v3
      - id: z_axis
        type: v3
      - id: vertex_count
        type: u4
      - id: vertex_array
        type: u4
        repeat: expr
        repeat-expr: vertex_count
  spot:
    seq:
      - id: x_axis
        type: v3
      - id: y_axis
        type: v3
      - id: z_axis
        type: v3
      - id: center
        type: v3
      - id: label
        type: str
        encoding: ASCII
        terminator: 0x00
  vertex:
    seq:
      - id: position
        type: v3
      - id: normal
        type: v3
      - id: color
        type: color
      - id: uvcoords
        type: tcoords
  surface:
    seq:
      - id: texture
        type: str
        encoding: ASCII
        terminator: 0x00
      - id: start_corner
        type: u4
      - id: span_corner
        type: u4
      - id: vertex_count
        type: u4
      - id: face_array
        type: face
        repeat: expr
        repeat-expr: vertex_count / 3
  logical_mesh:
    seq:
      - id: index
        type: u1
      - id: bounding_box
        type: bounding_box
      - id: floor
        type: u4
        enum: floor
      - id: num_connections
        type: u4
      - id: connection_section_array_6_2
        type: connection_section_6_2
        if: _root.version.major == 6 and _root.version.minor >= 4
        repeat: expr
        repeat-expr: num_connections
      - id: connection_section_array_7
        type: connection_section_7
        if: _root.version.major == 7
        repeat: expr
        repeat-expr: num_connections
      - id: num_nodal_connections
        type: u4
      - id: nodal_array
        type: nodal_section
        repeat: expr
        repeat-expr: num_nodal_connections
      - id: triangle_section_count
        type: u4
      - id: triangle_section
        type: triangle_section
        repeat: expr
        repeat-expr: triangle_section_count
      - id: bsp_tree
        type: bsp_section
  connection_section_6_2:
    seq:
      - id: index
        type: u2
      - id: bounding_box
        type: bounding_box
      - id: triangle_count
        type: u2
      - id: triangle_array
        type: u2
        repeat: expr
        repeat-expr: triangle_count
      - id: local_connection_count
        type: u4
      - id: local_connection_array
        type: u2
        repeat: expr
        repeat-expr: local_connection_count
  connection_section_7:
    seq:
      - id: index
        type: u2
      - id: bounding_box
        type: bounding_box
      - id: center
        type: v3
      - id: triangle_count
        type: u2
      - id: trangle_array
        type: u2
        repeat: expr
        repeat-expr: triangle_count
      - id: local_connection_count
        type: u4
      - id: local_connection_array
        type: u2
        repeat: expr
        repeat-expr: local_connection_count
  nodal_section:
    seq:
      - id: far_id
        type: u1
      - id: nodal_leaf_connection_count
        type: u4
      - id: data
        type: u2
        repeat: expr
        repeat-expr: nodal_leaf_connection_count * 2
  triangle_section:
    seq:
      - id: triangle
        type: triangle
      - id: normal
        type: v3
  bsp_section:
    seq:
      - id: bounding_box
        type: bounding_box
      - id: is_leaf
        type: u1
      - id: triangle_count
        type: u2
      - id: triangle_data
        type: u2
        repeat: expr
        repeat-expr: triangle_count
      - id: children
        type: u1
      - id: bsp_child
        type: bsp_section
        repeat: expr
        repeat-expr: children