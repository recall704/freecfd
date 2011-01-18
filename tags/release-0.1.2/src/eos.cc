/************************************************************************
	
	Copyright 2007-2009 Emre Sozer & Patrick Clark Trizila

	Contact: emresozer@freecfd.com , ptrizila@freecfd.com

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
#include "commons.h"
#include "inputs.h"
extern InputFile input;

EOS::EOS() {
	;
}

void EOS::set() {
	if (input.section("fluidProperties").get_string("eos")=="idealGas") {
		type=IDEAL_GAS;
		R=UNIV_GAS_CONST/input.section("fluidProperties").get_double("molarMass");
	}
}

double EOS::rho (double p, double T) { 
	return (p+Pref)/(R*(T+Tref));
	
}

double EOS::p (double rho, double T) {
	return rho*R*(T+Tref)-Pref;
}

double EOS::T (double p, double rho) {
	return (p+Pref)/(R*rho)-Tref;
}