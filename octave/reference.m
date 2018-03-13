
Fs=44100;
window_size=10000;
T_one=100;
T_zero=200;
alph_one=0.01;

steps=30;



[signal,fs,nbits]=wavread("Sun In The Sky.wav");
Ys=zeros(1,window_size+1);
indx_read=1;
for indx = [1:steps]
	disp(indx_read);
	disp(indx_read+window_size);
	Xs=signal(indx_read:indx_read+window_size,1)';
%	Xs=Xs';
%	save "goldentru_org" Xs;
%	Xs=Xs';
%	disp(columns(Ys)-T_one);
%	disp(columns(Ys));
%	Xs+=[Ys(columns(Ys)-T_one:columns(Ys)),zeros(1,columns(Xs)-T_one-1)];
	Xs=conv(Xs,[1;zeros(T_one-2,1);alph_one]);
%	Xs=Xs';
%	save "goldentru" Xs;
%	Xs=Xs';
	if(indx==1)
%		Ys=Xs(1:columns(Xs)-T_one+1);
		Ys=Xs(1:columns(Xs)-T_one+1);
	else
%		Ys=[Ys,Xs(1:columns(Xs)-T_one+1)];
		Ys=[Ys,Xs(1:columns(Xs)-T_one+1)];
	endif
	indx_read+=window_size;
	ctmrr= abs(ifft((log(fft(Xs))).^2));
	plot(ctmrr(1:300));
%	input("");
endfor
sound(Ys,fs,nbits);


%subplot(2,1,1);
%plot(signal(1:steps*window_size));
%subplot(2,1,2);
%plot(Ys);




%disp(rows(signal));
%disp(columns(signal));

%out=signal;
%out=zeros(rows(signal),1);
%signal=signal(1:3*window_size,1);
%plot(signal);
%sys=[0.5,zeros(1,200),0.25,zeros(1,1000),1];
%out=conv(signal,sys);
%for i = [1:rows(signal)]
%	if(i<100+1)
%		out(i,1)=signal(i,1);
%	else
%		out(i,1)=0.2*(signal(i,1)+signal(i-100,1));
%	endif
%endfor

%plot(out);

%ff=fft(out(15000:20000));
%ffmag=abs(ff);
%plot(ffmag(1:2500));
%ffc=log(ff(1:2500)).^2;
%crr= ifft(ffc);

%plot(10*log(crr));

%ftest=1000;

%signal= @(n) cos(2*pi*ftest*n/Fs);
%samp=(0:1:10*window_size);

%plot(samp,signal(samp));

%ff=fft(signal(samp)*100);
%ffmag=abs(ff);
%plot(ffmag)
%ffc=(log(ff)).^2;
%ffmag=abs(ffc);
%plot(ffmag);
%iffc=ifft(ff);
%plot(ifft(fft(iffc)));

%plot(ffmag(1:2500));
%ffc=log(ff(1:2500)).^2;
%crr= ifft(ffc);






