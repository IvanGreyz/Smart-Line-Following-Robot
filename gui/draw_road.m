function [left_road,right_road] = draw_road()    
    % Imaginary point 
    I1 = 100*[-25 5]; I2 = 100*[-20 10]; I3 = 100*[-15 15]; I4 = 100*[-15 5];
    % Travel points 
    A = [0 0]; 
    B = 100*[-25 0]; 
    C = 100*[-25 10]; 
    D = I2 + 100*[-8*tan(pi/8) 0]; 
    E1 = I3 + 100*[8*tan(pi/8) 0]; F1 = 100*[0 15];
    E2 = I4 + 100*[8*tan(pi/8) 0]; F2 = 100*[0 5];
    
    % Circle center points
    R1 = 100*[-25 5]; 
    R21 = I2 + 100*[-8*tan(pi/8) 8]; R22 = I3 + 100*[8*tan(pi/8) -8]; 
    R31 = I2 + 100*[-8*tan(pi/8) -8]; R32 = I4 + 100*[8*tan(pi/8) 8]; 
    
    % Coverage angle
    a1 = [3*pi/2 pi/2]; 
    a21 = [-pi/2 -pi/4]; a22 = [3*pi/4 pi/2]; 
    a31 = [pi/2 pi/4]; a32 = [-3*pi/4 -pi/2];
    
    % Line plot
    l1 = line_plot(A,B);
    c2 = circle_plot(R1,a1,500);
    l3 = line_plot(C,D);
    c4 = circle_plot(R21,a21,800);
    c5 = circle_plot(R22,a22,800);
    l6 = line_plot(E1,F1);
    c7 = circle_plot(R31,a31,800);
    c8 = circle_plot(R32,a32,800);
    l9 = line_plot(E2,F2);
    
    left_road = [l1(:,1:end-1) c2(:,1:end-1) l3(:,1:end-1) c4 c5(:,1:end-1) l6];
    right_road = [l1(:,1:end-1) c2(:,1:end-1) l3(:,1:end-1) c7 c8(:,1:end-1) l9];
end

function points = line_plot(A,B)
    points = [linspace(A(1),B(1),100);linspace(A(2),B(2),100)];
end

function points = circle_plot(R,a,r)
    points = R'*ones(1,100) + r*[cos(linspace(a(1),a(2),100)) ; sin(linspace(a(1),a(2),100))];
end

