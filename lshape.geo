// Gmsh project created on Tue May 12 09:05:47 2026
//+
Point(1) = {0, 0, 0, 1.0};
//+
Point(2) = {0.5, -0, 0, 1.0};
//+
Point(3) = {-0, -1, -0, 1.0};
//+
Point(4) = {0.5, -1, -0, 1.0};
//+
Point(5) = {-0, -1.5, -0, 1.0};
//+
Point(6) = {0.5, -1.5, -0, 1.0};
//+
Point(7) = {1.3, -1, -0, 1.0};
//+
Point(8) = {1.3, -1.5, -0, 1.0};
//+
Line(1) = {1, 2};
//+
Line(2) = {1, 3};
//+
Line(3) = {2, 4};
//+
Line(4) = {4, 7};
//+
Line(5) = {7, 8};
//+
Line(6) = {6, 6};
//+
Line(7) = {6, 8};
//+
Line(8) = {6, 5};
//+
Line(9) = {5, 3};
//+
Line(10) = {4, 6};
//+
Line(11) = {4, 3};
//+
Curve Loop(1) = {3, 11, -2, 1};
//+
Plane Surface(1) = {1};
//+
Curve Loop(2) = {9, -11, 10, 8};
//+
Plane Surface(2) = {2};
//+
Curve Loop(3) = {4, 5, -7, -10};
//+
Plane Surface(3) = {3};
//+
Transfinite Surface {1};
//+
Transfinite Surface {2};
//+
Transfinite Surface {3};
//+
Transfinite Curve {2, 3} = 10 Using Progression 1;
//+
Transfinite Curve {1, 11} = 5 Using Progression 1;
//+
Transfinite Curve {9, 10} = 5 Using Progression 1;
//+
Transfinite Curve {11, 8} = 5 Using Progression 1;
//+
Transfinite Curve {10, 5} = 5 Using Progression 1;
//+
Transfinite Curve {4, 7} = 7 Using Progression 1;
//+
Recombine Surface {1, 2, 3};
