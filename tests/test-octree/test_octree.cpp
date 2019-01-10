#include "bbox.h"
#include "mesh_face_bbox.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <OpenMesh/Core/Geometry/Vector11T.hh>
#include <OpenMesh/Core/Mesh/Traits.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>


#include <algorithm>

using testing::Eq;
using testing::FloatEq;

using MeshT = OpenMesh::TriMesh_ArrayKernelT<OpenMesh::DefaultTraits>;




TEST( meshFaceAndBBox, computeCorrectBoxes )
{
    //

    MeshT mesh1;
    MeshT mesh2;

    std::array<MeshT::VertexHandle, 4> vhandleMesh1;
    std::array<MeshT::VertexHandle, 4> vhandleMesh2;

    vhandleMesh1[ 0 ] = mesh1.add_vertex( MeshT::Point{0., 0., 0.} );
    vhandleMesh1[ 1 ] = mesh1.add_vertex( MeshT::Point{4., 2., 0.} );
    vhandleMesh1[ 2 ] = mesh1.add_vertex( MeshT::Point{3., 6., 0.} );
    vhandleMesh1[ 3 ] = mesh1.add_vertex( MeshT::Point{0., 4., 0.} );


    vhandleMesh2[ 0 ] = mesh2.add_vertex( MeshT::Point{3., 1., 0.} );
    vhandleMesh2[ 1 ] = mesh2.add_vertex( MeshT::Point{5., 3., 0.} );
    vhandleMesh2[ 2 ] = mesh2.add_vertex( MeshT::Point{8., 4., 0.} );
    vhandleMesh2[ 3 ] = mesh2.add_vertex( MeshT::Point{0., 6., 0.} );

    std::vector<MeshT::VertexHandle> faceVhandles;
    faceVhandles.resize( 4 );
    std::copy( std::begin( vhandleMesh1 ), std::end( vhandleMesh1 ), std::begin( faceVhandles ) );

    mesh1.add_face( faceVhandles );

    std::copy( std::begin( vhandleMesh2 ), std::end( vhandleMesh2 ), std::begin( faceVhandles ) );

    mesh2.add_face( faceVhandles );


    auto faceAndBBox1 = MeshFaceAndBBox{mesh1};
    auto faceAndBBox2 = MeshFaceAndBBox{mesh2};

    auto const& mesh1box = faceAndBBox1.meshBBox;

    auto const& mesh2box = faceAndBBox2.meshBBox;

    EXPECT_DOUBLE_EQ( 0., mesh1box.lower()[ 0 ] );
    EXPECT_DOUBLE_EQ( 0., mesh1box.lower()[ 1 ] );
    EXPECT_DOUBLE_EQ( 0., mesh1box.lower()[ 2 ] );

    EXPECT_DOUBLE_EQ( 4., mesh1box.upper()[ 0 ] );
    EXPECT_DOUBLE_EQ( 6., mesh1box.upper()[ 1 ] );
    EXPECT_DOUBLE_EQ( 0., mesh1box.upper()[ 2 ] );


    ASSERT_TRUE( intersect( mesh1box, mesh2box ) );

    auto intersectionBox = bboxIntersect( mesh1box, mesh2box );


    auto computeInAndOut
        = [&intersectionBox]( auto meshinfirst, auto meshoutfirst, auto const& faceAndBBox ) {
              auto const& mesh = faceAndBBox.mesh;
              std::partition_copy( mesh.faces_begin(), mesh.faces_end(), meshinfirst, meshoutfirst,
                                   [&faceAndBBox, &intersectionBox]( auto const& fh ) //
                                   {
                                       auto const& faceToBBox = faceAndBBox.faceToBBox;

                                       auto fIt = faceToBBox.find( fh );
                                       if ( fIt != std::end( faceToBBox ) )
                                       {
                                           return intersect( fIt->second, intersectionBox );
                                       }
                                       else
                                       {
                                           auto bbox = bboxFromFace( faceAndBBox.mesh, fh );
                                           return intersect( bbox, intersectionBox );
                                       }
                                   } );
          };

    using FaceHandle = MeshT::FaceHandle;

    std::vector<FaceHandle> mesh1in;
    std::vector<FaceHandle> mesh1out;

    computeInAndOut( std::back_inserter( mesh1in ), std::back_inserter( mesh1out ), faceAndBBox1 );

    std::vector<FaceHandle> mesh2in;
    std::vector<FaceHandle> mesh2out;

    computeInAndOut( std::back_inserter( mesh2in ), std::back_inserter( mesh2out ), faceAndBBox2 );
}
