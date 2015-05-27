source("sucesion.R");
puntos = 400;
c <- seq(-1.5, -1.4, length.out=puntos);
z <- matrix(data=c, nrow=puntos^2, ncol=2);
z[, 2] <- rep(seq(-0.1, 0, length.out=puntos), each=puntos);
z <- complex(real=z[, 1], imaginary=z[, 2]);
n = c(1:puntos^2);
for(i in 1:puntos^2){
	n[i] = sucesion(z[i]);
	if(!i%%1000) print(i);
}
tiff(paste('imagenes/mandelbrot_', format(Sys.time(), "%Y-%m-%d-%H-%M-%S"), '.tiff', sep=""), height = 4, width = 4, units = 'in', compression="lzw", res=300)
plot(z, col=n, pch=10)
dev.off()

