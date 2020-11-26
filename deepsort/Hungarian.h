// Modified based on https://github.com/mcximing/sort-cpp

///////////////////////////////////////////////////////////////////////////////
// Hungarian.h: Header file for Class HungarianAlgorithm.
//
// This is a C++ wrapper with slight modification of a hungarian algorithm implementation by Markus Buehren.
// The original implementation is a few mex-functions for use in MATLAB, found here:
// http://www.mathworks.com/matlabcentral/fileexchange/6543-functions-for-the-rectangular-assignment-problem
//
// Both this code and the orignal code are published under the BSD license.
// by Cong Ma, 2016
//

#include <iostream>
#include <vector>

using namespace std;

namespace HungarianAlgorithm
{
double Solve(vector<vector<double>>& DistMatrix, vector<int>& Assignment);
}; // namespace HungarianAlgorithm