function out = nrRateMatchLDPC_modify(in, outlen,rv,Qm,nlayers,varargin)
    narginchk(5,6);
    if nargin==5
        Nref = [];
    else
        Nref = varargin{1};
    end

    % Output empty if input is empty or outlen is 0
    if isempty(in) || ~outlen
        out = zeros(0,1,class(in));
        return;
    end
    N = length(in);
    % Check against all possible lifting sizes
    ZcVec = [2:16 18:2:32 36:4:64 72:8:128 144:16:256 288:32:384];
    coder.internal.errorIf(~(any(N==(ZcVec.*66)) || any(N==(ZcVec.*50))), ...
        'nr5g:nrLDPC:InvalidInputNumRows',N);

    % Determine base graph number from N
    if any(N==(ZcVec.*66))
        bgn = 1;
        ncwnodes = 66;
    else % must be one of ZcVec.*50
        bgn = 2;
        ncwnodes = 50;
    end
    Zc = N/ncwnodes;

    

    % Get code block soft buffer size
    if ~isempty(Nref)

        Ncb = min(N,Nref);
    else    % No limit on buffer size
        Ncb = N;
    end

    % Get starting position in circular buffer
    if bgn == 1
        if rv == 0
            k0 = 0;
        elseif rv == 1
            k0 = floor(17*Ncb/N)*Zc;
        elseif rv == 2
            k0 = floor(33*Ncb/N)*Zc;
        else % rv is equal to 3
            k0 = floor(56*Ncb/N)*Zc;
        end
    else
        if rv == 0
            k0 = 0;
        elseif rv == 1
            k0 = floor(13*Ncb/N)*Zc;
        elseif rv == 2
            k0 = floor(25*Ncb/N)*Zc;
        else % rv is equal to 3
            k0 = floor(43*Ncb/N)*Zc;
        end
    end

    % Get rate matching output for all scheduled code blocks and perform
    % code block concatenation according to Section 5.4.2 and 5.5
    if (1-outlen/(nlayers*Qm)-1 >=0)
        E = nlayers*Qm*floor(outlen/(nlayers*Qm));
    else
        E = nlayers*Qm*ceil(outlen/(nlayers*Qm));
    end

    out = cbsRateMatch(in,E,k0,Ncb,Qm); %#ok<AGROW>

end

function e = cbsRateMatch(d,E,k0,Ncb,Qm)
% Rate match a single code block segment as per TS 38.212 Section 5.4.2

    % Bit selection, Section 5.4.2.1 
    % Get number of filler bits inside the circular buffer
    NFillerBits = sum(d(1:Ncb) == -1); 

    % Duplicate data if more than one iteration around the circular
    % buffer is required to obtain a total of E bits
    d = repmat(d(1:Ncb),ceil(E/(length(d(1:Ncb))-NFillerBits)),1);

    % Shift data to start from selected redundancy version
    d = circshift(d,-k0);

    % Avoid filler bits and provide an empty vector if E is 0
    e = zeros(E,1,class(d));
    e(:) = d(find(d ~= -1,E + (E==0)));        
    
    % Bit interleaving, Section 5.4.2.2
    e = reshape(e,E/Qm,Qm);
    e = e.';
    e = e(:); 

end
