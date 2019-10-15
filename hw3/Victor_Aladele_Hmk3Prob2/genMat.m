% Writing a Matrix to a file
N = 10000;
fileName = 'Test1000.txt';
dlmwrite(fileName, [N, N], 'delimiter', ' ');
M = randi([0 99], [N, N], 'int16');
dlmwrite(fileName, M, '-append', 'delimiter', ' ')