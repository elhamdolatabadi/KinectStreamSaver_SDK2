path = ''
% read the Skeleton
disp(path)
fid = fopen([path,'\Skel\liTimeStamp.binary']);
T = fread(fid,'int64');
fclose(fid);
sensor.TimeElapsed = T;
fid = fopen([path,'\Skel\Joint_Position.binary']);
A = fread(fid,'float');
fclose(fid);
A(A>5)= 0;
A(A<-5)= 0;
jointNumber = 25;
tracks = 6;
AAAA = reshape(A, 4,jointNumber,tracks,[]);
sensor.Skeleton = permute(AAAA,[1,2,4,3]);
AnimateSkel(sensor);