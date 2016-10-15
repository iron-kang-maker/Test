function f = line_function(p1, p2, p)
    %(x1 - x)(y2 - y1) - (x2 - x1)(y1 - y) = 0
    f = (p1(1) - p(1))*(p2(2) - p1(2)) - (p2(1) - p1(1))*(p1(2) - p(2));
end