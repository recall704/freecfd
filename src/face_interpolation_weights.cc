/************************************************************************
	
	Copyright 2007-2010 Emre Sozer

	Contact: emresozer@freecfd.com

	This file is a part of Free CFD

	Free CFD is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    Free CFD is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    For a copy of the GNU General Public License,
    see <http://www.gnu.org/licenses/>.

*************************************************************************/
#include "grid.h"
#include "interpolate.h"
#include "inputs.h"
#include <iomanip>
using namespace std;

extern vector<Grid> grid;
extern InputFile input;

void face_interpolation_weights(int gid) {

	Interpolate interpolation; // Declare interpolation object
	
	interpolation.init();

	if (input.section("grid",0).subsection("interpolation").get_string("method")=="wtli") interpolation.method=WTLI;
	if (input.section("grid",0).subsection("interpolation").get_string("method")=="idw") interpolation.method=IDW;
	if (input.section("grid",0).subsection("interpolation").get_string("method")=="simple") interpolation.method=SIMPLE;
	
	interpolation.max_stencil_size=input.section("grid",gid).subsection("interpolation").get_int("stencilsize");
	interpolation.skewness_tolerance=input.section("grid",gid).subsection("interpolation").get_double("skewnesstolerance");
	interpolation.dimension=grid[gid].dimension;

	if (interpolation.max_stencil_size<1) { // Means auto based on dimension
		if (interpolation.dimension==3) interpolation.max_stencil_size=12;
		else if (interpolation.dimension==2) interpolation.max_stencil_size=6; 
		else if (interpolation.dimension==1) interpolation.max_stencil_size=2;
	}
	
	if (interpolation.method==IDW) interpolation.max_stencil_size=99;

	int tetra_intp_count=0;		
	int tri_intp_count=0;		
	int line_intp_count=0;		
	int point_intp_count=0;
	
	set<int> stencil;
	set<int>::iterator sit;
	// Loop all the faces
	bool is_internal=true;
	for (int f=0;f<grid[gid].faceCount;++f) {
		interpolation.point=grid[gid].face[f].centroid;
		int c=grid[gid].face[f].parent;
		int n=grid[gid].face[f].neighbor;
		bool extend_stencil=true;
		// Insert the parent and the neighbor cells directly to the interpolation stencil
		interpolation.stencil.push_back(grid[gid].cell[c].centroid);
		interpolation.stencil_indices.push_back(c);	
		interpolation.stencil.push_back(grid[gid].cell[n].centroid);
		interpolation.stencil_indices.push_back(n);
		// Parent and neighbor are in the stencil now
		if (interpolation.stencil.size()>=interpolation.max_stencil_size) extend_stencil=false;
	
		if (extend_stencil) {
			// Initialize stencil to nearest neighbor cells
			// Loop face nodes and their neighboring cells
			for (int nc=0;nc<grid[gid].cell[c].neighborCells.size();++nc) stencil.insert(grid[gid].cell[c].neighborCells[nc]);
			if (grid[gid].face[f].bc==INTERNAL_FACE) {
				c=grid[gid].face[f].neighbor;
				// Add neighbor cell's neighbors
				for (int nc=0;nc<grid[gid].cell[c].neighborCells.size();++nc) stencil.insert(grid[gid].cell[c].neighborCells[nc]);
			}
			// Eliminate parent and neighbor from stencil set (those were directly inserted into interpolation class stencil)
			stencil.erase(grid[gid].face[f].parent);
			stencil.erase(grid[gid].face[f].neighbor);

			for (sit=stencil.begin();sit!=stencil.end();sit++) {
				interpolation.stencil_indices.push_back(*sit);
				interpolation.stencil.push_back(grid[gid].cell[*sit].centroid);
			}
		}
		
		if (interpolation.method==WTLI) {
			
			interpolation.calculate_weights(is_internal);
			double weightSum=0.;
			for (int i=0;i<interpolation.weights.size();++i) {
				grid[gid].face[f].average.insert(pair<int,double>(interpolation.stencil_indices[i],interpolation.weights[i]));
				weightSum+=interpolation.weights[i];
			}

			if      (interpolation.kind==4) tetra_intp_count++;
			else if (interpolation.kind==3) tri_intp_count++; 
			else if (interpolation.kind==2) line_intp_count++;
			else if (interpolation.kind==1) point_intp_count++;
			
		} else if (interpolation.method==IDW) {
			// Loop the stencil
			vector<double> weights(interpolation.stencil.size(),0.);
			double weightSum=0.;
			for (int s=0;s<interpolation.stencil.size();++s) {
				weights[s]=fabs(grid[gid].face[f].centroid-interpolation.stencil[s]);
				weightSum+=weights[s];
			}		
			for (int s=0;s<interpolation.stencil.size();++s) weights[s]/=weightSum;
			for (int s=0;s<interpolation.stencil.size();++s) grid[gid].face[f].average.insert(pair<int,double>(interpolation.stencil_indices[s],weights[s])); 

		} else if (interpolation.method==SIMPLE) {		
	
			grid[gid].face[f].average.insert(pair<int,double>(interpolation.stencil_indices[0],0.5));
			grid[gid].face[f].average.insert(pair<int,double>(interpolation.stencil_indices[1],0.5));
		}
	
		interpolation.flush();
		stencil.clear();
	}
	
/*	
	cout << "[I rank=" << Rank << "] Face centers for which tetra interpolation method was used = " << tetra_intp_count << endl; 
	cout << "[I rank=" << Rank << "] Face centers for which tri interpolation method was used = " << tri_intp_count << endl; 
	cout << "[I rank=" << Rank << "] Face centers for which line interpolation method was used = " << line_intp_count << endl; 
	cout << "[I rank=" << Rank << "] Face centers for which point interpolation method was used = " << point_intp_count << endl;
*/	
	
	return;
}
