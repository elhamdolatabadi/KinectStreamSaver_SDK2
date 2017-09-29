function edConvertBinaryImage%(filename,StreamType)
%  ----------- Description
% edConvertBinaryImage(filename,StreamType) reads and converts binary images to any format (e.g. bmp, jpeg or png)
%
% Author: Elham Dolatabadi 

filename = uigetdir('C:/KinectData');
StreamType = 'Color';
% convert binary image to png
cColorWidth = 1920;
cColorHeight = 1080;

cDepthWidth = 512;
cDepthHeight = 424;
k = 0;
frame_bgn = 1;
% create a new folder to save to file the converted binary images
status = mkdir(char(filename),strcat(StreamType,'_png'));
    if (status)
        newdir = strcat(filename,'\',StreamType,'_png');
    else
        newdir = filename;
    end
while (1)
    % open the binary image
    fid = fopen(char(strcat(filename,'\',StreamType,'\', StreamType,'_Raw_',num2str(k),'.binary')));
    if (fid == -1)
        break;
    end
    
    if (strcmp(StreamType,'Color')) % check if the stream is color
        Img = fread(fid,'*uint8');
        str_byte_No = 4;
        TT = reshape(Img, str_byte_No,cColorWidth*cColorHeight, []);
        frameCount = ( length(Img) / cColorHeight / cColorWidth /str_byte_No);
    else  % check if the strean is depth
        Img = fread(fid,'*ushort');
        %Img = double(Img)/8;
        str_byte_No = 1;
        TT = reshape(Img, cDepthWidth,cDepthHeight, []);
        frameCount = ( length(Img) / cDepthHeight / cDepthWidth /str_byte_No);
    end
    fclose(fid);  
    frame_end = frameCount + frame_bgn - 1;
    for frameNumber = 1 : frameCount
        currentframe = int2str(frameNumber+ frame_bgn - 1);
        Bnr_image = squeeze(TT(:,:,frameNumber));
        if (str_byte_No == 4)
            im = reshape(Bnr_image,str_byte_No,cColorWidth,cColorHeight);
            im = permute(im,[3,2,1]);
            ARGB = im(:,:,end:-1:1); %BGRA -> RGBA
            RGB = ARGB(:,:,2:end);   % RGBA -> RGB(true)
            alpha = ARGB(:,:,1);
            Cnrtd_Img = RGB;
        else
            depth = Bnr_image';  %depth
            Cnrtd_Img = depth;
        end
        SavePath = char(strcat(newdir, '\', currentframe,StreamType,'.png'));
        imshow(Cnrtd_Img)
        imwrite(Cnrtd_Img,SavePath,'png'); % write the image to the file as png format
    end
    frame_bgn = frame_end + 1;
    
    k = k+1;
end