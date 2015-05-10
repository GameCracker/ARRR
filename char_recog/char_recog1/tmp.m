% Detect by erosion in crisp image
alpha = 0.4; 
detectByErosion = 1; 
if detectByErosion
    for nLetter = 1:length(letters)
        SE = imerode(templates{nLetter}, ones(3,3)); 
        imgTestErode = imerode(imgTestBin, SE);
        rows = 50:height-50; 
        cols = 50:width-50;
        if sum(sum(imgTestErode(rows,cols))) > 0
            fprintf('Detected clean, erosion: %s\n', letters(nLetter)); 
            imgTestErodeDilate = imdilate(imgTestErode, SE);
            figure(3); 
            clf; 
            set(gcf, 'Color', 'w'); 
            imshow(max(imgTestErodeDilate, alpha*imgTestBin));
            pause
        end
    end
end

% Detect by hit-miss in crisp image
detectByHitMiss = 1; 
if detectByHitMiss
    for nLetter = 1:length(letters)
        SE1 = imerode(templates{nLetter}, ones(3,3));
        SE2 = imdilate(templates{nLetter}, ones(5,5)) - imdilate(templates{nLetter}, ones(3,3));
        imgTestHitMiss = bwhitmiss(imgTestBin, SE1, SE2); 
        rows = 50:height-50; 
        cols = 50:width-50;
        if sum(sum(imgTestHitMiss(rows,cols))) > 0
            fprintf('Detected clean, hit-miss: %s\n', letters(nLetter)); 
            imgTestHitMissDilate = imdilate(imgTestHitMiss, SE1); 
            figure(3); 
            clf; 
            set(gcf, 'Color', 'w'); 
            imshow(max(imgTestHitMissDilate, alpha*imgTestBin));
            pause
        end
    end 
end







% Detect by hit-miss in noisy image
detectByHitMiss = 1; 
if detectByHitMiss
    for nLetter = 1:length(letters)
        SE1 = imerode(templates{nLetter}, ones(3,3));
        SE2 = imdilate(templates{nLetter}, ones(5,5)) - imdilate(templates{nLetter}, ones(3,3));
        imgTestHitMiss = bwhitmiss(imgTestNoiseBin, SE1, SE2); 
        rows = 50:height-50; 
        cols = 50:width-50;
        if sum(sum(imgTestHitMiss(rows,cols))) > 0
            fprintf('Detected noisy, hit-miss: %s\n', letters(nLetter)); 
            imgTestHitMissDilate = imdilate(imgTestHitMiss, SE1); 
            figure(3); 
            clf; 
            set(gcf, 'Color', 'w'); 
            imshow(max(imgTestHitMissDilate, alpha*imgTestBin));
            pause
        end
    end 
end

% Detect by rank filter in noisy image
detectByRankFilter = 1; 
if detectByRankFilter
    for nLetter = 1:length(letters)
        SE1 = imerode(templates{nLetter}, ones(3,3));
        SE2 = imdilate(templates{nLetter}, ones(5,5)) - imdilate(templates{nLetter}, ones(3,3));
        imgTestHitMiss1 = ordfilt2(imgTestNoiseBin, 15, SE1); 
        imgTestHitMiss2 = ordfilt2(~imgTestNoiseBin, 12, SE2); 
        imgTestHitMiss = min(imgTestHitMiss1, imgTestHitMiss2); 
        rows = 50:height-50; 
        cols = 50:width-50;
        if sum(sum(imgTestHitMiss(rows,cols))) > 0
            fprintf('Detected noisy, rank: %s\n', letters(nLetter)); imgTestHitMissDilate = imdilate(imgTestHitMiss, SE1); 
            figure(3); 
            clf; 
            set(gcf, 'Color', 'w'); 
            imshow(max(imgTestHitMissDilate, alpha*imgTestNoiseBin)); 
            pause
        end
    end 
end
