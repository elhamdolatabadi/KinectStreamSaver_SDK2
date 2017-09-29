function AnimateSkel(data)

joint = data.Skeleton(1:3,:,:,:);
t = data.TimeElapsed;
nframe = numel(t);

t = t - min(t);
timediff = diff(t);

zmax = max(max(joint(3,:,:)));
zmin = min(min(joint(3,:,:)));
xmin = min(min(joint(1,:,:)))*1.5;
xmax = max(max(joint(1,:,:)))*1.5;
ymin = min(min(joint(2,:,:)));
ymax = max(max(joint(2,:,:)));
axis ([zmin zmax xmin xmax ymin ymax]);
axis vis3d;

for f = 1: nframe-1
    drawnow;
    show3(joint(:,:,f,:),t(f),f);
    axis ([zmin zmax xmin xmax ymin ymax]);
    axis vis3d;
    %view (90,0);
    %view (300,30);
    %grid on;
    drawnow;
    %pause
    %pause(0.05);
    %pause(timediff(f)/1000);
    
end

end