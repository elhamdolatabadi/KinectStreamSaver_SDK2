function plot3D(pointCloud,varargin)
%plot3(pointCloud(3,:), pointCloud(1,:), pointCloud(2,:), 'o', 'MarkerSize', 20, 'MarkerFaceColor',varargin{:});

% head
plot3(pointCloud(3,4), pointCloud(1,4), pointCloud(2,4), 'o', 'MarkerSize', 20, 'MarkerFaceColor',varargin{:});

line(pointCloud(3,5:8), pointCloud(1,5:8), pointCloud(2,5:8), 'LineWidth', 3, 'Color',varargin{:});
line(pointCloud(3,13:16), pointCloud(1,13:16), pointCloud(2,13:16), 'LineWidth', 3, 'Color',varargin{:});

line(pointCloud(3,9:12), pointCloud(1,9:12), pointCloud(2,9:12), 'LineWidth', 3, 'Color',varargin{:});
line(pointCloud(3,17:20), pointCloud(1,17:20), pointCloud(2,17:20),'LineWidth', 3, 'Color',varargin{:});


line(pointCloud(3,1:4), pointCloud(1,1:4), pointCloud(2,1:4), 'LineWidth', 3, 'Color',varargin{:});
line(pointCloud(3,[17,1,13]), pointCloud(1,[17,1,13]), pointCloud(2,[17,1,13]), 'LineWidth', 3, 'Color',varargin{:});
%line(pointCloud(3,[5,21,9]), pointCloud(1,[5,21,9]), pointCloud(2,[5,21,9]), 'LineWidth', 3, 'Color',varargin{:});

end
