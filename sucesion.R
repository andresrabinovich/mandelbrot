sucesion <- function(c, r_max = 2, control = 500){
	paso = 0;
	z_n = c;
	z_n_mas_uno = 0;
	while(Mod(z_n_mas_uno) < r_max && paso < control && z_n_mas_uno != z_n){
		z_n = z_n_mas_uno;
		z_n_mas_uno = z_n^2 + c;
		paso <- paso + 1;
	}
	if(Mod(z_n_mas_uno) < r_max) paso = control;
	return (paso);
}
