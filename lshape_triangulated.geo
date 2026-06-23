// Gmsh project created on Tue May 12 14:03:37 2026
//+
Point(1) = {0, 3.5, 0, 1.0};
//+
Point(2) = {1, 3.5, 0, 1.0};
//+
Point(3) = {0, 0, -0, 1.0};
//+
Point(4) = {1, 0, -0, 1.0};
//+
Point(5) = {1, 0.8, -0, 1.0};
//+
Point(6) = {2.6, 0.8, -0, 1.0};
//+
Point(7) = {2.6, 0, -0, 1.0};
//+
Point(8) = {2.6, 3.5, 0, 1.0};
//+
Point(9) = {2.6, 2.7, 0, 1.0};
//+
Point(10) = {1, 2.7, 0, 1.0};
//+
Line(1) = {1, 2};
//+
Line(2) = {2, 10};
//+
Line(3) = {5, 5};
//+
Line(4) = {5, 10};
//+
Line(5) = {5, 4};
//+
Line(6) = {5, 6};
//+
Line(7) = {6, 7};
//+
Line(8) = {7, 4};
//+
Line(9) = {4, 3};
//+
Line(10) = {3, 1};
//+
Line(11) = {2, 8};
//+
Line(12) = {9, 10};
//+
Line(13) = {8, 9};
//+
Curve Loop(1) = {10, 1, 2, -4, 5, 9};
//+
Plane Surface(1) = {1};
//+
Curve Loop(2) = {11, 13, 12, -2};
//+
Plane Surface(2) = {2};
//+
Curve Loop(3) = {6, 7, 8, -5};
//+
Plane Surface(3) = {3};
//+
Transfinite Surface {2};
//+
Transfinite Surface {1};
//+
Transfinite Surface {3};
//+
Transfinite Curve {10, 2, 4, 5} = 10 Using Progression 1;
//+
Point(11) = {0, 0.8, 0, 1.0};
//+
Point(12) = {0, 2.7, -0, 1.0};
//+
Line(14) = {12, 10};
//+
Line(15) = {11, 5};
//+
Plane Surface(4) = {2};
