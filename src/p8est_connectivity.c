/*
  This file is part of p4est.
  p4est is a C library to manage a parallel collection of quadtrees and/or
  octrees.

  Copyright (C) 2008,2009 Carsten Burstedde, Lucas Wilcox.

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <p8est_connectivity.h>
#include <sc_io.h>

/* *INDENT-OFF* */
const int           p8est_face_corners[6][4] =
{{ 0, 2, 4, 6 },
 { 1, 3, 5, 7 },
 { 0, 1, 4, 5 },
 { 2, 3, 6, 7 },
 { 0, 1, 2, 3 },
 { 4, 5, 6, 7 }};
const int           p8est_face_edges[6][4] =
{{ 4, 6,  8, 10 },
 { 5, 7,  9, 11 },
 { 0, 2,  8,  9 },
 { 1, 3, 10, 11 },
 { 0, 1,  4,  5 },
 { 2, 3,  6,  7 }};
const int           p8est_face_dual[6] = { 1, 0, 3, 2, 5, 4 };
const int           p8est_face_child_hang[6][8] =
{{  0, -1,  1, -1,  2, -1,  3, -1 },
 { -1,  0, -1,  1, -1,  2, -1,  3 },
 {  0,  1, -1, -1,  2,  3, -1, -1 },
 { -1, -1,  0,  1, -1, -1,  2,  3 },
 {  0,  1,  2,  3, -1, -1, -1, -1 },
 { -1, -1, -1, -1,  0,  1,  2,  3 }};
const int           p8est_face_permutations[8][4] =
{{ 0, 1, 2, 3 },                /* no.  0 of 0..23 */
 { 0, 2, 1, 3 },                /* no.  2 of 0..23 */
 { 1, 0, 3, 2 },                /* no.  7 of 0..23 */
 { 1, 3, 0, 2 },                /* no. 10 of 0..23 */
 { 2, 0, 3, 1 },                /* no. 13 of 0..23 */
 { 2, 3, 0, 1 },                /* no. 16 of 0..23 */
 { 3, 1, 2, 0 },                /* no. 21 of 0..23 */
 { 3, 2, 1, 0 }};               /* no. 23 of 0..23 */
const int           p8est_face_permutation_sets[3][4] =
{{ 1, 2, 5, 6 },
 { 0, 3, 4, 7 },
 { 0, 4, 3, 7 }};
const int           p8est_face_permutation_refs[6][6] =
{{ 0, 1, 1, 0, 0, 1 },
 { 2, 0, 0, 1, 1, 0 },
 { 2, 0, 0, 1, 1, 0 },
 { 0, 2, 2, 0, 0, 1 },
 { 0, 2, 2, 0, 0, 1 },
 { 2, 0, 0, 2, 2, 0 }};

const int           p8est_edge_faces[12][2] =
{{ 2, 4 },
 { 3, 4 },
 { 2, 5 },
 { 3, 5 },
 { 0, 4 },
 { 1, 4 },
 { 0, 5 },
 { 1, 5 },
 { 0, 2 },
 { 1, 2 },
 { 0, 3 },
 { 1, 3 }};
const int           p8est_edge_corners[12][2] =
{{ 0, 1 },
 { 2, 3 },
 { 4, 5 },
 { 6, 7 },
 { 0, 2 },
 { 1, 3 },
 { 4, 6 },
 { 5, 7 },
 { 0, 4 },
 { 1, 5 },
 { 2, 6 },
 { 3, 7 }};
const int           p8est_edge_face_corners[12][6][2] =
{{{ -1, -1 }, { -1, -1 }, {  0,  1 }, { -1, -1 }, {  0,  1 }, { -1, -1 }},
 {{ -1, -1 }, { -1, -1 }, { -1, -1 }, {  0,  1 }, {  2,  3 }, { -1, -1 }},
 {{ -1, -1 }, { -1, -1 }, {  2,  3 }, { -1, -1 }, { -1, -1 }, {  0,  1 }},
 {{ -1, -1 }, { -1, -1 }, { -1, -1 }, {  2,  3 }, { -1, -1 }, {  2,  3 }},
 {{  0,  1 }, { -1, -1 }, { -1, -1 }, { -1, -1 }, {  0,  2 }, { -1, -1 }},
 {{ -1, -1 }, {  0,  1 }, { -1, -1 }, { -1, -1 }, {  1,  3 }, { -1, -1 }},
 {{  2,  3 }, { -1, -1 }, { -1, -1 }, { -1, -1 }, { -1, -1 }, {  0,  2 }},
 {{ -1, -1 }, {  2,  3 }, { -1, -1 }, { -1, -1 }, { -1, -1 }, {  1,  3 }},
 {{  0,  2 }, { -1, -1 }, {  0,  2 }, { -1, -1 }, { -1, -1 }, { -1, -1 }},
 {{ -1, -1 }, {  0,  2 }, {  1,  3 }, { -1, -1 }, { -1, -1 }, { -1, -1 }},
 {{  1,  3 }, { -1, -1 }, { -1, -1 }, {  0,  2 }, { -1, -1 }, { -1, -1 }},
 {{ -1, -1 }, {  1,  3 }, { -1, -1 }, {  1,  3 }, { -1, -1 }, { -1, -1 }}};

const int           p8est_corner_faces[8][3] =
{{ 0, 2, 4 },
 { 1, 2, 4 },
 { 0, 3, 4 },
 { 1, 3, 4 },
 { 0, 2, 5 },
 { 1, 2, 5 },
 { 0, 3, 5 },
 { 1, 3, 5 }};
const int           p8est_corner_edges[8][3] =
{{ 0, 4,  8 },
 { 0, 5,  9 },
 { 1, 4, 10 },
 { 1, 5, 11 },
 { 2, 6,  8 },
 { 2, 7,  9 },
 { 3, 6, 10 },
 { 3, 7, 11 }};
const int           p8est_corner_face_corners[8][6] =
{{  0, -1,  0, -1,  0, -1 },
 { -1,  0,  1, -1,  1, -1 },
 {  1, -1, -1,  0,  2, -1 },
 { -1,  1, -1,  1,  3, -1 },
 {  2, -1,  2, -1, -1,  0 },
 { -1,  2,  3, -1, -1,  1 },
 {  3, -1, -1,  2, -1,  2 },
 { -1,  3, -1,  3, -1,  3 }};

const int           p8est_child_edge_faces[8][12] =
{{ -1,  4,  2, -1, -1,  4,  0, -1, -1,  2,  0, -1 },
 { -1,  4,  2, -1,  4, -1, -1,  1,  2, -1, -1,  1 },
 {  4, -1, -1,  3, -1,  4,  0, -1,  0, -1, -1,  3 },
 {  4, -1, -1,  3,  4, -1, -1,  1, -1,  1,  3, -1 },
 {  2, -1, -1,  5,  0, -1, -1,  5, -1,  2,  0, -1 },
 {  2, -1, -1,  5, -1,  1,  5, -1,  2, -1, -1,  1 },
 { -1,  3,  5, -1,  0, -1, -1,  5,  0, -1, -1,  3 },
 { -1,  3,  5, -1, -1,  1,  5, -1, -1,  1,  3, -1 }};
const int           p8est_child_corner_faces[8][8] =
{{ -1, -1, -1,  4, -1,  2,  0, -1 },
 { -1, -1,  4, -1,  2, -1, -1,  1 },
 { -1,  4, -1, -1,  0, -1, -1,  3 },
 {  4, -1, -1, -1, -1,  1,  3, -1 },
 { -1,  2,  0, -1, -1, -1, -1,  5 },
 {  2, -1, -1,  1, -1, -1,  5, -1 },
 {  0, -1, -1,  3, -1,  5, -1, -1 },
 { -1,  1,  3, -1,  5, -1, -1, -1 }};
const int           p8est_child_corner_edges[8][8] =
{{ -1,  0,  4, -1,  8, -1, -1, -1 },
 {  0, -1, -1,  5, -1,  9, -1, -1 },
 {  4, -1, -1,  1, -1, -1, 10, -1 },
 { -1,  5,  1, -1, -1, -1, -1, 11 },
 {  8, -1, -1, -1, -1,  2,  6, -1 },
 { -1,  9, -1, -1,  2, -1, -1,  7 },
 { -1, -1, 10, -1,  6, -1, -1,  3 },
 { -1, -1, -1, 11, -1,  7,  3, -1 }};
/* *INDENT-ON* */

static p8est_connectivity_t *
p8est_connectivity_new_copy (p4est_topidx_t num_vertices,
                             p4est_topidx_t num_trees,
                             p4est_topidx_t num_edges,
                             p4est_topidx_t num_corners,
                             const double *vertices,
                             const p4est_topidx_t * ttv,
                             const p4est_topidx_t * ttt,
                             const int8_t * ttf,
                             const p4est_topidx_t * tte,
                             const p4est_topidx_t * eoff,
                             const p4est_topidx_t * ett,
                             const int8_t * ete,
                             const p4est_topidx_t * ttc,
                             const p4est_topidx_t * coff,
                             const p4est_topidx_t * ctt, const int8_t * ctc)
{
  p4est_topidx_t      num_ett;
  p4est_topidx_t      num_ctt;
  p8est_connectivity_t *conn;

  num_ett = eoff[num_edges];
  num_ctt = coff[num_corners];
  conn = p8est_connectivity_new (num_vertices, num_trees,
                                 num_edges, num_ett, num_corners, num_ctt);

  if (num_vertices > 0) {
    P4EST_ASSERT (vertices != NULL && ttv != NULL);
    memcpy (conn->vertices, vertices, sizeof (double) * 3 * num_vertices);
    memcpy (conn->tree_to_vertex, ttv,
            sizeof (p4est_topidx_t) * 8 * num_trees);
  }
  else {
    conn->vertices = NULL;
    conn->tree_to_vertex = NULL;
  }
  memcpy (conn->tree_to_tree, ttt, sizeof (p4est_topidx_t) * 6 * num_trees);
  memcpy (conn->tree_to_face, ttf, sizeof (int8_t) * 6 * num_trees);

  if (num_edges > 0) {
    memcpy (conn->tree_to_edge, tte,
            sizeof (p4est_topidx_t) * 12 * num_trees);
    memcpy (conn->edge_to_tree, ett, sizeof (p4est_topidx_t) * num_ett);
    memcpy (conn->edge_to_edge, ete, sizeof (int8_t) * num_ett);
  }
  memcpy (conn->ett_offset, eoff, sizeof (p4est_topidx_t) * (num_edges + 1));

  if (num_corners > 0) {
    memcpy (conn->tree_to_corner, ttc,
            sizeof (p4est_topidx_t) * 8 * num_trees);
    memcpy (conn->corner_to_tree, ctt, sizeof (p4est_topidx_t) * num_ctt);
    memcpy (conn->corner_to_corner, ctc, sizeof (int8_t) * num_ctt);
  }
  memcpy (conn->ctt_offset, coff,
          sizeof (p4est_topidx_t) * (num_corners + 1));

  P4EST_ASSERT (p8est_connectivity_is_valid (conn));

  return conn;
}

p8est_connectivity_t *
p8est_connectivity_new (p4est_topidx_t num_vertices,
                        p4est_topidx_t num_trees,
                        p4est_topidx_t num_edges,
                        p4est_topidx_t num_ett,
                        p4est_topidx_t num_corners, p4est_topidx_t num_ctt)
{
  p8est_connectivity_t *conn;

  conn = P4EST_ALLOC_ZERO (p8est_connectivity_t, 1);

  conn->num_vertices = num_vertices;
  conn->num_trees = num_trees;
  if (num_vertices > 0) {
    conn->vertices = P4EST_ALLOC (double, 3 * num_vertices);
    conn->tree_to_vertex = P4EST_ALLOC (p4est_topidx_t, 8 * num_trees);
  }
  else {
    conn->vertices = NULL;
    conn->tree_to_vertex = NULL;
  }
  conn->tree_to_tree = P4EST_ALLOC (p4est_topidx_t, 6 * num_trees);
  conn->tree_to_face = P4EST_ALLOC (int8_t, 6 * num_trees);

  conn->num_edges = num_edges;
  if (num_edges > 0) {
    conn->tree_to_edge = P4EST_ALLOC (p4est_topidx_t, 12 * num_trees);
    conn->edge_to_tree = P4EST_ALLOC (p4est_topidx_t, num_ett);
    conn->edge_to_edge = P4EST_ALLOC (int8_t, num_ett);
  }
  else {
    conn->tree_to_edge = NULL;
    conn->edge_to_tree = NULL;
    conn->edge_to_edge = NULL;
  }
  conn->ett_offset = P4EST_ALLOC (p4est_topidx_t, num_edges + 1);
  conn->ett_offset[num_edges] = num_ett;

  conn->num_corners = num_corners;
  if (num_corners > 0) {
    conn->tree_to_corner = P4EST_ALLOC (p4est_topidx_t, 8 * num_trees);
    conn->corner_to_tree = P4EST_ALLOC (p4est_topidx_t, num_ctt);
    conn->corner_to_corner = P4EST_ALLOC (int8_t, num_ctt);
  }
  else {
    conn->tree_to_corner = NULL;
    conn->corner_to_tree = NULL;
    conn->corner_to_corner = NULL;
  }
  conn->ctt_offset = P4EST_ALLOC (p4est_topidx_t, num_corners + 1);
  conn->ctt_offset[num_corners] = num_ctt;

  return conn;
}

void
p8est_connectivity_destroy (p8est_connectivity_t * conn)
{
  P4EST_FREE (conn->vertices);
  P4EST_FREE (conn->tree_to_vertex);

  P4EST_FREE (conn->tree_to_tree);
  P4EST_FREE (conn->tree_to_face);

  P4EST_FREE (conn->tree_to_edge);
  P4EST_FREE (conn->ett_offset);
  P4EST_FREE (conn->edge_to_tree);
  P4EST_FREE (conn->edge_to_edge);

  P4EST_FREE (conn->tree_to_corner);
  P4EST_FREE (conn->ctt_offset);
  P4EST_FREE (conn->corner_to_tree);
  P4EST_FREE (conn->corner_to_corner);

  P4EST_FREE (conn);
}

bool
p8est_connectivity_is_valid (p8est_connectivity_t * conn)
{
  int                 nvert;
  int                 face, rface, nface, orientation;
  int                 edge, nedge, corner, ncorner;
  int                 flip, nflip, nflip1, nflip2;
  int                 ecode, ecount;
  bool                good, cfound[4];
  p4est_topidx_t      ntree1, ntree2, ntree3;
  p4est_topidx_t      vertex, tree, ntree;
  p4est_topidx_t      aedge, edge_begin, edge_end;
  p4est_topidx_t      acorner, corner_begin, corner_end;
  p4est_topidx_t      nett, nctt;
  const p4est_topidx_t num_vertices = conn->num_vertices;
  const p4est_topidx_t num_trees = conn->num_trees;
  const p4est_topidx_t num_edges = conn->num_edges;
  const p4est_topidx_t num_corners = conn->num_corners;
  const p4est_topidx_t *ttv = conn->tree_to_vertex;
  const p4est_topidx_t *ttt = conn->tree_to_tree;
  const int8_t       *ttf = conn->tree_to_face;
  const p4est_topidx_t *tte = conn->tree_to_edge;
  const p4est_topidx_t *eoff = conn->ett_offset;
  const p4est_topidx_t *ett = conn->edge_to_tree;
  const int8_t       *ete = conn->edge_to_edge;
  const p4est_topidx_t num_ett = eoff[num_edges];
  const p4est_topidx_t *ttc = conn->tree_to_corner;
  const p4est_topidx_t *coff = conn->ctt_offset;
  const p4est_topidx_t *ctt = conn->corner_to_tree;
  const int8_t       *ctc = conn->corner_to_corner;
  const p4est_topidx_t num_ctt = coff[num_corners];
  p8est_edge_info_t   ei;
  p8est_corner_info_t ci;
  sc_array_t         *eta = &ei.edge_transforms;
  sc_array_t         *cta = &ci.corner_transforms;

  good = false;
  sc_array_init (eta, sizeof (p8est_edge_transform_t));
  sc_array_init (cta, sizeof (p8est_corner_transform_t));

  if (num_vertices == 0 && (conn->vertices != NULL || ttv != NULL)) {
    P4EST_NOTICE ("Zero vertices still with arrays\n");
    goto failure;
  }
  if (num_vertices > 0 && (conn->vertices == NULL || ttv == NULL)) {
    P4EST_NOTICE ("Nonzero vertices missing arrays\n");
    goto failure;
  }

  for (nett = 0; nett < num_ett; ++nett) {
    if (ett[nett] < 0 || ett[nett] >= num_trees) {
      P4EST_NOTICEF ("Edge to tree %lld out of range\n", (long long) nett);
      goto failure;
    }
    if (ete[nett] < 0 || ete[nett] >= 24) {
      P4EST_NOTICEF ("Edge to edge %lld out of range\n", (long long) nett);
      goto failure;
    }
  }

  for (nctt = 0; nctt < num_ctt; ++nctt) {
    if (ctt[nctt] < 0 || ctt[nctt] >= num_trees) {
      P4EST_NOTICEF ("Corner to tree %lld out of range\n", (long long) nctt);
      goto failure;
    }
    if (ctc[nctt] < 0 || ctc[nctt] >= 8) {
      P4EST_NOTICEF ("Corner to corner %lld out of range\n",
                     (long long) nctt);
      goto failure;
    }
  }

  if (num_vertices > 0) {
    for (tree = 0; tree < num_trees; ++tree) {
      for (nvert = 0; nvert < 8; ++nvert) {
        vertex = ttv[tree * 8 + nvert];
        if (vertex < 0 || vertex >= num_vertices) {
          P4EST_NOTICEF ("Tree to vertex out of range %lld %d",
                         (long long) tree, nvert);
          goto failure;
        }
      }
    }
  }

  for (tree = 0; tree < num_trees; ++tree) {
    for (face = 0; face < 6; ++face) {
      ntree = ttt[tree * 6 + face];
      if (ntree < 0 || ntree >= num_trees) {
        P4EST_NOTICEF ("Tree to tree out of range %lld %d\n",
                       (long long) tree, face);
        goto failure;
      }
      rface = (int) ttf[tree * 6 + face];
      if (rface < 0 || rface >= 24) {
        P4EST_NOTICEF ("Tree to face out of range %lld %d\n",
                       (long long) tree, face);
        goto failure;
      }
      nface = rface % 6;        /* clamp to a real face index */
      orientation = rface / 6;  /* 0..3 for relative rotation */
      if (ntree == tree) {
        /* no neighbor across this face or self-periodic */
        if (nface == face && orientation != 0) {
          P4EST_NOTICEF ("Face invalid in %lld %d\n", (long long) tree, face);
          goto failure;
        }
      }
      if (ntree != tree || nface != face) {
        /* check reciprocity */
        if (ttt[ntree * 6 + nface] != tree) {
          P4EST_NOTICEF ("Tree to tree reciprocity in %lld %d\n",
                         (long long) tree, face);
          goto failure;
        }
        if ((int) ttf[ntree * 6 + nface] != face + 6 * orientation) {
          P4EST_NOTICEF ("Tree to face reciprocity in %lld %d\n",
                         (long long) tree, face);
          goto failure;
        }
      }
    }

    if (num_edges > 0) {
      for (edge = 0; edge < 12; ++edge) {
        p8est_find_edge_transform (conn, tree, edge, &ei);
        aedge = tte[tree * 12 + edge];
        if (aedge < -1 || aedge >= num_edges) {
          P4EST_NOTICEF ("Tree to edge out of range %lld %d\n",
                         (long long) tree, edge);
          goto failure;
        }
        if (aedge == -1) {
          continue;
        }
        ecode = ecount = 0;
        flip = nflip1 = nflip2 = -1;
        ntree1 = ttt[6 * tree + p8est_edge_faces[edge][0]];
        ntree2 = ttt[6 * tree + p8est_edge_faces[edge][1]];
        edge_begin = eoff[aedge];
        edge_end = eoff[aedge + 1];
        if (edge_begin < 0 || edge_begin >= num_ett ||
            edge_end < 0 || edge_end > num_ett) {
          P4EST_NOTICEF ("Invalid edge range %lld %d\n",
                         (long long) tree, edge);
          goto failure;
        }
        for (nett = edge_begin; nett < edge_end; ++nett) {
          ntree = ett[nett];
          nedge = (int) ete[nett] % 12;
          if (tte[ntree * 12 + nedge] != aedge) {
            P4EST_NOTICEF ("Edge to edge reciprocity in %lld %d %lld\n",
                           (long long) tree, edge, (long long) nett);
            goto failure;
          }
          nflip = (int) ete[nett] / 12;
          if (ntree == tree && nedge == edge) {
            if (flip != -1) {
              ecode = 1;
              break;
            }
            flip = nflip;
            continue;
          }
          ++ecount;
        }
        if (ecode > 0) {
          P4EST_NOTICEF ("Shared edge %lld %d %lld inconsistency %d\n",
                         (long long) tree, edge, (long long) nett, ecode);
          goto failure;
        }
        if (flip == -1 ||
            !(nflip1 == -1 || nflip1 == flip) ||
            !(nflip2 == -1 || nflip2 == flip)) {
          P4EST_NOTICEF ("Shared edge %lld %d inconsistent flip\n",
                         (long long) tree, edge);
          goto failure;
        }
      }
    }

    if (num_corners > 0) {
      for (corner = 0; corner < 8; ++corner) {
        p8est_find_corner_transform (conn, tree, corner, &ci);
        acorner = ttc[tree * 8 + corner];
        if (acorner < -1 || acorner >= num_corners) {
          P4EST_NOTICEF ("Tree to corner out of range %lld %d\n",
                         (long long) tree, corner);
          goto failure;
        }
        if (acorner == -1) {
          continue;
        }
        ecode = ecount = 0;
        cfound[0] = cfound[1] = cfound[2] = cfound[3] = false;
        ntree1 = ttt[6 * tree + p8est_corner_faces[corner][0]];
        ntree2 = ttt[6 * tree + p8est_corner_faces[corner][1]];
        ntree3 = ttt[6 * tree + p8est_corner_faces[corner][2]];
        corner_begin = coff[acorner];
        corner_end = coff[acorner + 1];
        if (corner_begin < 0 || corner_begin >= num_ctt ||
            corner_end < 0 || corner_end > num_ctt) {
          P4EST_NOTICEF ("Invalid corner range %lld %d\n",
                         (long long) tree, corner);
          goto failure;
        }
        for (nctt = corner_begin; nctt < corner_end; ++nctt) {
          ntree = ctt[nctt];
          ncorner = (int) ctc[nctt];
          if (ttc[ntree * 8 + ncorner] != acorner) {
            P4EST_NOTICEF ("Corner to corner reciprocity in %lld %d %lld\n",
                           (long long) tree, corner, (long long) nctt);
            goto failure;
          }
          if (ntree == tree && ncorner == corner) {
            if (cfound[0]) {
              ecode = 1;
              break;
            }
            cfound[0] = true;
            continue;
          }
          ++ecount;
        }
        if (ecode > 0) {
          P4EST_NOTICEF ("Shared corner %lld %d %lld inconsistency %d\n",
                         (long long) tree, corner, (long long) nctt, ecode);
          goto failure;
        }
        if (!cfound[0]) {       /* cfound[0]..[2] have been removed */
          P4EST_NOTICEF ("Shared corner %lld %d inconsistent count B\n",
                         (long long) tree, corner);
          goto failure;
        }
      }
    }
  }
  good = true;

failure:
  sc_array_reset (eta);
  sc_array_reset (cta);

  return good;
}

bool
p8est_connectivity_is_equal (p8est_connectivity_t * conn1,
                             p8est_connectivity_t * conn2)
{
  size_t              topsize, int8size;
  size_t              tcount;
  p4est_topidx_t      num_vertices, num_edges, num_ett, num_corners, num_ctt;

  topsize = sizeof (p4est_topidx_t);
  int8size = sizeof (int8_t);

  if (conn1->num_vertices != conn2->num_vertices ||
      conn1->num_trees != conn2->num_trees ||
      conn1->num_edges != conn2->num_edges ||
      conn1->num_corners != conn2->num_corners)
    return false;

  num_vertices = conn1->num_vertices;
  if (num_vertices > 0) {
    P4EST_ASSERT (conn1->vertices != NULL && conn2->vertices != NULL);
    if (memcmp (conn1->vertices, conn2->vertices,
                sizeof (double) * 3 * num_vertices))
      return false;

    P4EST_ASSERT (conn1->tree_to_vertex != NULL &&
                  conn2->tree_to_vertex != NULL);
    tcount = (size_t) (8 * conn1->num_trees);
    if (memcmp (conn1->tree_to_vertex, conn2->tree_to_vertex,
                tcount * topsize))
      return false;
  }

  tcount = (size_t) (12 * conn1->num_trees);
  if (memcmp (conn1->tree_to_edge, conn2->tree_to_edge, tcount * topsize))
    return false;

  tcount = (size_t) (8 * conn1->num_trees);
  if (memcmp (conn1->tree_to_corner, conn2->tree_to_corner, tcount * topsize))
    return false;

  tcount = (size_t) (6 * conn1->num_trees);
  if (memcmp (conn1->tree_to_tree, conn2->tree_to_tree,
              tcount * topsize) ||
      memcmp (conn1->tree_to_face, conn2->tree_to_face, tcount * int8size))
    return false;

  num_edges = conn1->num_edges;
  num_ett = conn1->ett_offset[num_edges];
  if (memcmp (conn1->ett_offset, conn2->ett_offset,
              topsize * (num_edges + 1)) ||
      memcmp (conn1->edge_to_tree, conn2->edge_to_tree,
              topsize * num_ett) ||
      memcmp (conn1->edge_to_edge, conn2->edge_to_edge, int8size * num_ett))
    return false;

  num_corners = conn1->num_corners;
  num_ctt = conn1->ctt_offset[num_corners];
  if (memcmp (conn1->ctt_offset, conn2->ctt_offset,
              topsize * (num_corners + 1)) ||
      memcmp (conn1->corner_to_tree, conn2->corner_to_tree,
              topsize * num_ctt) ||
      memcmp (conn1->corner_to_corner, conn2->corner_to_corner,
              int8size * num_ctt))
    return false;

  return true;
}

void
p8est_connectivity_save (const char *filename, p8est_connectivity_t * conn)
{
  int                 retval;
  char                magic6[6];
  size_t              u64z, topsize, int8size;
  size_t              tcount;
  uint64_t            array8[8];
  p4est_topidx_t      num_vertices, num_trees;
  p4est_topidx_t      num_edges, num_ett, num_corners, num_ctt;
  FILE               *file;

  P4EST_ASSERT (p8est_connectivity_is_valid (conn));

  file = fopen (filename, "wb");
  SC_CHECK_ABORT (file != NULL, "file open");

  num_vertices = conn->num_vertices;
  num_trees = conn->num_trees;
  num_edges = conn->num_edges;
  num_ett = conn->ett_offset[num_edges];
  num_corners = conn->num_corners;
  num_ctt = conn->ctt_offset[num_corners];

  strncpy (magic6, P8EST_STRING, 6);
  sc_fwrite (magic6, 6, 1, file, "write magic");

  u64z = sizeof (uint64_t);
  topsize = sizeof (p4est_topidx_t);
  int8size = sizeof (int8_t);
  array8[0] = P8EST_ONDISK_FORMAT;
  array8[1] = (uint64_t) topsize;
  array8[2] = (uint64_t) num_vertices;
  array8[3] = (uint64_t) num_trees;
  array8[4] = (uint64_t) num_edges;
  array8[5] = (uint64_t) num_ett;
  array8[6] = (uint64_t) num_corners;
  array8[7] = (uint64_t) num_ctt;
  sc_fwrite (array8, u64z, 8, file, "write header");

  if (num_vertices > 0)
    sc_fwrite (conn->vertices, sizeof (double), 3 * num_vertices, file,
               "write vertices");

  tcount = (size_t) (12 * num_trees);
  if (num_edges > 0)
    sc_fwrite (conn->tree_to_edge, topsize, tcount, file, "write tte");

  tcount = (size_t) (8 * num_trees);
  if (num_vertices > 0)
    sc_fwrite (conn->tree_to_vertex, topsize, tcount, file, "write ttv");
  if (num_corners > 0)
    sc_fwrite (conn->tree_to_corner, topsize, tcount, file, "write ttc");

  tcount = (size_t) (6 * num_trees);
  sc_fwrite (conn->tree_to_tree, topsize, tcount, file, "write ttt");
  sc_fwrite (conn->tree_to_face, int8size, tcount, file, "write ttf");

  sc_fwrite (conn->ett_offset, topsize, num_edges + 1, file,
             "write ett_offset");
  if (num_edges > 0) {
    sc_fwrite (conn->edge_to_tree, topsize, num_ett, file, "write ett");
    sc_fwrite (conn->edge_to_edge, int8size, num_ett, file, "write ete");
  }

  sc_fwrite (conn->ctt_offset, topsize, num_corners + 1, file,
             "write ctt_offset");
  if (num_corners > 0) {
    sc_fwrite (conn->corner_to_tree, topsize, num_ctt, file, "write ctt");
    sc_fwrite (conn->corner_to_corner, int8size, num_ctt, file, "write ctc");
  }

  retval = fclose (file);
  SC_CHECK_ABORT (retval == 0, "file close");
}

p8est_connectivity_t *
p8est_connectivity_load (const char *filename, long *length)
{
  int                 retval;
  char                magic6[6];
  size_t              u64z, topsize, int8size;
  size_t              tcount;
  uint64_t            array8[8];
  p4est_topidx_t      num_vertices, num_trees;
  p4est_topidx_t      num_edges, num_ett, num_corners, num_ctt;
  FILE               *file;
  p8est_connectivity_t *conn = NULL;

  file = fopen (filename, "rb");
  SC_CHECK_ABORT (file != NULL, "file open");

  sc_fread (magic6, 6, 1, file, "read magic");
  SC_CHECK_ABORT (strncmp (magic6, P8EST_STRING, 6) == 0, "invalid magic");

  u64z = sizeof (uint64_t);
  topsize = sizeof (p4est_topidx_t);
  int8size = sizeof (int8_t);
  sc_fread (array8, u64z, 8, file, "read header");

  SC_CHECK_ABORT (array8[0] == P8EST_ONDISK_FORMAT,
                  "on-disk format mismatch");
  SC_CHECK_ABORT (array8[1] == (uint64_t) topsize,
                  "p4est_topidx_t size mismatch");

  num_vertices = (p4est_topidx_t) array8[2];
  num_trees = (p4est_topidx_t) array8[3];
  num_edges = (p4est_topidx_t) array8[4];
  num_ett = (p4est_topidx_t) array8[5];
  num_corners = (p4est_topidx_t) array8[6];
  num_ctt = (p4est_topidx_t) array8[7];
  SC_CHECK_ABORT (num_vertices >= 0, "negative num_vertices");
  SC_CHECK_ABORT (num_trees >= 0, "negative num_trees");
  SC_CHECK_ABORT (num_edges >= 0, "negative num_edges");
  SC_CHECK_ABORT (num_ett >= 0, "negative num_ett");
  SC_CHECK_ABORT (num_corners >= 0, "negative num_corners");
  SC_CHECK_ABORT (num_ctt >= 0, "negative num_ctt");

  conn = p8est_connectivity_new (num_vertices, num_trees, num_edges, num_ett,
                                 num_corners, num_ctt);

  if (num_vertices > 0)
    sc_fread (conn->vertices, sizeof (double), 3 * num_vertices, file,
              "read vertices");

  tcount = (size_t) (12 * num_trees);
  if (num_edges > 0)
    sc_fread (conn->tree_to_edge, topsize, tcount, file, "read tte");

  tcount = (size_t) (8 * num_trees);
  if (num_vertices > 0)
    sc_fread (conn->tree_to_vertex, topsize, tcount, file, "read ttv");
  if (num_corners > 0)
    sc_fread (conn->tree_to_corner, topsize, tcount, file, "read ttc");

  tcount = (size_t) (6 * num_trees);
  sc_fread (conn->tree_to_tree, topsize, tcount, file, "read ttt");
  sc_fread (conn->tree_to_face, int8size, tcount, file, "read ttf");

  sc_fread (conn->ett_offset, topsize, num_edges + 1, file,
            "read ett_offset");
  SC_CHECK_ABORT (num_ett == conn->ett_offset[num_edges], "num_ett mismatch");
  if (num_edges > 0) {
    sc_fread (conn->edge_to_tree, topsize, num_ett, file, "read ett");
    sc_fread (conn->edge_to_edge, int8size, num_ett, file, "read ete");
  }

  sc_fread (conn->ctt_offset, topsize, num_corners + 1, file,
            "read ctt_offset");
  SC_CHECK_ABORT (num_ctt == conn->ctt_offset[num_corners],
                  "num_ctt mismatch");
  if (num_corners > 0) {
    sc_fread (conn->corner_to_tree, topsize, num_ctt, file, "read ctt");
    sc_fread (conn->corner_to_corner, int8size, num_ctt, file, "read ctc");
  }

  if (length != NULL) {
    *length = ftell (file);
    SC_CHECK_ABORT (*length > 0, "file tell");
  }

  retval = fclose (file);
  SC_CHECK_ABORT (retval == 0, "file close");

  SC_CHECK_ABORT (p8est_connectivity_is_valid (conn), "invalid connectivity");

  return conn;
}

p8est_connectivity_t *
p8est_connectivity_new_unitcube (void)
{
  const p4est_topidx_t num_vertices = 8;
  const p4est_topidx_t num_trees = 1;
  const p4est_topidx_t num_ett = 0;
  const p4est_topidx_t num_ctt = 0;
  const double        vertices[8 * 3] = {
    0, 0, 0,
    1, 0, 0,
    0, 1, 0,
    1, 1, 0,
    0, 0, 1,
    1, 0, 1,
    0, 1, 1,
    1, 1, 1,
  };
  const p4est_topidx_t tree_to_vertex[1 * 8] = {
    0, 1, 2, 3, 4, 5, 6, 7,
  };
  const p4est_topidx_t tree_to_tree[1 * 6] = {
    0, 0, 0, 0, 0, 0,
  };
  const int8_t        tree_to_face[1 * 6] = {
    0, 1, 2, 3, 4, 5,
  };

  return p8est_connectivity_new_copy (num_vertices, num_trees, 0, 0,
                                      vertices, tree_to_vertex,
                                      tree_to_tree, tree_to_face,
                                      NULL, &num_ett, NULL, NULL,
                                      NULL, &num_ctt, NULL, NULL);
}

p8est_connectivity_t *
p8est_connectivity_new_periodic (void)
{
  const p4est_topidx_t num_vertices = 8;
  const p4est_topidx_t num_trees = 1;
  const p4est_topidx_t num_edges = 3;
  const p4est_topidx_t num_corners = 1;
  const double        vertices[8 * 3] = {
    0, 0, 0,
    1, 0, 0,
    0, 1, 0,
    1, 1, 0,
    0, 0, 1,
    1, 0, 1,
    0, 1, 1,
    1, 1, 1,
  };
  const p4est_topidx_t tree_to_vertex[1 * 8] = {
    0, 1, 2, 3, 4, 5, 6, 7,
  };
  const p4est_topidx_t tree_to_tree[1 * 6] = {
    0, 0, 0, 0, 0, 0,
  };
  const int8_t        tree_to_face[1 * 6] = {
    1, 0, 3, 2, 5, 4,
  };
  const p4est_topidx_t tree_to_edge[1 * 12] = {
    0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
  };
  const p4est_topidx_t ett_offset[3 + 1] = {
    0, 4, 8, 12,
  };
  const p4est_topidx_t edge_to_tree[12] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  };
  const int8_t        edge_to_edge[12] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
  };
  const p4est_topidx_t tree_to_corner[1 * 8] = {
    0, 0, 0, 0, 0, 0, 0, 0,
  };
  const p4est_topidx_t ctt_offset[1 + 1] = {
    0, 8,
  };
  const p4est_topidx_t corner_to_tree[8] = {
    0, 0, 0, 0, 0, 0, 0, 0,
  };
  const int8_t        corner_to_corner[8] = {
    0, 1, 2, 3, 4, 5, 6, 7,
  };

  return p8est_connectivity_new_copy (num_vertices, num_trees,
                                      num_edges, num_corners,
                                      vertices, tree_to_vertex,
                                      tree_to_tree, tree_to_face,
                                      tree_to_edge, ett_offset,
                                      edge_to_tree, edge_to_edge,
                                      tree_to_corner, ctt_offset,
                                      corner_to_tree, corner_to_corner);
}

p8est_connectivity_t *
p8est_connectivity_new_rotwrap (void)
{
  const p4est_topidx_t num_vertices = 8;
  const p4est_topidx_t num_trees = 1;
  const p4est_topidx_t num_edges = 4;
  const p4est_topidx_t num_corners = 1;
  const double        vertices[8 * 3] = {
    0, 0, 0,
    1, 0, 0,
    0, 1, 0,
    1, 1, 0,
    0, 0, 1,
    1, 0, 1,
    0, 1, 1,
    1, 1, 1,
  };
  const p4est_topidx_t tree_to_vertex[1 * 8] = {
    0, 1, 2, 3, 4, 5, 6, 7,
  };
  const p4est_topidx_t tree_to_tree[1 * 6] = {
    0, 0, 0, 0, 0, 0,
  };
  const int8_t        tree_to_face[1 * 6] = {
    1, 0, 2, 3, 11, 10,
  };
  const p4est_topidx_t tree_to_edge[1 * 12] = {
    0, 0, 1, 1, 1, 1, 0, 0, 2, 2, 3, 3,
  };
  const p4est_topidx_t ett_offset[4 + 1] = {
    0, 4, 8, 10, 12,
  };
  const p4est_topidx_t edge_to_tree[12] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  };
  const int8_t        edge_to_edge[12] = {
    0, 7, 1, 6, 2, 16, 3, 17, 8, 9, 10, 11,
  };
  const p4est_topidx_t tree_to_corner[1 * 8] = {
    0, 0, 0, 0, 0, 0, 0, 0,
  };
  const p4est_topidx_t ctt_offset[1 + 1] = {
    0, 8,
  };
  const p4est_topidx_t corner_to_tree[8] = {
    0, 0, 0, 0, 0, 0, 0, 0,
  };
  const int8_t        corner_to_corner[8] = {
    0, 1, 2, 3, 4, 5, 6, 7,
  };

  return p8est_connectivity_new_copy (num_vertices, num_trees,
                                      num_edges, num_corners,
                                      vertices, tree_to_vertex,
                                      tree_to_tree, tree_to_face,
                                      tree_to_edge, ett_offset,
                                      edge_to_tree, edge_to_edge,
                                      tree_to_corner, ctt_offset,
                                      corner_to_tree, corner_to_corner);
}

p8est_connectivity_t *
p8est_connectivity_new_twocubes (void)
{
  const p4est_topidx_t num_vertices = 12;
  const p4est_topidx_t num_trees = 2;
  const p4est_topidx_t num_ett = 0;
  const p4est_topidx_t num_ctt = 0;
  const double        vertices[12 * 3] = {
    0, 0, 0,
    1, 0, 0,
    2, 0, 0,
    0, 1, 0,
    1, 1, 0,
    2, 1, 0,
    0, 0, 1,
    1, 0, 1,
    2, 0, 1,
    0, 1, 1,
    1, 1, 1,
    2, 1, 1,
  };
  const p4est_topidx_t tree_to_vertex[2 * 8] = {
    0, 1, 3, 4, 6, 7, 9, 10,
    1, 2, 4, 5, 7, 8, 10, 11,
  };
  const p4est_topidx_t tree_to_tree[2 * 6] = {
    0, 1, 0, 0, 0, 0,
    0, 1, 1, 1, 1, 1,
  };
  const int8_t        tree_to_face[2 * 6] = {
    0, 0, 2, 3, 4, 5,
    1, 1, 2, 3, 4, 5,
  };

  return p8est_connectivity_new_copy (num_vertices, num_trees, 0, 0,
                                      vertices, tree_to_vertex,
                                      tree_to_tree, tree_to_face,
                                      NULL, &num_ett, NULL, NULL,
                                      NULL, &num_ctt, NULL, NULL);
}

p8est_connectivity_t *
p8est_connectivity_new_brick (p4est_topidx_t m, p4est_topidx_t n,
                              p4est_topidx_t p, bool periodic_a,
                              bool periodic_b, bool periodic_c)
{
  const p4est_topidx_t num_trees = m * n * p;
  const p4est_topidx_t num_vertices = (m + 1) * (n + 1) * (p + 1);
  const int           mc = periodic_a ? m : (m - 1);
  const int           nc = periodic_b ? n : (n - 1);
  const int           pc = periodic_c ? p : (p - 1);
  const p4est_topidx_t num_corners = mc * nc * pc;
  const p4est_topidx_t num_ctt = 8 * num_corners;
  const p4est_topidx_t num_edges = m * nc * pc + mc * n * pc + mc * nc * p;
  const p4est_topidx_t num_ett = 4 * num_edges;
  const bool          periodic[3] = { periodic_a, periodic_b, periodic_c };
  const int           max[3] = { m - 1, n - 1, p - 1 };
  double             *vertices;
  p4est_topidx_t     *tree_to_vertex;
  p4est_topidx_t     *tree_to_tree;
  int8_t             *tree_to_face;
  p4est_topidx_t     *tree_to_edge;
  p4est_topidx_t     *ett_offset;
  p4est_topidx_t     *edge_to_tree;
  int8_t             *edge_to_edge;
  p4est_topidx_t     *tree_to_corner;
  p4est_topidx_t     *ctt_offset;
  p4est_topidx_t     *corner_to_tree;
  int8_t             *corner_to_corner;
  p4est_topidx_t      cube_length, n_iter;
  int8_t              log_cl;
  int                 i, j, k, l;
  p4est_topidx_t      ti, tj, tk, tl;
  p4est_topidx_t      tx, ty, tz;
  p4est_topidx_t      tfx[6], tfy[6], tfz[6];
  p4est_topidx_t      tex[12], tey[12], tez[12];
  p4est_topidx_t      tcx[8], tcy[8], tcz[8];
  p4est_topidx_t      tf[6], te[12], tc[8];
  p4est_topidx_t      coord[3], ttemp;
  p4est_topidx_t     *linear_to_tree;
  p4est_topidx_t     *tree_to_corner2;
  p4est_topidx_t     *tree_to_edge2;
  p4est_topidx_t      vcount = 0, vicount = 0;
  int8_t              dir1, dir2, temp;
  int8_t              c[3];

  P4EST_ASSERT (m > 0 && n > 0 && p > 0);
  p8est_connectivity_t *conn = p8est_connectivity_new (num_vertices,
                                                       num_trees,
                                                       num_edges,
                                                       num_ett,
                                                       num_corners,
                                                       num_ctt);

  vertices = conn->vertices;
  tree_to_vertex = conn->tree_to_vertex;
  tree_to_tree = conn->tree_to_tree;
  tree_to_face = conn->tree_to_face;
  tree_to_edge = conn->tree_to_edge;
  ett_offset = conn->ett_offset;
  edge_to_tree = conn->edge_to_tree;
  edge_to_edge = conn->edge_to_edge;
  tree_to_corner = conn->tree_to_corner;
  ctt_offset = conn->ctt_offset;
  corner_to_tree = conn->corner_to_tree;
  corner_to_corner = conn->corner_to_corner;

  for (ti = 0; ti < num_edges + 1; ti++) {
    ett_offset[ti] = 4 * ti;
  }

  for (ti = 0; ti < num_corners + 1; ti++) {
    ctt_offset[ti] = 8 * ti;
  }

  cube_length = (m > n) ? m : n;
  cube_length = (cube_length > p) ? cube_length : p;
  log_cl = SC_LOG2_32 (cube_length);
  if (cube_length > (1 << log_cl)) {
    log_cl++;
  }
  cube_length = (1 << log_cl);
  n_iter = cube_length * cube_length * cube_length;

  linear_to_tree = P4EST_ALLOC (p4est_topidx_t, n_iter);
  tree_to_corner2 = P4EST_ALLOC (p4est_topidx_t, num_trees);
  tree_to_edge2 = P4EST_ALLOC (p4est_topidx_t, 3 * num_trees);

  tj = 0;
  tk = 0;
  tl = 0;
  for (ti = 0; ti < n_iter; ti++) {
    tx = 0;
    ty = 0;
    tz = 0;
    for (i = 0; i < log_cl; i++) {
      tx |= (ti & (1 << (3 * i))) >> (2 * i);
      ty |= (ti & (1 << (3 * i + 1))) >> (2 * i + 1);
      tz |= (ti & (1 << (3 * i + 2))) >> (2 * i + 2);
    }
    if (tx < m && ty < n && tz < p) {
      linear_to_tree[ti] = tj;
      if ((tx < m - 1 || periodic_a) && (ty < n - 1 || periodic_b) &&
          (ty < p - 1 || periodic_c)) {
        tree_to_corner2[tj] = tk++;
        tree_to_edge2[3 * tj] = tl++;
        tree_to_edge2[3 * tj + 1] = tl++;
        tree_to_edge2[3 * tj + 2] = tl++;
      }
      else {
        tree_to_corner2[tj] = -1;
        if ((ty < n - 1 || periodic_b) && (tz < p - 1 || periodic_c)) {
          tree_to_edge2[3 * tj] = tl++;
        }
        else {
          tree_to_edge2[3 * tj] = -1;
        }
        if ((tx < m - 1 || periodic_a) && (tz < p - 1 || periodic_c)) {
          tree_to_edge2[3 * tj + 1] = tl++;
        }
        else {
          tree_to_edge2[3 * tj + 1] = -1;
        }
        if ((tx < m - 1 || periodic_a) && (ty < n - 1 || periodic_b)) {
          tree_to_edge2[3 * tj + 2] = tl++;
        }
        else {
          tree_to_edge2[3 * tj + 2] = -1;
        }
      }
      tj++;
    }
    else {
      linear_to_tree[ti] = -1;
    }
  }
  P4EST_ASSERT (tj == num_trees);
  P4EST_ASSERT (tk == num_corners);
  P4EST_ASSERT (tl == num_edges);

  for (ti = 0; ti < n_iter; ti++) {
    tx = ty = tz = 0;
    for (i = 0; i < log_cl; i++) {
      tx |= (ti & (1 << (3 * i))) >> (2 * i);
      ty |= (ti & (1 << (3 * i + 1))) >> (2 * i + 1);
      tz |= (ti & (1 << (3 * i + 2))) >> (2 * i + 2);
    }
    coord[0] = tx;
    coord[1] = ty;
    coord[2] = tz;
    if (tx < m && ty < n && tz < p) {
      tj = linear_to_tree[ti];
      P4EST_ASSERT (tj >= 0);
      for (i = 0; i < 3; i++) {
        for (j = 0; j < 2; j++) {
          l = i * 2 + j;
          tfx[l] = ((tx + ((i == 0) ? (2 * j - 1) : 0)) + m) % m;
          tfy[l] = ((ty + ((i == 1) ? (2 * j - 1) : 0)) + n) % n;
          tfz[l] = ((tz + ((i == 2) ? (2 * j - 1) : 0)) + p) % p;
          tf[l] = 0;
          for (k = 0; k < log_cl; k++) {
            tf[l] |= (tfx[l] & (1 << k)) << (2 * k);
            tf[l] |= (tfy[l] & (1 << k)) << (2 * k + 1);
            tf[l] |= (tfz[l] & (1 << k)) << (2 * k + 2);
          }
          tf[l] = linear_to_tree[tf[l]];
          P4EST_ASSERT (tf[l] >= 0);
        }
        for (j = 0; j < 4; j++) {
          l = 4 * i + j;
          tex[l] = ((tx + ((i == 0) ? 0 : (2 * (j % 2) - 1))) + m) % m;
          tey[l] = ((ty + ((i == 1) ? 0 :
                           (2 * ((i == 0) ? (j % 2) : (j / 2)) - 1))) +
                    n) % n;
          tez[l] = ((tz + ((i == 2) ? 0 : (2 * (j / 2) - 1))) + p) % p;
          te[l] = 0;
          for (k = 0; k < log_cl; k++) {
            te[l] |= (tex[l] & (1 << k)) << (2 * k);
            te[l] |= (tey[l] & (1 << k)) << (2 * k + 1);
            te[l] |= (tez[l] & (1 << k)) << (2 * k + 2);
          }
          te[l] = linear_to_tree[te[l]];
          P4EST_ASSERT (te[l] >= 0);
        }
      }
      for (i = 0; i < 8; i++) {
        tcx[i] = ((tx + (((i % 2) == 0) ? -1 : 1)) + m) % m;
        tcy[i] = ((ty + ((((i / 2) % 2) == 0) ? -1 : 1)) + n) % n;
        tcz[i] = ((tz + (((i / 4) == 0) ? -1 : 1)) + p) % p;
        tc[i] = 0;
        for (j = 0; j < log_cl; j++) {
          tc[i] |= (tcx[i] & (1 << j)) << (2 * j);
          tc[i] |= (tcy[i] & (1 << j)) << (2 * j + 1);
          tc[i] |= (tcz[i] & (1 << j)) << (2 * j + 2);
        }
        tc[i] = linear_to_tree[tc[i]];
        P4EST_ASSERT (tc[i] >= 0);
      }
      for (i = 0; i < 3; i++) {
        for (j = 0; j < 2; j++) {
          l = i * 2 + j;
          if (periodic[i] == false &&
              ((coord[i] == 0 && j == 0) || (coord[i] == max[i] && j == 1))) {
            tree_to_tree[tj * 6 + l] = tj;
            tree_to_face[tj * 6 + l] = l;
          }
          else {
            tree_to_tree[tj * 6 + l] = tf[l];
            tree_to_face[tj * 6 + l] = i * 2 + (j ^ 1);
          }
        }
        if (tree_to_edge != NULL) {
          dir1 = (i + 1) % 3;
          dir2 = (i + 2) % 3;
          if (dir2 < dir1) {
            temp = dir1;
            dir1 = dir2;
            dir2 = temp;
          }
          for (j = 0; j < 4; j++) {
            l = i * 4 + j;
            if ((periodic[dir1] == false &&
                 ((coord[dir1] == 0 && (j % 2) == 0) ||
                  (coord[dir1] == max[dir1] && (j % 2) == 1))) ||
                (periodic[dir2] == false &&
                 ((coord[dir2] == 0 && (j / 2) == 0) ||
                  (coord[dir2] == max[dir2] && (j / 2) == 1)))) {
              tree_to_edge[tj * 12 + l] = -1;
            }
            else {
              if (j == 0) {
                ttemp = tree_to_edge2[te[l] * 3 + i];
                P4EST_ASSERT (ttemp >= 0);
                tree_to_edge[tj * 12 + l] = ttemp;
                edge_to_tree[4 * ttemp + 3] = tj;
                edge_to_edge[4 * ttemp + 3] = l;
              }
              else if (j == 3) {
                ttemp = tree_to_edge2[tj * 3 + i];
                P4EST_ASSERT (ttemp >= 0);
                tree_to_edge[tj * 12 + l] = ttemp;
                edge_to_tree[4 * ttemp] = tj;
                edge_to_edge[4 * ttemp] = l;
              }
              else if (j == 1) {
                ttemp = tree_to_edge2[tf[dir2 * 2] * 3 + i];
                P4EST_ASSERT (ttemp >= 0);
                tree_to_edge[tj * 12 + l] = ttemp;
                edge_to_tree[4 * ttemp + 2] = tj;
                edge_to_edge[4 * ttemp + 2] = l;
              }
              else if (j == 2) {
                ttemp = tree_to_edge2[tf[dir1 * 2] * 3 + i];
                P4EST_ASSERT (ttemp >= 0);
                tree_to_edge[tj * 12 + l] = ttemp;
                edge_to_tree[4 * ttemp + 1] = tj;
                edge_to_edge[4 * ttemp + 1] = l;
              }
            }
          }
        }
      }
      if (tree_to_corner != NULL) {
        for (i = 0; i < 8; i++) {
          c[0] = i % 2;
          c[1] = (i / 2) % 2;
          c[2] = i / 4;
          if ((periodic[0] == false &&
               ((coord[0] == 0 && c[0] == 0) ||
                (coord[0] == max[0] && c[0] == 1))) ||
              (periodic[1] == false &&
               ((coord[1] == 0 && c[1] == 0) ||
                (coord[1] == max[1] && c[1] == 1))) ||
              (periodic[2] == false &&
               ((coord[2] == 0 && c[2] == 0) ||
                (coord[2] == max[2] && c[2] == 1)))) {
            tree_to_corner[tj * 8 + i] = -1;
          }
          else {
            switch (i) {
            case 0:
              ttemp = tree_to_corner2[tc[0]];
              P4EST_ASSERT (ttemp >= 0);
              tree_to_corner[tj * 8] = ttemp;
              corner_to_tree[ttemp * 8 + 7] = tj;
              corner_to_corner[ttemp * 8 + 7] = 0;
              break;
            case 1:
              ttemp = tree_to_corner2[te[0]];
              P4EST_ASSERT (ttemp >= 0);
              tree_to_corner[tj * 8 + 1] = ttemp;
              corner_to_tree[ttemp * 8 + 6] = tj;
              corner_to_corner[ttemp * 8 + 6] = 1;
              break;
            case 2:
              ttemp = tree_to_corner2[te[4]];
              P4EST_ASSERT (ttemp >= 0);
              tree_to_corner[tj * 8 + 2] = ttemp;
              corner_to_tree[ttemp * 8 + 5] = tj;
              corner_to_corner[ttemp * 8 + 5] = 2;
              break;
            case 3:
              ttemp = tree_to_corner2[tf[4]];
              P4EST_ASSERT (ttemp >= 0);
              tree_to_corner[tj * 8 + 3] = ttemp;
              corner_to_tree[ttemp * 8 + 4] = tj;
              corner_to_corner[ttemp * 8 + 4] = 3;
              break;
            case 4:
              ttemp = tree_to_corner2[te[8]];
              P4EST_ASSERT (ttemp >= 0);
              tree_to_corner[tj * 8 + 4] = ttemp;
              corner_to_tree[ttemp * 8 + 3] = tj;
              corner_to_corner[ttemp * 8 + 3] = 4;
              break;
            case 5:
              ttemp = tree_to_corner2[tf[2]];
              P4EST_ASSERT (ttemp >= 0);
              tree_to_corner[tj * 8 + 5] = ttemp;
              corner_to_tree[ttemp * 8 + 2] = tj;
              corner_to_corner[ttemp * 8 + 2] = 5;
              break;
            case 6:
              ttemp = tree_to_corner2[tf[0]];
              P4EST_ASSERT (ttemp >= 0);
              tree_to_corner[tj * 8 + 6] = ttemp;
              corner_to_tree[ttemp * 8 + 1] = tj;
              corner_to_corner[ttemp * 8 + 1] = 6;
              break;
            case 7:
              ttemp = tree_to_corner2[tj];
              P4EST_ASSERT (ttemp >= 0);
              tree_to_corner[tj * 8 + 7] = ttemp;
              corner_to_tree[ttemp * 8] = tj;
              corner_to_corner[ttemp * 8] = 7;
              break;
            }
          }
        }
      }
      if (tx == 0 && ty == 0 && tz == 0) {
        tree_to_vertex[tj * 8] = vcount++;
        vertices[vicount++] = 0;
        vertices[vicount++] = 0;
        vertices[vicount++] = 0;
      }
      else {
        if (tx == 0) {
          if (ty == 0) {
            tree_to_vertex[tj * 8] = tree_to_vertex[tf[4] * 8 + 4];
          }
          else {
            tree_to_vertex[tj * 8] = tree_to_vertex[tf[2] * 8 + 2];
          }
        }
        else {
          tree_to_vertex[tj * 8] = tree_to_vertex[tf[0] * 8 + 1];
        }
      }
      if (ty == 0 && tz == 0) {
        tree_to_vertex[tj * 8 + 1] = vcount++;
        vertices[vicount++] = tx + 1;
        vertices[vicount++] = 0;
        vertices[vicount++] = 0;
      }
      else {
        if (ty == 0) {
          tree_to_vertex[tj * 8 + 1] = tree_to_vertex[tf[4] * 8 + 5];
        }
        else {
          tree_to_vertex[tj * 8 + 1] = tree_to_vertex[tf[2] * 8 + 3];
        }
      }
      if (tx == 0 && tz == 0) {
        tree_to_vertex[tj * 8 + 2] = vcount++;
        vertices[vicount++] = 0;
        vertices[vicount++] = ty + 1;
        vertices[vicount++] = 0;
      }
      else {
        if (tx == 0) {
          tree_to_vertex[tj * 8 + 2] = tree_to_vertex[tf[4] * 8 + 6];
        }
        else {
          tree_to_vertex[tj * 8 + 2] = tree_to_vertex[tf[0] * 8 + 3];
        }
      }
      if (tz == 0) {
        tree_to_vertex[tj * 8 + 3] = vcount++;
        vertices[vicount++] = tx + 1;
        vertices[vicount++] = ty + 1;
        vertices[vicount++] = 0;
      }
      else {
        tree_to_vertex[tj * 8 + 3] = tree_to_vertex[tf[4] * 8 + 7];
      }
      if (tx == 0 && ty == 0) {
        tree_to_vertex[tj * 8 + 4] = vcount++;
        vertices[vicount++] = 0;
        vertices[vicount++] = 0;
        vertices[vicount++] = tz + 1;
      }
      else {
        if (tx == 0) {
          tree_to_vertex[tj * 8 + 4] = tree_to_vertex[tf[2] * 8 + 6];
        }
        else {
          tree_to_vertex[tj * 8 + 4] = tree_to_vertex[tf[0] * 8 + 5];
        }
      }
      if (ty == 0) {
        tree_to_vertex[tj * 8 + 5] = vcount++;
        vertices[vicount++] = tx + 1;
        vertices[vicount++] = 0;
        vertices[vicount++] = tz + 1;
      }
      else {
        tree_to_vertex[tj * 8 + 5] = tree_to_vertex[tf[2] * 8 + 7];
      }
      if (tx == 0) {
        tree_to_vertex[tj * 8 + 6] = vcount++;
        vertices[vicount++] = 0;
        vertices[vicount++] = ty + 1;
        vertices[vicount++] = tz + 1;
      }
      else {
        tree_to_vertex[tj * 8 + 6] = tree_to_vertex[tf[0] * 8 + 7];
      }
      tree_to_vertex[tj * 8 + 7] = vcount++;
      vertices[vicount++] = tx + 1;
      vertices[vicount++] = ty + 1;
      vertices[vicount++] = tz + 1;
    }
  }

  P4EST_FREE (linear_to_tree);
  P4EST_FREE (tree_to_corner2);
  P4EST_FREE (tree_to_edge2);

  P4EST_ASSERT (p8est_connectivity_is_valid (conn));

  return conn;

}

p8est_connectivity_t *
p8est_connectivity_new_rotcubes (void)
{
  const p4est_topidx_t num_vertices = 26;
  const p4est_topidx_t num_trees = 6;
  const p4est_topidx_t num_edges = 3;
  const p4est_topidx_t num_corners = 1;
  const double        vertices[26 * 3] = {
    0, 0, 0,
    1, 0, 2,
    2, 0, 0,
    0, 1, 0,
    1, 1, 0,
    2, 1, 0,
    1, -1, 0,
    2, -1, 0,
    1, -1, 1,
    2, -1, 1,
    2, 1, 1,
    1, 0, 1,
    2, 0, 1,
    0, 1, 1,
    1, 1, 1,
    0, 0, 1,
    0, 0, 2,
    1, 0, 0,
    1, 1, 2,
    0, 1, 2,
    2.5, 1.5, 2,
    2, 1.5, 2,
    2, 1.5, 2.5,
    2, .5, 2.5,
    2.5, .5, 2,
    2, .5, 2,
  };
  const p4est_topidx_t tree_to_vertex[6 * 8] = {
    0, 17, 3, 4, 15, 11, 13, 14,
    7, 2, 6, 17, 9, 12, 8, 11,
    2, 12, 5, 10, 17, 11, 4, 14,
    19, 13, 18, 14, 16, 15, 1, 11,
    14, 11, 21, 25, 18, 1, 22, 23,
    21, 20, 25, 24, 14, 10, 11, 12,
  };
  const p4est_topidx_t tree_to_tree[6 * 6] = {
    0, 2, 0, 0, 0, 3,
    1, 2, 1, 1, 1, 1,
    2, 5, 1, 2, 2, 0,
    3, 0, 3, 4, 3, 3,
    4, 4, 3, 4, 5, 4,
    4, 5, 5, 5, 5, 2,
  };
  const int8_t        tree_to_face[6 * 6] = {
    0, 5, 2, 3, 4, 13,
    0, 2, 2, 3, 4, 5,
    0, 23, 1, 3, 4, 1,
    0, 17, 2, 8, 4, 5,
    0, 1, 9, 3, 12, 5,
    16, 1, 2, 3, 4, 19,
  };
  const p4est_topidx_t tree_to_edge[6 * 12] = {
    -1, -1, -1, -1, -1, -1, -1, 0, -1, 2, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, 1, -1, -1, -1, 2,
    -1, -1, 2, -1, -1, -1, -1, 0, -1, 1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0,
    0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, 1, -1, -1, 0, -1, -1, -1, -1, -1,
  };
  const p4est_topidx_t ett_offset[3 + 1] = { 0, 5, 8, 11 };
  const p4est_topidx_t edge_to_tree[11] = {
    0, 2, 3, 4, 5, 1, 2, 5, 0, 1, 2
  };
  const int8_t        edge_to_edge[11] = {
    7, 7, 23, 12, 18, 7, 9, 15, 9, 11, 2
  };
  const p4est_topidx_t tree_to_corner[6 * 8] = {
    -1, -1, -1, -1, -1, 0, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, 0,
    -1, -1, -1, -1, -1, 0, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, 0,
    -1, 0, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, 0, -1,
  };
  const p4est_topidx_t ctt_offset[1 + 1] = { 0, 6 };
  const p4est_topidx_t corner_to_tree[6] = { 0, 1, 2, 3, 4, 5 };
  const int8_t        corner_to_corner[6] = { 5, 7, 5, 7, 1, 6 };

  return p8est_connectivity_new_copy (num_vertices, num_trees,
                                      num_edges, num_corners,
                                      vertices, tree_to_vertex,
                                      tree_to_tree, tree_to_face,
                                      tree_to_edge, ett_offset,
                                      edge_to_tree, edge_to_edge,
                                      tree_to_corner, ctt_offset,
                                      corner_to_tree, corner_to_corner);
}

p8est_connectivity_t *
p8est_connectivity_new_shell (void)
{
/* *INDENT-OFF* */
  const p4est_topidx_t num_vertices = 18;
  const p4est_topidx_t num_trees =    24;
  const p4est_topidx_t num_edges =    18;
  const p4est_topidx_t num_corners =   0;
  const p4est_topidx_t ctt_offset =    0;
  const double        vertices[18 * 3] = {
    -1, -1,  1,
     0, -1,  1,
     1, -1,  1,
    -1,  0,  1,
     0,  0,  1,
     1,  0,  1,
    -1,  1,  1,
     0,  1,  1,
     1,  1,  1,
    -1, -1,  2,
     0, -1,  2,
     1, -1,  2,
    -1,  0,  2,
     0,  0,  2,
     1,  0,  2,
    -1,  1,  2,
     0,  1,  2,
     1,  1,  2,
  };
  const p4est_topidx_t tree_to_vertex[24 * 8] = {
    0, 1, 3, 4,  9, 10, 12, 13,
    1, 2, 4, 5, 10, 11, 13, 14,
    3, 4, 6, 7, 12, 13, 15, 16,
    4, 5, 7, 8, 13, 14, 16, 17,
    0, 1, 3, 4,  9, 10, 12, 13,
    1, 2, 4, 5, 10, 11, 13, 14,
    3, 4, 6, 7, 12, 13, 15, 16,
    4, 5, 7, 8, 13, 14, 16, 17,
    0, 1, 3, 4,  9, 10, 12, 13,
    1, 2, 4, 5, 10, 11, 13, 14,
    3, 4, 6, 7, 12, 13, 15, 16,
    4, 5, 7, 8, 13, 14, 16, 17,
    0, 1, 3, 4,  9, 10, 12, 13,
    1, 2, 4, 5, 10, 11, 13, 14,
    3, 4, 6, 7, 12, 13, 15, 16,
    4, 5, 7, 8, 13, 14, 16, 17,
    0, 1, 3, 4,  9, 10, 12, 13,
    1, 2, 4, 5, 10, 11, 13, 14,
    3, 4, 6, 7, 12, 13, 15, 16,
    4, 5, 7, 8, 13, 14, 16, 17,
    0, 1, 3, 4,  9, 10, 12, 13,
    1, 2, 4, 5, 10, 11, 13, 14,
    3, 4, 6, 7, 12, 13, 15, 16,
    4, 5, 7, 8, 13, 14, 16, 17,
  };
  const p4est_topidx_t tree_to_tree[24 * 6] = {
    18,  1, 14,  2,  0,  0,
     0, 23, 15,  3,  1,  1,
    16,  3,  0,  4,  2,  2,
     2, 21,  1,  5,  3,  3,
    16,  5,  2,  6,  4,  4,
     4, 21,  3,  7,  5,  5,
    17,  7,  4,  8,  6,  6,
     6, 20,  5,  9,  7,  7,
    17,  9,  6, 10,  8,  8,
     8, 20,  7, 11,  9,  9,
    19, 11,  8, 12, 10, 10,
    10, 22,  9, 13, 11, 11,
    19, 13, 10, 14, 12, 12,
    12, 22, 11, 15, 13, 13,
    18, 15, 12,  0, 14, 14,
    14, 23, 13,  1, 15, 15,
     2, 17,  4, 18, 16, 16,
    16,  8,  6, 19, 17, 17,
     0, 19, 16, 14, 18, 18,
    18, 10, 17, 12, 19, 19,
     9, 21,  7, 22, 20, 20,
    20,  3,  5, 23, 21, 21,
    11, 23, 20, 13, 22, 22,
    22,  1, 21, 15, 23, 23,
  };
  const int8_t        tree_to_face[24 * 6] = {
    6, 0, 3, 2, 4, 5,
    1, 7, 3, 2, 4, 5,
    6, 0, 3, 2, 4, 5,
    1, 7, 3, 2, 4, 5,
    2, 0, 3, 2, 4, 5,
    1, 8, 3, 2, 4, 5,
    2, 0, 3, 2, 4, 5,
    1, 8, 3, 2, 4, 5,
    1, 0, 3, 2, 4, 5,
    1, 0, 3, 2, 4, 5,
    1, 0, 3, 2, 4, 5,
    1, 0, 3, 2, 4, 5,
    9, 0, 3, 2, 4, 5,
    1, 3, 3, 2, 4, 5,
    9, 0, 3, 2, 4, 5,
    1, 3, 3, 2, 4, 5,
    6, 0, 0, 2, 4, 5,
    1, 0, 0, 2, 4, 5,
    6, 0, 3, 6, 4, 5,
    1, 0, 3, 6, 4, 5,
    1, 0, 7, 2, 4, 5,
    1, 7, 7, 2, 4, 5,
    1, 0, 3, 1, 4, 5,
    1, 7, 3, 1, 4, 5,
  };
  const p4est_topidx_t tree_to_edge[24 * 12] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1,  8,  6,  0,
    -1, -1, -1, -1, -1, -1, -1, -1,  8, -1,  0,  7,
    -1, -1, -1, -1, -1, -1, -1, -1,  6,  0, -1,  9,
    -1, -1, -1, -1, -1, -1, -1, -1,  0,  7,  9, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1,  9, 10,  1,
    -1, -1, -1, -1, -1, -1, -1, -1,  9, -1,  1, 11,
    -1, -1, -1, -1, -1, -1, -1, -1, 10,  1, -1, 12,
    -1, -1, -1, -1, -1, -1, -1, -1,  1, 11, 12, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, 12, 13,  2,
    -1, -1, -1, -1, -1, -1, -1, -1, 12, -1,  2, 14,
    -1, -1, -1, -1, -1, -1, -1, -1, 13,  2, -1, 15,
    -1, -1, -1, -1, -1, -1, -1, -1,  2, 14, 15, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, 15, 16,  3,
    -1, -1, -1, -1, -1, -1, -1, -1, 15, -1,  3, 17,
    -1, -1, -1, -1, -1, -1, -1, -1, 16,  3, -1,  8,
    -1, -1, -1, -1, -1, -1, -1, -1,  3, 17,  8, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, 10,  6,  4,
    -1, -1, -1, -1, -1, -1, -1, -1, 10, -1,  4, 13,
    -1, -1, -1, -1, -1, -1, -1, -1,  6,  4, -1, 16,
    -1, -1, -1, -1, -1, -1, -1, -1,  4, 13, 16, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, 11, 14,  5,
    -1, -1, -1, -1, -1, -1, -1, -1, 11, -1,  5 , 7,
    -1, -1, -1, -1, -1, -1, -1, -1, 14,  5, -1, 17,
    -1, -1, -1, -1, -1, -1, -1, -1,  5,  7, 17, -1,
  };
  const p4est_topidx_t ett_offset[18 + 1] = {
     0,  4 , 8, 12, 16, 20, 24, 28, 32,
    36, 40, 44, 48, 52, 56, 60, 64, 68, 72,
  };
  const p4est_topidx_t edge_to_tree[72] = {
     0,  1,  2,  3,
     4,  5,  6,  7,
     8,  9, 10, 11,
    12, 13, 14, 15,
    16, 17, 18, 19,
    20, 21, 22, 23,
     0,  2, 16, 18,
     1,  3, 21, 23,
     0,  1, 14, 15,
     2,  3,  4,  5,
     4,  6, 16, 17,
     5,  7, 20, 21,
     6,  7,  8,  9,
     8, 10, 17, 19,
     9, 11, 20, 22,
    10, 11, 12, 13,
    12, 14, 18, 19,
    13, 15, 22, 23,
  };
  const int8_t        edge_to_edge[72] = {
    11, 10,  9,  8,
    11, 10,  9,  8,
    11, 10,  9,  8,
    11, 10,  9,  8,
    11, 10,  9,  8,
    11, 10,  9,  8,
    10,  8, 10,  8,
    11,  9, 11,  9,
     9,  8, 11, 10,
    11, 10,  9,  8,
    10,  8,  9,  8,
    11,  9,  9,  8,
    11, 10,  9,  8,
    10,  8, 11,  9,
    11,  9, 10,  8,
    11, 10,  9,  8,
    10,  8, 11, 10,
    11,  9, 11, 10,
  };
/* *INDENT-ON* */

  return p8est_connectivity_new_copy (num_vertices, num_trees,
                                      num_edges, num_corners,
                                      vertices, tree_to_vertex,
                                      tree_to_tree, tree_to_face,
                                      tree_to_edge, ett_offset,
                                      edge_to_tree, edge_to_edge,
                                      NULL, &ctt_offset, NULL, NULL);
}

p8est_connectivity_t *
p8est_connectivity_new_sphere (void)
{
/* *INDENT-OFF* */
  const p4est_topidx_t num_vertices = 16;
  const p4est_topidx_t num_trees =    13;
  const p4est_topidx_t num_edges =    12;
  const p4est_topidx_t num_corners =   0;
  const p4est_topidx_t ctt_offset = 0;
  const double        vertices[16 * 3] = {
    -1, -1,  1,
     1, -1,  1,
    -1,  1,  1,
     1,  1,  1,
    -1, -1,  2,
     1, -1,  2,
    -1,  1,  2,
     1,  1,  2,
    -1, -1, -1,
     1, -1, -1,
    -1,  1, -1,
     1,  1, -1,
    -1, -1,  1,
     1, -1,  1,
    -1,  1,  1,
     1,  1,  1,
  };
  const p4est_topidx_t tree_to_vertex[13 * 8] = {
    0,  1,  2,  3,  4,  5,  6,  7,
    0,  1,  2,  3,  4,  5,  6,  7,
    0,  1,  2,  3,  4,  5,  6,  7,
    0,  1,  2,  3,  4,  5,  6,  7,
    0,  1,  2,  3,  4,  5,  6,  7,
    0,  1,  2,  3,  4,  5,  6,  7,
    0,  1,  2,  3,  4,  5,  6,  7,
    0,  1,  2,  3,  4,  5,  6,  7,
    0,  1,  2,  3,  4,  5,  6,  7,
    0,  1,  2,  3,  4,  5,  6,  7,
    0,  1,  2,  3,  4,  5,  6,  7,
    0,  1,  2,  3,  4,  5,  6,  7,
    8,  9, 10, 11, 12, 13, 14, 15,
  };
  const p4est_topidx_t tree_to_tree[13 * 6] = {
     5,  3,  4,  1,  6,  0,
     5,  3,  0,  2,  7,  1,
     5,  3,  1,  4,  8,  2,
     2,  0,  1,  4,  9,  3,
     2,  0,  3,  5, 10,  4,
     2,  0,  4,  1, 11,  5,
    11,  9, 10,  7, 12,  0,
    11,  9,  6,  8, 12,  1,
    11,  9,  7, 10, 12,  2,
     8,  6,  7, 10, 12,  3,
     8,  6,  9, 11, 12,  4,
     8,  6, 10,  7, 12,  5,
    11,  9,  6,  8, 10,  7,
  };
  const int8_t        tree_to_face[13 * 6] = {
     1,  7,  7,  2,  5,  5,
     9,  8,  3,  2,  5,  5,
     6,  0,  3,  6,  5,  5,
     1,  7,  7,  2,  5,  5,
     9,  8,  3,  2,  5,  5,
     6,  0,  3,  6,  5,  5,
     1,  7,  7,  2,  2,  4,
     9,  8,  3,  2,  5,  4,
     6,  0,  3,  6, 15,  4,
     1,  7,  7,  2, 19,  4,
     9,  8,  3,  2, 22,  4,
     6,  0,  3,  6,  6,  4,
    10, 22,  4, 16, 22,  4,
  };
  const p4est_topidx_t tree_to_edge[13 * 12] = {
     0,  2, -1, -1,  8,  9, -1, -1, -1, -1, -1, -1,
     2,  3, -1, -1,  6,  7, -1, -1, -1, -1, -1, -1,
     3,  1, -1, -1, 10, 11, -1, -1, -1, -1, -1, -1,
     7,  5, -1, -1, 11,  9, -1, -1, -1, -1, -1, -1,
     5,  4, -1, -1,  1,  0, -1, -1, -1, -1, -1, -1,
     4,  6, -1, -1, 10,  8, -1, -1, -1, -1, -1, -1,
    -1, -1,  0,  2, -1, -1,  8,  9, -1, -1, -1, -1,
    -1, -1,  2,  3, -1, -1,  6,  7, -1, -1, -1, -1,
    -1, -1,  3,  1, -1, -1, 10, 11, -1, -1, -1, -1,
    -1, -1,  7,  5, -1, -1, 11,  9, -1, -1, -1, -1,
    -1, -1,  5,  4, -1, -1,  1,  0, -1, -1, -1, -1,
    -1, -1,  4,  6, -1, -1, 10,  8, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  };
  const p4est_topidx_t ett_offset[12 + 1] = {
    0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48,
  };
  const p4est_topidx_t edge_to_tree[48] = {
    0,  4,  6, 10,
    2,  4,  8, 10,
    0,  1,  6,  7,
    1,  2,  7,  8,
    4,  5, 10, 11,
    3,  4,  9, 10,
    1,  5,  7, 11,
    1,  3,  7,  9,
    0,  5,  6, 11,
    0,  3,  6,  9,
    2,  5,  8, 11,
    2,  3,  8,  9,
  };
  const int8_t        edge_to_edge[48] = {
     0, 17,  2, 19,
     1, 16,  3, 18,
     1,  0,  3,  2,
     1,  0,  3,  2,
    13, 12, 15, 14,
    13, 12, 15, 14,
     4, 13,  6, 15,
     5, 12,  7, 14,
     4,  5,  6,  7,
     5, 17,  7, 19,
    16,  4, 18,  6,
    17, 16, 19, 18,
  };
#if 0   /* corner information would be redundant */
  const p4est_topidx_t tree_to_corner[13 * 8] = {
     0,  1,  4,  5, -1, -1, -1, -1,
     4,  5,  6,  7, -1, -1, -1, -1,
     6,  7,  2,  3, -1, -1, -1, -1,
     7,  5,  3,  1, -1, -1, -1, -1,
     3,  1,  2,  0, -1, -1, -1, -1,
     2,  0,  6,  4, -1, -1, -1, -1,
    -1, -1, -1, -1,  0,  1,  4,  5,
    -1, -1, -1, -1,  4,  5,  6,  7,
    -1, -1, -1, -1,  6,  7,  2,  3,
    -1, -1, -1, -1,  7,  5,  3,  1,
    -1, -1, -1, -1,  3,  1,  2,  0,
    -1, -1, -1, -1,  2,  0,  6,  4,
    -1, -1, -1, -1, -1, -1, -1, -1,
  };
  const p4est_topidx_t ctt_offset[8 + 1] = {
    0, 6, 12, 18, 24, 30, 36, 42, 48,
  };
  const p4est_topidx_t corner_to_tree[48] = {
    0,  4,  5,  6, 10, 11,
    0,  3,  4,  6,  9, 10,
    2,  4,  5,  8, 10, 11,
    2,  3,  4,  8,  9, 10,
    0,  1,  5,  6,  7, 11,
    0,  1,  3,  6,  7,  9,
    1,  2,  5,  7,  8, 11,
    1,  2,  3,  7,  8,  9,
  };
  const int8_t        corner_to_corner[48] = {
    0, 3, 1, 4, 7, 5,
    1, 3, 1, 5, 7, 5,
    2, 2, 0, 6, 6, 4,
    3, 2, 0, 7, 6, 4,
    2, 0, 3, 6, 4, 7,
    3, 1, 1, 7, 5, 5,
    2, 0, 2, 6, 4, 6,
    3, 1, 0, 7, 5, 4,
  };
#endif  /* 0 */
/* *INDENT-ON* */

  return p8est_connectivity_new_copy (num_vertices, num_trees,
                                      num_edges, num_corners,
                                      vertices, tree_to_vertex,
                                      tree_to_tree, tree_to_face,
                                      tree_to_edge, ett_offset,
                                      edge_to_tree, edge_to_edge,
                                      NULL, &ctt_offset, NULL, NULL);
}

p4est_topidx_t
p8est_find_face_transform (p8est_connectivity_t * connectivity,
                           p4est_topidx_t my_tree, int my_face,
                           int ftransform[])
{
  int                 target_code, target_face, orientation, reverse;
#ifdef P4EST_DEBUG
  int                 i;
  int                *my_axis = &ftransform[0];
  int                *target_axis = &ftransform[3];
#endif
  p4est_topidx_t      target_tree;

  target_tree = connectivity->tree_to_tree[6 * my_tree + my_face];
  target_code = (int) connectivity->tree_to_face[6 * my_tree + my_face];
  target_face = target_code % 6;
  orientation = target_code / 6;

  P4EST_ASSERT (0 <= my_face && my_face < 6);
  P4EST_ASSERT (0 <= target_face && target_face < 6);
  P4EST_ASSERT (0 <= orientation && orientation < 4);

  if (target_tree == my_tree && target_face == my_face) {
    P4EST_ASSERT (orientation == 0);
    return -1;
  }

  /* the code that was here before is now in test/test_face_transform3.c */
  ftransform[0] = my_face < 2 ? 1 : 0;
  ftransform[1] = my_face < 4 ? 2 : 1;
  ftransform[2] = my_face / 2;
  reverse =
    p8est_face_permutation_refs[0][my_face] ^
    p8est_face_permutation_refs[0][target_face] ^
    (orientation == 0 || orientation == 3);
  ftransform[3 + reverse] = target_face < 2 ? 1 : 0;
  ftransform[3 + !reverse] = target_face < 4 ? 2 : 1;
  ftransform[5] = target_face / 2;
  reverse = p8est_face_permutation_refs[my_face][target_face] == 1;
  ftransform[6 + reverse] = orientation % 2;
  ftransform[6 + !reverse] = orientation / 2;
  ftransform[8] = 2 * (my_face % 2) + target_face % 2;

#ifdef P4EST_DEBUG
  for (i = 0; i < 3; ++i) {
    P4EST_ASSERT (0 <= my_axis[i] && my_axis[i] < 3);
    P4EST_ASSERT (0 <= target_axis[i] && target_axis[i] < 3);
  }
  P4EST_ASSERT (my_axis[0] != my_axis[1] &&
                my_axis[0] != my_axis[2] && my_axis[1] != my_axis[2]);
  P4EST_ASSERT (target_axis[0] != target_axis[1] &&
                target_axis[0] != target_axis[2] &&
                target_axis[1] != target_axis[2]);
#endif

  return target_tree;
}

void
p8est_find_edge_transform (p8est_connectivity_t * conn,
                           p4est_topidx_t itree, int iedge,
                           p8est_edge_info_t * ei)
{
  int                 i;
  int                 redge, nedge, iflip, nflip;
  int                 pref, pset, fc[2];
  int                 faces[2], nfaces[2], orients[2];
  bool                founds[2], nows[2];
  p4est_topidx_t      edge_trees, etree, ietree;
  p4est_topidx_t      aedge, ntree, ntrees[2];
  p8est_edge_transform_t *et;
  sc_array_t         *ta = &ei->edge_transforms;
  const int           noncorners[2] = { -1, -1 };
  const int          *fcorners[2] = { noncorners, noncorners };
  const int          *nfcorners;
#ifdef P4EST_DEBUG
  int                 flipped = 0;
#endif

  P4EST_ASSERT (0 <= itree && itree < conn->num_trees);
  P4EST_ASSERT (0 <= iedge && iedge < 12);
  P4EST_ASSERT (ta->elem_size == sizeof (p8est_edge_transform_t));

  ei->iedge = (int8_t) iedge;
  sc_array_resize (ta, 0);
  if (conn->num_edges == 0) {
    return;
  }
  aedge = conn->tree_to_edge[12 * itree + iedge];
  if (aedge == -1) {
    return;
  }

  /* identify touching faces */
  for (i = 0; i < 2; ++i) {
    faces[i] = p8est_edge_faces[iedge][i];
    ntrees[i] = conn->tree_to_tree[6 * itree + faces[i]];
    nfaces[i] = (int) conn->tree_to_face[6 * itree + faces[i]];
    if (ntrees[i] == itree && nfaces[i] == faces[i]) {  /* domain boundary */
      ntrees[i] = -1;
      nfaces[i] = orients[i] = -1;
    }
    else {
      orients[i] = nfaces[i] / 6;
      nfaces[i] %= 6;
      fcorners[i] = p8est_edge_face_corners[iedge][faces[i]];
      P4EST_ASSERT (fcorners[i][0] >= 0 && fcorners[i][1] >= 0);
    }
    founds[i] = false;
  }

  edge_trees =                  /* same type */
    conn->ett_offset[aedge + 1] - conn->ett_offset[aedge];

  /* find orientation of this edge */
  ietree = -1;
  iflip = -1;
  for (etree = 0; etree < edge_trees; ++etree) {
    ntree = conn->edge_to_tree[conn->ett_offset[aedge] + etree];
    redge = (int) conn->edge_to_edge[conn->ett_offset[aedge] + etree];
    P4EST_ASSERT (redge >= 0 && redge < 24);
    nedge = redge % 12;
    if (nedge == iedge && ntree == itree) {
      iflip = redge / 12;
      ietree = etree;
      break;
    }
  }
  P4EST_ASSERT (ietree >= 0 && iflip >= 0);

  /* loop through all trees connected through this edge */
  for (etree = 0; etree < edge_trees; ++etree) {
    if (etree == ietree) {
      continue;
    }
    ntree = conn->edge_to_tree[conn->ett_offset[aedge] + etree];
    redge = (int) conn->edge_to_edge[conn->ett_offset[aedge] + etree];
    P4EST_ASSERT (redge >= 0 && redge < 24);
    nedge = redge % 12;
    nflip = (redge / 12) ^ iflip;

    nows[0] = nows[1] = false;
    for (i = 0; i < 2; ++i) {
      if (ntree == ntrees[i]) {
        /* check if the edge touches this neighbor contact face */
        nfcorners = p8est_edge_face_corners[nedge][nfaces[i]];
        if (nfcorners[0] >= 0) {
          P4EST_ASSERT (fcorners[i][0] >= 0);
          pref = p8est_face_permutation_refs[faces[i]][nfaces[i]];
          pset = p8est_face_permutation_sets[pref][orients[i]];
          fc[0] = p8est_face_permutations[pset][fcorners[i][0]];
          fc[1] = p8est_face_permutations[pset][fcorners[i][1]];

          if (fc[0] == nfcorners[nflip] && fc[1] == nfcorners[!nflip]) {
            P4EST_ASSERT (!founds[i] && !nows[!i]);
            founds[i] = nows[i] = true;
          }
#ifdef P4EST_DEBUG
          else if (fc[0] == nfcorners[!nflip] && fc[1] == nfcorners[nflip]) {
            ++flipped;
          }
#endif
        }
      }
    }
    if (nows[0] || nows[1]) {
      continue;
    }

    /* else we have a true diagonal edge with ntree */
    et = sc_array_push (ta);
    et->ntree = ntree;
    et->nedge = (int8_t) nedge;
    et->naxis[0] = (int8_t) (nedge / 4);
    et->naxis[1] = (int8_t) (nedge < 4 ? 1 : 0);
    et->naxis[2] = (int8_t) (nedge < 8 ? 2 : 1);
    et->nflip = (int8_t) nflip;
    et->corners = (int8_t) (nedge % 4);
  }
  P4EST_ASSERT (edge_trees == (p4est_topidx_t) ta->elem_count
                + 1 + (ntrees[0] != -1) + (ntrees[1] != -1) - flipped);
}

void
p8est_find_corner_transform (p8est_connectivity_t * conn,
                             p4est_topidx_t itree, int icorner,
                             p8est_corner_info_t * ci)
{
  int                 i, edge_ignored;
  int                 ncorner;
  int                 iedge[3], iwhich[3];
  int                 iface[3], ncode, nface[3], orient[3], fcorner[3];
  int                 pref, pset, fc, nc;
  bool                omit;
  size_t              jz;
  p4est_topidx_t      aedge[3];
  p4est_topidx_t      corner_trees, ctree, nctree;
  p4est_topidx_t      acorner, ntree[3];
  p8est_edge_info_t   ei[3];
  sc_array_t         *eta[3];
  p8est_edge_transform_t *et;
  sc_array_t         *cta = &ci->corner_transforms;
  p8est_corner_transform_t *ct;

  P4EST_ASSERT (0 <= itree && itree < conn->num_trees);
  P4EST_ASSERT (0 <= icorner && icorner < 8);
  P4EST_ASSERT (cta->elem_size == sizeof (p8est_corner_transform_t));

  ci->icorner = (int8_t) icorner;
  sc_array_resize (cta, 0);
  if (conn->num_corners == 0) {
    return;
  }
  acorner = conn->tree_to_corner[8 * itree + icorner];
  if (acorner == -1) {
    return;
  }

  /* find the three face neighbors */
  for (i = 0; i < 3; ++i) {
    iface[i] = p8est_corner_faces[icorner][i];
    ntree[i] = conn->tree_to_tree[6 * itree + iface[i]];
    ncode = (int) conn->tree_to_face[6 * itree + iface[i]];
    if (ntree[i] == itree && ncode == iface[i]) {       /* domain boundary */
      ntree[i] = -1;
      nface[i] = -1;
      orient[i] = -1;
      fcorner[i] = -1;
    }
    else {
      nface[i] = ncode % 6;
      orient[i] = ncode / 6;
      fcorner[i] = p8est_corner_face_corners[icorner][iface[i]];
      P4EST_ASSERT (fcorner[i] >= 0);
    }
  }
  corner_trees =                /* same type */
    conn->ctt_offset[acorner + 1] - conn->ctt_offset[acorner];

  /* find the three edge transforms */
  if (conn->num_edges == 0) {
    eta[0] = eta[1] = eta[2] = NULL;
    aedge[0] = aedge[1] = aedge[2] = -1;
  }
  else {
    for (i = 0; i < 3; ++i) {
      iedge[i] = p8est_corner_edges[icorner][i];
      aedge[i] = conn->tree_to_edge[12 * itree + iedge[i]];
      if (aedge[i] == -1) {
        eta[i] = NULL;
        continue;
      }
      iwhich[i] = (p8est_edge_corners[iedge[i]][1] == icorner);
      P4EST_ASSERT (p8est_edge_corners[iedge[i]][iwhich[i]] == icorner);

      eta[i] = &ei[i].edge_transforms;
      sc_array_init (eta[i], sizeof (p8est_edge_transform_t));
      p8est_find_edge_transform (conn, itree, iedge[i], &ei[i]);
    }
  }

  /* collect all corners that are not from face or edge neighbors */
  edge_ignored = 0;
  for (ctree = 0; ctree < corner_trees; ++ctree) {
    nctree = conn->corner_to_tree[conn->ctt_offset[acorner] + ctree];
    ncorner = (int) conn->corner_to_corner[conn->ctt_offset[acorner] + ctree];
    if (ncorner == icorner && nctree == itree) {
      continue;
    }

    /* rule out face neighbors */
    omit = false;
    for (i = 0; i < 3; ++i) {
      if (nctree == ntree[i]) {
        P4EST_ASSERT (fcorner[i] >= 0);
        pref = p8est_face_permutation_refs[iface[i]][nface[i]];
        pset = p8est_face_permutation_sets[pref][orient[i]];
        fc = p8est_face_permutations[pset][fcorner[i]];
        nc = p8est_face_corners[nface[i]][fc];

        if (nc == ncorner) {
          omit = true;
          break;
        }
      }
    }
    if (omit)
      continue;

    /* rule out edge neighbors */
    omit = false;
    for (i = 0; i < 3; ++i) {
      if (aedge[i] == -1) {
        continue;
      }
      for (jz = 0; jz < eta[i]->elem_count; ++jz) {
        et = sc_array_index (eta[i], jz);
        if (nctree == et->ntree) {
          nc = p8est_edge_corners[et->nedge][et->nflip ^ iwhich[i]];

          if (nc == ncorner) {
            omit = true;
            break;
          }
        }
      }
      if (omit)
        break;
    }
    if (omit) {
      ++edge_ignored;
      continue;
    }

    /* else we have a true all-diagonal corner with ntree */
    ct = sc_array_push (cta);
    ct->ntree = nctree;
    ct->ncorner = (int8_t) ncorner;
  }
  P4EST_ASSERT (corner_trees == (p4est_topidx_t) cta->elem_count +
                1 + edge_ignored + (ntree[0] != -1) +
                (ntree[1] != -1) + (ntree[2] != -1));

  for (i = 0; i < 3; ++i) {
    if (aedge[i] >= 0) {
      sc_array_reset (eta[i]);
    }
  }
}

/* EOF p8est_connectivity.c */
