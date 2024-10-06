%% Save file function
% file: the name of the file
% data: Input vector

function save_128bit_per_line(filename, data)
    fid = fopen(filename, 'w');
    if fid == -1
        error('Error open the file.');
    end
    
    binaryString = num2str(data);  
    binaryString = reshape(binaryString', 1, []); 
    
    numBits = length(binaryString);
    blockSize = 128;
    numBlocks = ceil(numBits / blockSize);
    
    for i = 1:numBlocks
        startIdx = (i - 1) * blockSize + 1;
        endIdx = min(i * blockSize, numBits); % Đảm bảo không vượt quá số bit có sẵn
        
        block = binaryString(startIdx:endIdx);
        
        if length(block) < blockSize
            block = cat(2, block, repmat('0', 1, blockSize - length(block)));
        end
        
        fprintf(fid, '%s\n', block);
    end
    
    fclose(fid);
end