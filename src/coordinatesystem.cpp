//C-headers
#include <cmath>
//C++ headers
#include <limits>
#include <sstream>
//3rd-party headers
//own headers
#include <radolan/coordinatesystem.h>

#ifdef __cplusplus
namespace Radolan
{
#endif
	
// M_PI is not standard C++
#ifndef M_PI
	#define M_PI 3.14159265358979323846264338327950288
#endif
	
	const double RDCoordinateSystem::LAMBDA_0(10.0);
	const double RDCoordinateSystem::PHI_0(60.0);
	const double RDCoordinateSystem::R_EARTH( 6370.04f ); // in meters!
	const double RDCoordinateSystem::MESH_WIDTH(1.0);
	
	RDCoordinateSystem::RDCoordinateSystem(RDScanType type) 
	{
		m_scanType = type;
		updateGridInfo();
	}
	
	RDCoordinateSystem::~RDCoordinateSystem() 
	{
	}
	
	void RDCoordinateSystem::setScanType(RDScanType type)
	{
		if (m_scanType==type) return;
		m_scanType = type;
		updateGridInfo();
	}

	
	void RDCoordinateSystem::updateGridInfo()
	{
		switch (this->m_scanType) {
				
			case RD_EX:
			case RD_TH:
			case RD_TZ:
			{
				m_radolanGridCountHorizontal = 1400;
				m_radolanGridCountVertical = 1500;
				m_originGeographical = rdGeographicalPoint(9.0f,51.0f);
				m_offset = rdCartesianPoint(600.0f, 800.0f);
			} break;
				
			default:
			{
				m_originGeographical = rdGeographicalPoint(9.0f,51.0f);
				m_radolanGridCountHorizontal = 900;
				m_radolanGridCountVertical = 900;
				m_offset = rdCartesianPoint(450.0f, 450.0f);
			}
		}	
		
		// calculate the cartesian coordinates of the center
		m_originCartesian = cartesianCoordinate(m_originGeographical);
	}
	
	double RDCoordinateSystem::rad(double deg) 
	{
		return deg * M_PI / 180.0;
	}
	
	double RDCoordinateSystem::deg(double rad) 
	{
		return rad * 180.0 / M_PI;
	}
	
	double RDCoordinateSystem::polarStereographicScalingFactor(double phi0,double phi) {
		return (1+sin(phi0)) / (1+sin(phi));
	}
	
	RDCartesianPoint RDCoordinateSystem::cartesianCoordinate(RDGeographicalPointRad p)
	{
		// Bezugskoordinate im Bogenmass
		static double lambda0 = rad(LAMBDA_0); 	
		static double phi0 = rad(PHI_0); 	
		double M = polarStereographicScalingFactor(phi0,p.latitude);
		return rdCartesianPoint( R_EARTH * M * cos(p.latitude) * sin(p.longitude-lambda0),
							   - R_EARTH * M * cos(p.latitude) * cos(p.longitude-lambda0));
	}
	
	RDCartesianPoint RDCoordinateSystem::cartesianCoordinate(RDGeographicalPoint p)
	{
		RDGeographicalPointRad pRad = toRad(p);
		return cartesianCoordinate(pRad);
	}

	
	RDGridQuadrant RDCoordinateSystem::RDQuadrant(RDGridPoint p) 
	{
		bool isLeft = (p.ix - m_offset.x <= 0);
		
		bool isLower = (p.iy - m_offset.y <= 0);
		
		RDGridQuadrant result;
		
		if (isLeft && isLower) 
		{
			result = RDLowerLeft;
		}
		else if (isLeft && !isLower) 
		{
			result = RDUpperLeft;
		}
		else if (!isLeft && !isLower) 
		{
			result = RDUpperRight;
		}
		else
		{
			result = RDLowerRight;
		}
		return result;
	}
	
	RDCartesianPoint RDCoordinateSystem::cartesianCoordinate(RDGridPoint p)
	{
		// distance from origin
		double dx,dy;

		// find out what quadrant we're in:
		RDGridQuadrant quadrant = this->RDQuadrant(p);
		switch (quadrant) 
		{
			case RDLowerLeft:
				dx = p.ix - (m_offset.x - 1.0f) - 1.0f;
				dy = p.iy - (m_offset.y - 1.0f) - 1.0f;
				break;
			case RDUpperLeft:
				dx = p.ix - (m_offset.x - 1.0f) - 1.0f;
				dy = p.iy - (m_offset.y - 1.0f);
				break;
			case RDUpperRight:
				dx = p.ix - (m_offset.x - 1.0f);
				dy = p.iy - (m_offset.y - 1.0f);
				break;
			case RDLowerRight:
				dx = p.ix - (m_offset.x - 1.0f);
				dy = p.iy - (m_offset.y - 1.0f) - 1.0f;
				break;
		}
		
		// to get the real distance, multiply with mesh width
		dx *= MESH_WIDTH;
		dy *= MESH_WIDTH;

		return rdCartesianPoint(m_originCartesian.x + dx,m_originCartesian.y + dy);
	}
	
	RDGeographicalPointRad RDCoordinateSystem::geographicalCoordinateRad(RDCartesianPoint p)
	{
		static double b_lambda_0 = rad(LAMBDA_0); 	
		static double b_phi_0 = rad(PHI_0);	
		RDGeographicalPointRad pg;
		pg.longitude = b_lambda_0 + atan(-p.x/p.y);
		double omphi = R_EARTH * R_EARTH * (1+sin(b_phi_0)) * (1+sin(b_phi_0));
		double xy = (p.x*p.x+p.y*p.y);
		pg.latitude = asin((omphi-xy)/(omphi+xy));
		return pg;
	}
	
	RDGeographicalPoint RDCoordinateSystem::geographicalCoordinate(RDCartesianPoint p) 
	{
		RDGeographicalPointRad pg = geographicalCoordinateRad(p);
		return toDeg(pg);
	}
	
	RDGeographicalPointRad RDCoordinateSystem::geographicalCoordinateRad(RDGridPoint p)	
	{
		RDCartesianPoint pps = cartesianCoordinate(p);
		return geographicalCoordinateRad(pps);
	}
	
	RDGeographicalPoint RDCoordinateSystem::geographicalCoordinate(RDGridPoint p)	
	{
		RDCartesianPoint pps = cartesianCoordinate(p);
		return geographicalCoordinate(pps);
	}
	
	RDGeographicalPoint* RDCoordinateSystem::geographicalPolygonForGridpoint(RDGridPoint p, int& count)
	{
		count=4;
		RDCartesianPoint pc = cartesianCoordinate(p);
		
		RDGeographicalPoint* poly = (RDGeographicalPoint*)calloc(sizeof(RDGeographicalPoint),count);
		if (poly==NULL) {
			fprintf(stderr, "ERROR:geographicalPolygonForGridpoint:could not allocate memory");
			count = 0;
			return NULL;
		}
		
		// TODO needs fixing
		RDGridQuadrant quadrant = this->RDQuadrant(p);
		switch (quadrant) 
		{
			case RDLowerLeft:
                
				poly[0] = geographicalCoordinate(pc);
				poly[1] = geographicalCoordinate(rdCartesianPoint(pc.x, pc.y+1.0f));
				poly[2] = geographicalCoordinate(rdCartesianPoint(pc.x+1.0f, pc.y+1.0f));
				poly[3] = geographicalCoordinate(rdCartesianPoint(pc.x+1.0f, pc.y));
				break;
                
			case RDUpperLeft:
				
				poly[0] = geographicalCoordinate(rdCartesianPoint(pc.x, pc.y-1.0f));
				poly[1] = geographicalCoordinate(pc);
				poly[2] = geographicalCoordinate(rdCartesianPoint(pc.x+1.0f, pc.y));
				poly[3] = geographicalCoordinate(rdCartesianPoint(pc.x+1.0f, pc.y-1.0f));
				break;
				
			case RDUpperRight:
				
				poly[0] = geographicalCoordinate(rdCartesianPoint(pc.x-1.0f, pc.y-1.0f));
				poly[1] = geographicalCoordinate(rdCartesianPoint(pc.x-1.0f, pc.y));
				poly[2] = geographicalCoordinate(pc);
				poly[3] = geographicalCoordinate(rdCartesianPoint(pc.x, pc.y-1.0f));
				break;
				
			case RDLowerRight:

				poly[0] = geographicalCoordinate(rdCartesianPoint(pc.x-1.0f, pc.y));
				poly[1] = geographicalCoordinate(rdCartesianPoint(pc.x-1.0f, pc.y-1.0));
				poly[2] = geographicalCoordinate(rdCartesianPoint(pc.x, pc.y-1.0f));
				poly[3] = geographicalCoordinate(pc);
				break;
		}
		
		return poly;
	}
	
	RDGridPoint RDCoordinateSystem::gridPoint(RDGeographicalPointRad g, bool& isInside)
	{
		RDCartesianPoint cp = cartesianCoordinate(g);
		RDGridPoint gp = gridPoint(cp, isInside);
		return gp;
	}
	
	RDGridPoint RDCoordinateSystem::gridPoint(RDGeographicalPoint g, bool& isInside)
	{
		RDCartesianPoint cp = cartesianCoordinate(g);
		RDGridPoint gp = gridPoint(cp, isInside);
		return gp;
	}

	RDGridPoint RDCoordinateSystem::gridPoint(RDCartesianPoint g, bool& isInside)
	{
		// calculate the distance of the point from the origin
		double dx = g.x - m_originCartesian.x;
		double dy = g.y - m_originCartesian.y;
		
		// quadrant?
		RDGridQuadrant quadrant = RDUpperRight;
        
		if (dx<0.0 && dy<0.0)
        {
			quadrant = RDLowerLeft;
        }
		else if (dx<0.0 && dy>=0.0)
        {
			quadrant = RDUpperLeft;
        }
		else if (dx>=0.0 && dy>=0.0)
        {
			quadrant = RDUpperRight;
        }
		else if (dx>=0.0 && dy<0.0)
        {
			quadrant = RDLowerRight;
        }
		
		// count
		RDGridPoint p;
		switch (quadrant) 
		{
			case RDLowerLeft:
				p = rdGridPoint((int)(floor(dx) + m_offset.x),
								(int)(floor(dy) + m_offset.y));
				break;
			case RDUpperLeft:
				p = rdGridPoint((int)(floor(dx) + m_offset.x),
								(int)(ceil(dy) + m_offset.y - 1.0f));
				break;
			case RDUpperRight:
				p = rdGridPoint((int)(ceil(dx) + m_offset.x - 1.0f),
								(int)(ceil(dy) + m_offset.y - 1.0f));
				break;
			case RDLowerRight:
				p = rdGridPoint((int)(floor(dx) + m_offset.x - 1.0f),
								(int)(floor(dy) + m_offset.y));
		}

		if (p.ix>=0  && p.iy>=0 && p.ix < m_radolanGridCountHorizontal && p.iy < m_radolanGridCountVertical)
		{
			isInside = true;
		} else {
			isInside = false;
		}
		
		return p;
	}
	
	RDGeographicalPointRad RDCoordinateSystem::toRad(RDGeographicalPoint p)
	{
		return rdGeographicalPointRad(rad(p.longitude),rad(p.latitude));
	}
	
	RDGeographicalPoint RDCoordinateSystem::toDeg(RDGeographicalPointRad p)
	{
		return rdGeographicalPoint(deg(p.longitude),deg(p.latitude));
	}
	
#ifdef __cplusplus
}
#endif
