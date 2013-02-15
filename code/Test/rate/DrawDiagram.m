function [ output_args ] = DrawDiagram( fName )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here

M = dlmread(fName);
%M(:, 1) = [];
[r, c] = size(M);
X = [0:.5:r*.5 - .5];
Y = [0: .2: c*.2 - .2];

figure;surf(Y, X, M);

end

