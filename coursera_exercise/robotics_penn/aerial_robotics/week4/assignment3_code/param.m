time = [ 0 3.4641 6.9282 10.3923 13.8564];
pos = [0 1 2 3 4;0 1 0 -1 0;0 1 2 1 0];
x0 = pos(1,1);
x1 = pos(1,2);
x2 = pos(1,3);
x3 = pos(1,4);
x4 = pos(1,5);
y0 = pos(2,1);
y1 = pos(2,2);
y2 = pos(2,3);
y3 = pos(2,4);
y4 = pos(2,5);
z0 = pos(3,1);
z1 = pos(3,2);
z2 = pos(3,3);
z3 = pos(3,4);
z4 = pos(3,5);
t0 = time(1);
t1 = time(2);
t2 = time(3);
t3 = time(4);
t4 = time(5);

c = coffie(t0, t1, t2, t3, t4, x0, x1, x2, x3, x4);
c_y = coffie(t0, t1, t2, t3, t4, y0, y1, y2, y3, y4);
c_z = coffie(t0, t1, t2, t3, t4, z0, z1, z2, z3, z4);
%Boundary condition
%x1(t0) = 0
%{
fprintf('x1(t0) = %f\n', ori(0, c(1:8)));
fprintf('x1(t1) = %f, x2(t1) = %f, ori: %f\n', ori(t1, c(1:8)), ori(t1, c(9:16)), x1);
fprintf('x2(t2) = %f, x3(t2) = %f, ori: %f\n', ori(t2, c(9:16)), ori(t2, c(17:24)), x2);
fprintf('x3(t3) = %f, x4(t3) = %f, ori: %f\n', ori(t3, c(17:24)), ori(t3, c(25:32)), x3);
fprintf('x4(t4) = %f, ori = %f\n', ori(t4, c(25:32)), x4);
fprintf('1dot x1(t0) = %f (0)\n', deri_1st(0, c(1:8)));
fprintf('2dot x1(t0) = %f (0)\n', deri_2nd(0, c(1:8)));
fprintf('3dot x1(t0) = %f (0)\n', deri_3rd(0, c(1:8)));
fprintf('1dot x4(t4) = %f (0)\n', deri_1st(t4, c(25:32)));
fprintf('2dot x4(t4) = %f (0)\n', deri_2nd(t4, c(25:32)));
fprintf('3dot x4(t4) = %f (0)\n', deri_3rd(t4, c(25:32)));
fprintf('1dot x1(t1) = %f, dot x2(t1) = %f\n', deri_1st(t1, c(1:8)), deri_1st(t1, c(9:16)));
fprintf('2dot x1(t1) = %f, dot x2(t1) = %f\n', deri_2nd(t1, c(1:8)), deri_2nd(t1, c(9:16)));
fprintf('3dot x1(t1) = %f, dot x2(t1) = %f\n', deri_3rd(t1, c(1:8)), deri_3rd(t1, c(9:16)));
fprintf('4dot x1(t1) = %f, dot x2(t1) = %f\n', deri_4th(t1, c(1:8)), deri_4th(t1, c(9:16)));
fprintf('5dot x1(t1) = %f, dot x2(t1) = %f\n', deri_5th(t1, c(1:8)), deri_5th(t1, c(9:16)));
fprintf('6dot x1(t1) = %f, dot x2(t1) = %f\n', deri_6th(t1, c(1:8)), deri_6th(t1, c(9:16)));
fprintf('1dot x2(t2) = %f, dot x3(t2) = %f\n', deri_1st(t2, c(9:16)), deri_1st(t2, c(17:24)));
fprintf('2dot x2(t2) = %f, dot x3(t2) = %f\n', deri_2nd(t2, c(9:16)), deri_2nd(t2, c(17:24)));
fprintf('3dot x2(t2) = %f, dot x3(t2) = %f\n', deri_3rd(t2, c(9:16)), deri_3rd(t2, c(17:24)));
fprintf('4dot x2(t2) = %f, dot x3(t2) = %f\n', deri_4th(t2, c(9:16)), deri_4th(t2, c(17:24)));
fprintf('5dot x2(t2) = %f, dot x3(t2) = %f\n', deri_5th(t2, c(9:16)), deri_5th(t2, c(17:24)));
fprintf('6dot x2(t2) = %f, dot x3(t2) = %f\n', deri_6th(t2, c(9:16)), deri_6th(t2, c(17:24)));
fprintf('1dot x3(t3) = %f, dot x4(t3) = %f\n', deri_1st(t3, c(17:24)), deri_1st(t3, c(25:32)));
fprintf('2dot x3(t3) = %f, dot x4(t3) = %f\n', deri_2nd(t3, c(17:24)), deri_2nd(t3, c(25:32)));
fprintf('3dot x3(t3) = %f, dot x4(t3) = %f\n', deri_3rd(t3, c(17:24)), deri_3rd(t3, c(25:32)));
fprintf('4dot x3(t3) = %f, dot x4(t3) = %f\n', deri_4th(t3, c(17:24)), deri_4th(t3, c(25:32)));
fprintf('5dot x3(t3) = %f, dot x4(t3) = %f\n', deri_5th(t3, c(17:24)), deri_5th(t3, c(25:32)));
fprintf('6dot x3(t3) = %f, dot x4(t3) = %f\n', deri_6th(t3, c(17:24)), deri_6th(t3, c(25:32)));
%}
