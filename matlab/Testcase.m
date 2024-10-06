%% Generating test case for rate matching function
% @author: Linhnth@soc.one
% @ Date : 08/09/2024
% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

clc;
clear all;
addpath(genpath(pwd));

% Parameters
rv = 3;                 % Redundancy version
modulation = '256QAM';    % Modulation type
nlayers = 4;            % Number of layers
Nref = [];
% Check if Nref is empty
if isempty(Nref)
    Nref_txt = 'NoNref';
else
    Nref_txt = num2str(Nref);
end

%% LDPC Processing
% LDPC encoding
%encoded = ones(1320,1);
encoded = randi([0,1], 1320,1);
% Rate matching and code block concatenation
outlen = 2000;

%% Save inputfile
% Open a text file for writing
input_filename = '../io/input/input_data1.txt';
%input_filename = strcat('./io/input/input_inlen', num2str(length(encoded)), '_outlen', num2str(outlen), '_rv',num2str(rv), '_nlayers', num2str(nlayers), '_', modulation,'_', Nref_txt, '.txt');
save_128bit_per_line(input_filename, encoded)

%% nrRateMatchLDPC function (standard function)
ratematched = nrRateMatchLDPC(encoded,outlen,rv,modulation,nlayers);
fname_out = '../io/output/output_data1_matlab.txt';
%fname_out = strcat('./io/output/output_inlen', num2str(length(encoded)), '_outlen', num2str(outlen), '_rv',num2str(rv), '_nlayers', num2str(nlayers), '_', modulation, '_', Nref_txt, '.mat');
save(fname_out, "ratematched");
save_128bit_per_line(fname_out, ratematched)

%% nrRateMatchLDPC2 function (function is modified)
% Get modulation order
switch modulation
    case 'BPSK'
        Qm = 1;
    case 'QPSK'
        Qm = 2;
    case '16QAM'
        Qm = 4;
    case '64QAM'
        Qm = 6;
    case '256QAM'
        Qm = 8;
    otherwise   % '1024QAM'
        Qm = 10;
end
ratematched2 = nrRateMatchLDPC_modify(encoded, outlen,rv, Qm, nlayers);
sum(ratematched - ratematched2)
