clear all;
clc;
p=[1 1;1 2;2 -1;2 0;-1 2;-2 1;-1 -1;-2 -2]';
t1=[0 0;0 0;0 1;0 1;1 0;1 0;1 1;1 1]';
t=t1;
w=[1 0;0 1];
b=[1;1];
e=zeros(2,8);
x=1;
k=0;
q(:,:)=0;
alpha=0.04;
for j=1:8
    for i=1:2
        if(t(i,j)==0)
            t(i,j)=-1;
        end
    end
end
while(x)
    k=k+1;
    for j=1:8
        y=purelin(w*p(:,j)+b);
        e(:,j)=t(:,j)-y;
        w=w+(2*alpha*e(:,j)*p(:,j)')
        b=b+2*alpha*e(:,j)
    end
    f(k)=meansqr(e);
    q(k+1)=f(k);
    h=f(k)-q(k);
    h=round(h,3);
    if(h==0)
        x=0;
    end  
end
figure('Name','Adaline Classification','NumberTitle','off');
plotpv(p,t1);
plotpc(w,b);
legend('p=[1 1] ''','p=[1 2] ''','p=[2 -1] ''','p=[2 0] ''','p=[-1 2] ''','p=[-2 1] ''','p=[-1 -1] ''','p=[-2 -2] ''');
figure('Name','Training Performance','NumberTitle','off');
plot(f);
title(['Training Performance using adaline for epochs = ' num2str(k)]);
legend('mean square error in adaline');
xlabel('epochs');
ylabel('mean square error');
