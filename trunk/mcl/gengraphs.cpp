#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include "MCL.h"
#include "DRE.h"
#include "LVR.h"
#include "MCLDist.h"

using namespace std;
using namespace mcl;
using num::Number;

double mclDeviation(const MCL & a_mcl) //{{{1 ///////////////////////
{
	double dw = 0;
	MCL::const_iterator s;
	for (s = a_mcl.begin(); s != a_mcl.end(); s++)
		dw += fabs(toDouble(s->w())-1);
	return dw/double(a_mcl.end() - a_mcl.begin());
}
struct Line //{{{1 //////////////////////////////////////////////////
{
	int numsamp;
	double deviation;
	double dist_dre;
	double dist_lvr;
	Line(int n, double dv, double dre, double lvr)
		: numsamp(n), deviation(dv), dist_dre(dre), dist_lvr(lvr) {}
	friend ostream & operator << (ostream & out, const Line & line)
	{
		out << setprecision(0) << setw(6) << line.numsamp
			<< setprecision(4) << setw(8) << line.deviation
			<< setprecision(6) << setw(14) << resetiosflags(ios::fixed) << line.dist_dre
			<< setprecision(8) << setw(14) << setiosflags(ios::fixed) << line.dist_lvr 
			<< endl;
		return out;
	}
};

void generateGraphs() //{{{1 ////////////////////////////////////////
{
	int numavg = 60;
	Number numdev;
	Rnd rnd;
	//Normal rnd;
	int numsamp;
	cout << setiosflags(ios::fixed | ios::right) << setprecision(12);
	//cout << endl << setw(6) << "samp" << setw(8) << "dev" << 
	//	setw(12) << "DRE" << setw(12) << "LVR" << endl;
	ofstream log("_log.dat"); 

	typedef vector< vector<Line> > tMatrix;
	tMatrix matrix;
	for (numdev = Number(16,10); numdev >= 1; numdev -= Number(1,50))
	{	
		matrix.push_back(vector<Line>());
		vector<Line> & buff = matrix.back();
		double sumdeviation = 0;
		for (numsamp = 10; numsamp < 500; numsamp += 10)
		{
			double dist_dre = 0;
			double dist_lvr = 0;
			double deviation = 0;
			
			MCL mcl_before(numsamp);
			DRE resampleDRE;
			LVR resampleLVR;
			for (int k = 0; k < numavg; k++)
			{
				//cout << numsamp << '\t' << rnd << endl;
				mcl_before.init(num::Pose(), rnd);
				for (MCL::iterator s = mcl_before.begin(); s != mcl_before.end(); s++)
					s->w() = rnd() + numdev;
				mcl_before.normalize();
				deviation += mclDeviation(mcl_before);
				
				MCL mcl_dre(mcl_before);
				resampleDRE(mcl_dre);
				mcl_dre.normalize();
				dist_dre += mclDist(mcl_before, mcl_dre);
				
				MCL mcl_lvr(mcl_before);
				resampleLVR(mcl_lvr, rnd);
				dist_lvr += mclDist(mcl_before, mcl_lvr);
			}
	
			dist_dre /= numavg;
			dist_lvr /= numavg;
			deviation /= numavg;
			sumdeviation += deviation;

			buff.push_back(Line(numsamp, deviation, dist_dre, dist_lvr));
			//cout << numsamp << '\t' << rnd << endl;
		}

		sumdeviation /= buff.size();
		for (vector<Line>::iterator l = buff.begin(); l != buff.end(); l++)
			l->deviation = sumdeviation;
		
		cout << sumdeviation << '\t' << rnd << endl;		
	}

	
	size_t col = 0, row = 0;
	Line line00(matrix[row][col].numsamp, matrix[row][col].deviation,
				(matrix[row+0][col].dist_dre + matrix[row+0][col+1].dist_dre +
				 matrix[row+1][col].dist_dre + matrix[row+1][col+1].dist_dre)/4,
				(matrix[row+0][col].dist_lvr + matrix[row+0][col+1].dist_lvr +
				 matrix[row+1][col].dist_lvr + matrix[row+1][col+1].dist_lvr)/4);
	//cout << line00;
	log << line00;
	
	for (col = 1; col < matrix[row].size()-1; col++)
	{
		Line line(matrix[row][col].numsamp, matrix[row][col].deviation,
					(matrix[row+0][col-1].dist_dre + matrix[row+0][col].dist_dre + matrix[row+0][col+1].dist_dre +
					 matrix[row+1][col-1].dist_dre + matrix[row+1][col].dist_dre + matrix[row+1][col+1].dist_dre)/6,
					(matrix[row+0][col-1].dist_lvr + matrix[row+0][col].dist_lvr + matrix[row+0][col+1].dist_lvr +
					 matrix[row+1][col-1].dist_lvr + matrix[row+1][col].dist_lvr + matrix[row+1][col+1].dist_lvr)/6);
		//cout << line;
		log << line;
	}
	
	Line line01(matrix[row][col].numsamp, matrix[row][col].deviation,
				(matrix[row+0][col-1].dist_dre + matrix[row+0][col].dist_dre +
				 matrix[row+1][col-1].dist_dre + matrix[row+1][col].dist_dre)/4,
				(matrix[row+0][col-1].dist_lvr + matrix[row+0][col].dist_lvr + 
				 matrix[row+1][col-1].dist_lvr + matrix[row+1][col].dist_lvr)/4);
	//cout << line01 << endl;
	log << line01 << endl;
	
	for (row = 1; row != matrix.size()-1; row++)
	{
		col = 0;
		Line first(matrix[row][col].numsamp, matrix[row][col].deviation,
					(matrix[row-1][col].dist_dre + matrix[row-1][col+1].dist_dre +
					 matrix[row+0][col].dist_dre + matrix[row+0][col+1].dist_dre +
					 matrix[row+1][col].dist_dre + matrix[row+1][col+1].dist_dre)/6,
					(matrix[row-1][col].dist_lvr + matrix[row-1][col+1].dist_lvr +
					 matrix[row+0][col].dist_lvr + matrix[row+0][col+1].dist_lvr +
					 matrix[row+1][col].dist_lvr + matrix[row+1][col+1].dist_lvr)/6);
		//cout << first;
		log << first;

		for (col = 1; col < matrix[row].size()-1; col++)
		{
			Line line(matrix[row][col].numsamp, matrix[row][col].deviation,
						(matrix[row-1][col-1].dist_dre + matrix[row-1][col].dist_dre + matrix[row-1][col+1].dist_dre +
						 matrix[row+0][col-1].dist_dre + matrix[row+0][col].dist_dre + matrix[row+0][col+1].dist_dre +
						 matrix[row+1][col-1].dist_dre + matrix[row+1][col].dist_dre + matrix[row+1][col+1].dist_dre)/9,
						(matrix[row-1][col-1].dist_lvr + matrix[row-1][col].dist_lvr + matrix[row-1][col+1].dist_lvr +
						 matrix[row+0][col-1].dist_lvr + matrix[row+0][col].dist_lvr + matrix[row+0][col+1].dist_lvr +
						 matrix[row+1][col-1].dist_lvr + matrix[row+1][col].dist_lvr + matrix[row+1][col+1].dist_lvr)/9);
			//cout << line;
			log << line;
		}

		Line line(matrix[row][col].numsamp, matrix[row][col].deviation,
					(matrix[row-1][col-1].dist_dre + matrix[row-1][col].dist_dre +
					 matrix[row+0][col-1].dist_dre + matrix[row+0][col].dist_dre +
					 matrix[row+1][col-1].dist_dre + matrix[row+1][col].dist_dre)/6,
					(matrix[row-1][col-1].dist_lvr + matrix[row-1][col].dist_lvr +
					 matrix[row+0][col-1].dist_lvr + matrix[row+0][col].dist_lvr + 
					 matrix[row+1][col-1].dist_lvr + matrix[row+1][col].dist_lvr)/6);
		//cout << line << endl;
		log << line << endl;
	}

	col = 0;
	Line line10(matrix[row][col].numsamp, matrix[row][col].deviation,
				(matrix[row-1][col].dist_dre + matrix[row-1][col+1].dist_dre +
				 matrix[row+0][col].dist_dre + matrix[row+0][col+1].dist_dre)/4,
				(matrix[row-1][col].dist_lvr + matrix[row-1][col+1].dist_lvr +
				 matrix[row+0][col].dist_lvr + matrix[row+0][col+1].dist_lvr)/4);
	//cout << line10;
	log << line10;

	for (col = 1; col < matrix[row].size()-1; col++)
	{
		Line line(matrix[row][col].numsamp, matrix[row][col].deviation,
					(matrix[row-1][col-1].dist_dre + matrix[row-1][col].dist_dre + matrix[row-1][col+1].dist_dre +
					 matrix[row+0][col-1].dist_dre + matrix[row+0][col].dist_dre + matrix[row+0][col+1].dist_dre)/6,
					(matrix[row-1][col-1].dist_lvr + matrix[row-1][col].dist_lvr + matrix[row-1][col+1].dist_lvr +
					 matrix[row+0][col-1].dist_lvr + matrix[row+0][col].dist_lvr + matrix[row+0][col+1].dist_lvr)/6);
		//cout << line;
		log << line;
	}

	Line line11(matrix[row][col].numsamp, matrix[row][col].deviation,
				(matrix[row-1][col-1].dist_dre + matrix[row-1][col].dist_dre +
				 matrix[row+0][col-1].dist_dre + matrix[row+0][col].dist_dre)/4,
				(matrix[row-1][col-1].dist_lvr + matrix[row-1][col].dist_lvr +
				 matrix[row+0][col-1].dist_lvr + matrix[row+0][col].dist_lvr)/4);
	//cout << line11 << endl;
	log << line11 << endl;
}

int main() //{{{1 ///////////////////////////////////////////////////
{
	generateGraphs();
	return 0;
}
