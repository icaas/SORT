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

#include "rectangle.h"
#include "sampler/sample.h"
#include "core/samplemethod.h"

Point Rectangle::Sample_l( const LightSample& ls , const Point& p , Vector& wi , Vector& n , float* pdf ) const{
    const float halfx = sizeX * 0.5f;
    const float halfy = sizeY * 0.5f;
    
	float u = 2 * ls.u - 1.0f;
	float v = 2 * ls.v - 1.0f;
	Point lp = m_transform( Point( halfx * u , 0.0f , halfy * v ) );
	n = m_transform( Vector( 0 , 1 , 0 ) );
	Vector delta = lp - p;
	wi = Normalize( delta );

	float dot = Dot( -wi , n );
	if( pdf ) 
	{
		if( dot <= 0 )
			*pdf = 0.0f;
		else
			*pdf = delta.SquaredLength() / ( SurfaceArea() * dot );
	}

	return lp;
}

void Rectangle::Sample_l( const LightSample& ls , Ray& r , Vector& n , float* pdf ) const{
    const auto halfx = sizeX * 0.5f;
    const auto halfy = sizeY * 0.5f;
    
	const auto u = 2 * ls.u - 1.0f;
	const auto v = 2 * ls.v - 1.0f;
	r.m_fMin = 0.0f;
	r.m_fMax = FLT_MAX;
	r.m_Ori = m_transform( Point( halfx * u , 0.0f , halfy * v ) );
	r.m_Dir = m_transform( UniformSampleHemisphere( sort_canonical() , sort_canonical() ) );
	n = m_transform.invMatrix.Transpose()( DIR_UP );

	if( pdf ) 
		*pdf = UniformHemispherePdf() / SurfaceArea();
}

float Rectangle::SurfaceArea() const{
    return sizeX * sizeY;
}

bool Rectangle::GetIntersect( const Ray& r , Intersection* intersect ) const{
	const auto ray = m_transform.invMatrix( r );
	if( ray.m_Dir.y == 0.0f )
		return false;

	const auto limit = intersect ? intersect->t : FLT_MAX;
	const auto t = -ray.m_Ori.y / ray.m_Dir.y;
	if( t > limit || t <= ray.m_fMin || t > ray.m_fMax )
		return false;

	const auto p = ray(t);
    const auto halfx = sizeX * 0.5f;
    const auto halfy = sizeY * 0.5f;
	if( p.x > halfx || p.x < -halfx )
		return false;
	if( p.z > halfy || p.z < -halfy )
		return false;

	if( intersect ){
		intersect->t = t;
		intersect->intersect = r(t);
		intersect->normal = m_transform.invMatrix.Transpose()(DIR_UP);
		intersect->gnormal = intersect->normal;
		intersect->tangent = m_transform(Vector( 0.0f , 0.0f , 1.0f ));
	}

	return true;
}

const BBox&	Rectangle::GetBBox() const{
    const auto halfx = sizeX * 0.5f;
    const auto halfy = sizeY * 0.5f;
	if( !m_bbox ){
        m_bbox = std::make_unique<BBox>();
		m_bbox->Union( m_transform( Point( halfx , 0.0f , halfy ) ) );
		m_bbox->Union( m_transform( Point( halfx , 0.0f , -halfy ) ) );
		m_bbox->Union( m_transform( Point( -halfx , 0.0f , halfy ) ) );
		m_bbox->Union( m_transform( Point( -halfx , 0.0f , -halfy ) ) );
	}

	return *m_bbox;
}
