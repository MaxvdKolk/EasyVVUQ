#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <iostream>
#include <fstream>

void launch(double g, double m, double v0, double theta, double h0, double nu, double dt, double &out_dist, double &out_vx, double &out_vy)
{
	double x = 0.0, y = h0;
	double vx = v0 * sin(theta), vy = v0 * cos(theta);
	while (y > 0.0) {
		// Euler integrate
		vy -= g * m * dt;
		vx -= vx * nu * dt;
		vy -= vy * nu * dt;
		x += vx * dt;
		y += vy * dt;
		//std::cout << "pos: " << x << " " << y << " vel: " << vx << " " << vy << "\n";
	}

	out_dist = x;
	out_vx = vx;
	out_vy = vy;
}

void fail_if_not(std::string input, std::string correct)
{
	if(input.compare(correct)) {
		fprintf(stderr, "Error: Found '%s' where '%s' expected\n", input.c_str(), correct.c_str());
		exit(1);
	}
}

int main(int argc, char **argv)
{
	if (argc != 3) {
		fprintf(stderr, "Usage: canonsim INPUTSCRIPT OUTFNAME\n");
		return 1;
	}

	char *in_fname = argv[1];
	char *out_fname = argv[2];

	// Read in parameters from the input file
	std::ifstream infile(in_fname);
	if(infile.fail()) {
		fprintf(stderr, "Could not open %s for reading.\n", in_fname);
		exit(1);
	}

	double g, m, v0, theta, h0, nu, dt;
	std::string header, param, equals;
	
	// Check for correct header
	infile >> header;
	fail_if_not(header, "CANONSIM_INPUT_FILE:");

	// Read gravity, initial velocity, angle, height and air resistance (in that order)
	infile >> param >> equals >> g;	    fail_if_not(equals, "="); fail_if_not(param, "gravity"); param="";
	infile >> param >> equals >> m;    fail_if_not(equals, "="); fail_if_not(param, "mass"); param="";
	infile >> param >> equals >> v0;    fail_if_not(equals, "="); fail_if_not(param, "velocity"); param="";
	infile >> param >> equals >> theta; fail_if_not(equals, "="); fail_if_not(param, "angle"); param="";
	infile >> param >> equals >> h0;    fail_if_not(equals, "="); fail_if_not(param, "height"); param="";
	infile >> param >> equals >> nu;    fail_if_not(equals, "="); fail_if_not(param, "air_resistance"); param="";
	infile >> param >> equals >> dt;    fail_if_not(equals, "="); fail_if_not(param, "time_step"); param="";

	infile.close();

	//std::cout << header << " " << param << " " << equals << " " << g << " " << m << " " << v0 << " " << theta << " " << h0 << " " << nu << " " << dt << "\n";

	// Run sim
	double out_dist, out_vx, out_vy;
	launch(g, m, v0, theta, h0, nu, dt, out_dist, out_vx, out_vy);

	// Write distance travelled to output csv file
	std::ofstream outfile(out_fname);
	if(outfile.fail()) {
		fprintf(stderr, "Could not open %s for writing.\n", out_fname);
		exit(1);
	}

	outfile << "# Dist,lastvx,lastvy\n";
	outfile << out_dist << "," << out_vx << "," << out_vy << "\n";

	outfile.close();
}
