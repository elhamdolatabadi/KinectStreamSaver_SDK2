

To collect data with the Kinect Stream Saver:

    Run KinectSources-D2D.exe:

    In the lower right corner of the window: Enter subject's name if desired. Choose which data streams to collect. Press Capture to begin data collection. Press Stop to end data collection.

    The collected data will appear in C:\KinectData.

    LiTimeStamp.binary: This file has fields: counter#, frame#, timestamp
        The counter starts at 1 when the Capture button is pressed and data collection begins.
        The frame number is from the Kinect API; it starts when KinectExplorer-D2D.exe is launched.
        The timestamp is in milliseconds, and is useful for recording frame rate. (At present data is collected at approximately 30 frames/second, so frames are approx 33 msecs apart.)

    Color images are saved as .bmp files, on each frame. . as .bmp files on each frame. . as "binary" data stored in a .binary file.

    Depth images are saved either . as .bmp files on each frame. . as "binary" data stored in a .binary file.

    Skeleton data: The program can track up to six skeletons at once. For the first four, all joints are tracked. For the last two, only the center of mass is tracked.

    Saving The Joint Positions: one data files is created, named Joint_Position.binary.

    One hundred fifty rows are written to each skeleton data file for each frame. These 150 rows store data for the 25 skeleton joints in the following order for 6 tracked users.

    HIP_CENTER SPINE SHOULDER_CENTER HEAD SHOULDER_LEFT ELBOW_LEFT WRIST_LEFT HAND_LEFT SHOULDER_RIGHT ELBOW_RIGHT WRIST_RIGHT HAND_RIGHT HIP_LEFT KNEE_LEFT ANKLE_LEFT FOOT_LEFT HIP_RIGHT KNEE_RIGHT ANKLE_RIGHT FOOT_RIGHT SPINESHOULDER HANDTIP_LEFT THUMB_LEFT HANDTIP_RIGHT THUMB_RIGHT

--- "Joint_Position_....binary" File ---

 Each row contains following fields: 
- position (x, y, z); joint tracking state (in joint rows)*    	

    Tracking states: 0=joint is not tracked; 1=joint is inferred; 2=joint is tracked

Caveats:

i) If a user that is being tracked leaves the field of view and later returns, the Kinect does not know it is the same person, so they will be assigned a new skeleton id. This can mean that the person's data appears in more than one data file.

    Run ReadBinarySkeleton.m to animate the recorded "Joint_Position_....binary"

    Run edConvertBinaryImage.m to open recorded binary images

Implementation:

The Kinect Stream Saver application is based on the KinectExplorer-D2D (for C++) sample code. Modifications to the sample code are explained below.

    Two new classes were added.

    StreamSaver:
    This class has methods that store stream data (color, depth, skeleton) to a dynamic FIFO buffer as the data is received from the Kinect device, and separately write the data from the FIFO buffer to output files as described above. The disk writes are done in a separate thread to avoid degradation of frame processing speed during data collection.

    SaverViewer: This class implements a fourth GUI tab in the KinectExplorer application. The tab is labeled "Recording" and it allows users to specify which data streams should be saved to disk.

    A few classes in the KinectExplorer sample code were modified. The changes were kept to a minimum, but were necessary to invoke the functionality of the new classes.

    KinectWindow
    NuiColorStream
    NuiDepthStream
    NuiSkeletonStream

    A new GUI "Recording" tab was designed and included in the application resources. This included modifications to the following files.

    KinectExplorer.rc
    resource.h

Instructions to open "Joint_Position_....binary" in MATLAB:

path = '' % read the Skeleton disp(path) fid = fopen([path,'\Skel\liTimeStamp.binary']); T = fread(fid,'int64'); fclose(fid); sensor.TimeElapsed = T; fid = fopen([path,'\Skel\Joint_Position.binary']); A = fread(fid,'float'); fclose(fid); A(A>5)= 0; A(A<-5)= 0; jointNumber = 25; tracks = 6; AAAA = reshape(A, 4,jointNumber,tracks,[]); sensor.Skeleton = permute(AAAA,[1,2,4,3]);

Acknowledgement:

Special thanks to Marge Coahran for all the help with putting together the README file !

Terms of Use:

If your research benefits from the Kinect Stream Saver application provided by IATSL, please consider the following Terms of Use:

    please cite the followign paper in any publications about your work:
        Elham Dolatabadi, Babak Taati, Gemma S. Parra-Dominguez, Alex Mihailidis, “A markerless motion tracking approach to understand changes in gait and balance: A Case Study”, the Rehabilitation Engineering and Assistive Technology Society of North America (RESNA 2013 Annual Conference). Student Scientific Paper Competition Winner

    please acknowledge this application in any publications about your work. Thank you for your support.

    Acknowledgement Text Examples:

    We would like to acknowledge Intelligent Assisstive Technology and Systems Lab for the application that facilitated this research, including the Kinect Stream Saver application developed by Elham Dolatabadi

Questions?

Please contact Elham Dolatabadi, at elham.dolatabadi@mail.utoronto.ca and +1 (416) 597-3422 x7391