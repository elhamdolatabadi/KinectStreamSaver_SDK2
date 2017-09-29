function m = show3(KinectJoint,time,k)

mrkr_clr = {'r','k','b','g','m','c'};
m = plot3(0,0,0,'w.');
hold on
title(['time =  ', num2str(time/1000),'  seconds', ',   Frame No. =  ', num2str(k)])
for s = 1:6
    disp('-----------')
    if (sum(KinectJoint(:,:,s)~= 0))
        plot3D(KinectJoint(:,:,s),mrkr_clr{s})
        text(KinectJoint(3,4,s), KinectJoint(1,4,s), KinectJoint(2,4,s)+0.5,num2str(s),'FontSize',18)
    else plot3([],[],[]);
    end
end

hold off
cameratoolbar('Show');