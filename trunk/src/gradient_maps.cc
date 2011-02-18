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
#include "inputs.h"

using namespace std;

extern vector<Grid> grid;
extern InputFile input;
extern void lsqr_grad_map(int gid,int c);
extern void curvilinear_grad_map(int gid,int c);

void gradient_maps(int gid) {

	for (int c=0;c<grid[gid].cellCount;++c) {
		if (grid[gid].cell[c].nodeCount==8) curvilinear_grad_map(gid,c);
		else lsqr_grad_map(gid,c);
	}
	
	return;
}
