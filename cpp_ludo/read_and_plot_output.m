filename = 'output.txt';
delimiterIn = '\t';
%headerlinesIn = 1;
A = importdata(filename,delimiterIn);

figure(1)
plot(A(:,1),A(:,2))
hold on;
plot(A(:,1),A(:,3))
plot(A(:,1),A(:,4))
hold off