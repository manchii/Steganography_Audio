file=csvread("../resultados_data.csv",1,0);
SONG=1;
AMP=2;
DELAY=3;
WINDOW=4;
ODG=5;
ERROR=6;

%scatter(file(:,ERROR),file(:,ODG),'filled');
%title('Error vs ODG');
%xlabel('Porcentaje de error');
%ylabel('ODG PEAQ');

%print (hf, "errvsodg.pdf", "-dpdflatexstandalone");
amp=[];
delay=[];
error_g=[];
count=1;
for row=(1:80*3)
	if(file(row,WINDOW)==1000)
		amp(count)=file(row,AMP);
		delay(count)=file(row,DELAY);
		error_g(count)=file(row,ERROR);
		count++;
	endif
endfor


scatter3(amp(1:count/3),delay(1:count/3),error_g(1:count/3),[],1*ones(1,1:count/3),'filled'); hold on
scatter3(amp(1+count/3:count*2/3),delay(1+count/3:count*2/3),error_g(1+count/3:count*2/3),[],3*ones(1,1:80),'filled'); hold on
scatter3(amp(1+count*2/3:count-1),delay(1+count*2/3:count-1),error_g(1+count*2/3:count-1),[],6*ones(1,1:80),'filled');
xlabel('Porcentaje de atenuación');
ylabel('Retardo del 1 lógico (us)');
zlabel('Porcentaje de error');
title('Comportamiento de tres canciones con ventana de 1000 muestras');
