
%%This program is used to show the first to fifth test problems

%clc
%clear
%load Problem_10.mat % load the map

%% Enlarge the map, in order to the image are easier to see

k=3; % Enlarge the map by 3 times
for i=1:size(Map,1)
    for j=1:size(Map,2)
        s=Map(i,j);
        ss=ones(k,k);
        ss=s*ss;
        sss(i,j)={ss};
    end
end
for i=1:size(Map,1)
    for j=1:size(Map,2)
        big((i-1)*k+1:i*k,(j-1)*k+1:j*k)=cell2mat(sss(i,j));
    end
end
imshow(big)
hold on

for i=1:size(Red_areas,1) % show red areas
    x=Red_areas(i,1);
    y=Red_areas(i,2);
    s1=[x,y];
    s2=[x-1,y];
    s3=[x-1,y-1];
    s4=[x,y-1];
    s=[s1;s2;s3;s4];
    xx=s(:,1)*k+0.5;
    yy=s(:,2)*k+0.5;
    h=fill(xx,yy,'r','linewidth',0.1);
    set(h,'edgealpha',0,'facealpha',1) ;
end 

text(START_x*k-0.5*k+0.5-3,START_y*k-0.5*k+0.5+k,'START','Color','blue','FontSize',5); % show 'START'
text(GOAL_x*k-0.5*k+0.5-2,GOAL_y*k-0.5*k+0.5-k,'GOAL','Color','blue','FontSize',5); % show 'GOAL'
scatter(START_x*k-0.5*k+0.5,START_y*k-0.5*k+0.5,'MarkerEdgeColor',[0 0 1],'MarkerFaceColor',[0 0 1], 'LineWidth',1); % show the blue dot for marking 'START'
scatter(GOAL_x*k-0.5*k+0.5,GOAL_y*k-0.5*k+0.5,'MarkerEdgeColor',[0 1 0],'MarkerFaceColor',[0 1 0], 'LineWidth',1); % show the green dot for marking 'GOAL'
