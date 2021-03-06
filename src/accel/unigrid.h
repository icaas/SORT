/*
    This file is a part of SORT(Simple Open Ray Tracing), an open-source cross
    platform physically based renderer.
 
    Copyright (c) 2011-2019 by Cao Jiayin - All rights reserved.
 
    SORT is a free software written for educational purpose. Anyone can distribute
    or modify it under the the terms of the GNU General Public License Version 3 as
    published by the Free Software Foundation. However, there is NO warranty that
    all components are functional in a perfect manner. Without even the implied
    warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    General Public License for more details.
 
    You should have received a copy of the GNU General Public License along with
    this program. If not, see <http://www.gnu.org/licenses/gpl-3.0.html>.
 */

#pragma once

#include "accelerator.h"

//! @brief Uniform Grid.
/**
 * Uniform grid is the simplest spatial acceleration structure in a ray tracer.
 * Unlike other complex data structure, like KD-Tree, uniform grid takes linear
 * time complexity to build. However the traversal efficiency may be lower than
 * its peers.
 */
class UniGrid : public Accelerator{
public:
    DEFINE_RTTI( UniGrid , Accelerator );

    //! @brief      Get intersection between the ray and the primitive set using KD-Tree.
    //!
    //! It will return true if there is intersection between the ray and the primitive 
    //! set. In case of an existed intersection, if intersect is not empty, it will fill 
    //! the structure and return the nearest intersection.
    //! If intersect is nullptr, it will stop as long as one intersection is found, it 
    //! is not necessary to be the nearest one.
    //!
    //! @param r            The input ray to be tested.
    //! @param intersect    The intersection result. If a nullptr pointer is provided, 
    //!                     it stops as long as it finds an intersection. It is faster 
    //!                     than the one with intersection information data and suitable 
    //!                     for shadow ray calculation.
    //! @return             It will return true if there is an intersection, otherwise 
    //!                     it returns false.
    bool GetIntersect( const Ray& r , Intersection* intersect ) const override;

	//! Build uniform grid structure in O(N).
    //!
    //! @param scene    The rendering scene.
	void    Build(const Scene& scene) override;
    
    //! @brief      Serializing data from stream
    //!
    //! @param      Stream where the serialization data comes from. Depending on different 
    //!             situation, it could come from different places.
    void    Serialize( IStreamBase& stream ) override{
        // to be implemented
    }

private:
    /**< Total number of voxels. */
	unsigned	                                m_voxelCount = 0;
    /**< Number of voxels along each axis. */
    unsigned	                                m_voxelNum[3] = {};
    /**< Extent of one voxel along each axis. */
	Vector		                                m_voxelExtent;
    /**< Inverse of extent of one voxel along each axis. */
	Vector		                                m_voxelInvExtent;
    /**< Vector holding all voxels. */
	std::vector<std::vector<Primitive*>>	    m_voxels;

	//! @brief      Locate the id of the voxel that the point belongs to along a specific axis.
    //!
    //! @param p        The point to be evaluated.
    //! @param axis     The id of axis to be tested along.
    //! @return         The id of the voxel along the selected axis.
	unsigned point2VoxelId( const Point& p , unsigned axis ) const;
    
	//! @brief      Translate voxel id from three-dimensional to one-dimentional.
    //! @param x        ID of voxel along axis-x.
    //! @param y        ID of voxel along axis-y.
    //! @param z        ID of voxel along axis-z.
    //! @return         ID of the voxel in one single dimension.
	unsigned offset( unsigned x , unsigned y , unsigned z ) const;
	
    //! @brief      Get the nearest intersection between a ray and the primitive set.
    //! @param r            The ray to be tested.
    //! @param intersect    A pointer to the intersection information. If it is empty, it will return 
    //!                     true as long as there is an intersection detected, which is not necessarily 
    //!                     the nearest one.
    //! @param voxelId      ID of the voxel to be tested.
    //! @param nextT        The intersected position of the ray and the next to-be-traversed voxel along 
    //!                     the ray.
    //! @return             It will return true if there is an intersection, otherwise it returns false.
	bool getIntersect( const Ray& r , Intersection* intersect , unsigned voxelId , float nextT ) const;
    
    SORT_STATS_ENABLE( "Spatial-Structure(UniformGrid)" )
};
