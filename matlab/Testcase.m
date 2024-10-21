%% Generating test case for rate matching function
% @author: Linhnth@hnsoc.one
% @ Date : 08/09/2024
% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

clc;
clear all;
addpath(genpath(pwd));

%% Config Parameters
inlen       = 1320; % Input length
outlen      = 2000; %Output length
rv          = 1;                 % Redundancy version
modulation  = '16QAM';    % Modulation type
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
nlayers     = 2;            % Number of layers
Nref        = 0;
% Check if Nref is empty
if isempty(Nref)
    Nref_txt = 'NoNref';
else
    Nref_txt = num2str(Nref);
end

% Open the file for writing
fileID = fopen('../io/input/config_inputdata2.txt', 'w');

% Write the configuration to the file in the specified format
fprintf(fileID, 'input_length: %d\n', inlen);
fprintf(fileID, 'output_length: %d\n', outlen);
fprintf(fileID, 'redundancy_version: %d\n', rv);
fprintf(fileID, 'layer: %d\n', nlayers);
fprintf(fileID, 'modulation_type: %d\n', Qm);
fprintf(fileID, 'Nref: %d\n', Nref);

% Close the file
fclose(fileID);

% Notify that the file has been written
disp('Configuration file "config_inputdata2.txt" generated.');

%% Generate input and output
% LDPC encoding
%encoded = ones(1320,1);
encoded = randi([0,1], inlen ,1);

%% Save inputfile
% Open a text file for writing
input_filename = '../io/input/input_data2.txt';
%input_filename = strcat('./io/input/input_inlen', num2str(length(encoded)), '_outlen', num2str(outlen), '_rv',num2str(rv), '_nlayers', num2str(nlayers), '_', modulation,'_', Nref_txt, '.txt');
save_128bit_per_line(input_filename, encoded)

%% nrRateMatchLDPC function (standard function)
ratematched = nrRateMatchLDPC(encoded,outlen,rv,modulation,nlayers);
fname_out = '../io/output/output_data2_matlab.txt';
%fname_out = strcat('./io/output/output_inlen', num2str(length(encoded)), '_outlen', num2str(outlen), '_rv',num2str(rv), '_nlayers', num2str(nlayers), '_', modulation, '_', Nref_txt, '.mat');
save(fname_out, "ratematched");
save_128bit_per_line(fname_out, ratematched)

%% nrRateMatchLDPC2 function (function is modified)
ratematched2 = nrRateMatchLDPC_modify(encoded, outlen,rv, Qm, nlayers);
sum(ratematched - ratematched2)
