// Copyright (c) 2006-2007  INRIA Sophia-Antipolis (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you may redistribute it under
// the terms of the Q Public License version 1.0.
// See the file LICENSE.QPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL$
// $Id$
//
// Author(s)     : Laurent RINEAU

#ifndef CGAL_POLYHEDRAL_SURFACE_3_H
#define CGAL_POLYHEDRAL_SURFACE_3_H

#include <CGAL/Polyhedron_3.h>
#include <CGAL/IO/Polyhedron_iostream.h>

#include <CGAL/make_surface_mesh.h>
#include <CGAL/Data_structure_using_octree_3.h>
#include <CGAL/Surface_mesher/Polyhedral_oracle.h>
#include <iostream>
#include <vector>

#ifdef CGAL_SURFACE_MESHER_DEBUG_POLYHEDRAL_SURFACE_CONSTRUCTION
#include <CGAL/Timer.h>
#endif

namespace CGAL {

template <class GT>
class Polyhedral_surface_3
{
public:
  typedef GT Geom_traits;

  class Normalized_geom_traits : public Geom_traits 
  {
  public:
    typedef typename 
    Kernel_traits<typename Geom_traits::Point_3>::Kernel::Point_3 Point_3;
  };

  typedef Data_structure_using_octree_3<Normalized_geom_traits> Subfacets_octree;
  typedef Data_structure_using_octree_3<Normalized_geom_traits> Subsegments_octree;
  typedef typename GT::Point_3 Point_3;

  typedef Polyhedral_surface_3<GT> Self;

  typedef Surface_mesher::Polyhedral_oracle<Self> Surface_mesher_traits_3;

  typedef typename Subfacets_octree::Bbox Bbox;

  Polyhedral_surface_3(std::istream& input_file)
    : subfacets_octree(), subsegments_octree(false, true, false),
      input_points()
  {
    typedef CGAL::Polyhedron_3<GT> Polyhedron_3;
    Polyhedron_3 polyhedron;

#ifdef CGAL_SURFACE_MESHER_DEBUG_POLYHEDRAL_SURFACE_CONSTRUCTION
    CGAL::Timer timer;
    std::cerr << "Creating polyhedron...";
    timer.start();
#endif
    input_file >> polyhedron;
#ifdef CGAL_SURFACE_MESHER_DEBUG_POLYHEDRAL_SURFACE_CONSTRUCTION
    timer.stop();
    std::cerr << "done (" << timer.time() << "s)\n";
#endif

    input_points.reserve(polyhedron.size_of_vertices());

    for(typename Polyhedron_3::Vertex_const_iterator vit = 
          polyhedron.vertices_begin();
        vit != polyhedron.vertices_end();
        ++vit)
    {
      subfacets_octree.add_constrained_vertex(vit->point());
      input_points.push_back(vit->point());
    }

    for(typename Polyhedron_3::Facet_const_iterator fit = 
          polyhedron.facets_begin();
        fit != polyhedron.facets_end();
        ++fit)
    {
      CGAL_assertion(fit->is_triangle());
      typename Polyhedron_3::Halfedge_around_facet_const_circulator 
        edges_circ = fit->facet_begin();

      const Point_3& p1 = edges_circ++->vertex()->point();
      const Point_3& p2 = edges_circ++->vertex()->point();
      const Point_3& p3 = edges_circ++->vertex()->point();

      subfacets_octree.add_constrained_facet(p1, p2, p3);
    }

#ifdef CGAL_SURFACE_MESHER_DEBUG_POLYHEDRAL_SURFACE_CONSTRUCTION
    std::cerr << "Creating octree...";
    timer.reset();
    timer.start();
#endif
    subfacets_octree.create_data_structure();
#ifdef CGAL_SURFACE_MESHER_DEBUG_POLYHEDRAL_SURFACE_CONSTRUCTION
    timer.stop();
    std::cerr << "done (" << timer.time() << "s)\n";
#endif
    
//     subfacets_octree.input(input_file,
//                            std::back_inserter(input_points));
  }

  Bbox bbox() const
  {
    return subfacets_octree.bbox();
  }

public:
  Subfacets_octree subfacets_octree;
  Subsegments_octree subsegments_octree;
  std::vector<Point_3> input_points;
};

} // end namespace CGAL

#endif // CGAL_POLYHEDRAL_SURFACE_3_H
