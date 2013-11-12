#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
	int iaux = 0;
	struct termios pattr;
	memset(&pattr, 0, sizeof(pattr));

	int icom = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY | O_NDELAY);
	if (icom < 0) {
		printf("No se pudo abrir la uart.\r\n");
		return -1;
	}

	// Se configura la estructura de control del puerto serie
	if (tcgetattr(icom, &pattr) < 0) {
		printf("No se pudo leer la configuración actual de puerto.\r\n");
		close(icom);
		return -1;
	}

	pattr.c_cflag &= ~PARENB;	// Paridad ninguna
	pattr.c_cflag &= ~CSTOPB;	// 1 Bit parada
	pattr.c_cflag |= CLOCAL | CREAD;	// No es un terminal, lectura
	pattr.c_cflag &= ~CSIZE;	// 8 bit datos
	pattr.c_cflag |= CS8;
	pattr.c_cflag &= ~CBAUD;	// 9600 baudios
	pattr.c_cflag |= B9600;
	cfsetispeed(&pattr, B9600);
	cfsetospeed(&pattr, B9600);

	// Se desactivan las opciones de consola
	pattr.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	pattr.c_oflag &= ~OPOST;

	// Control de flujo no
	pattr.c_iflag &= ~(IXON | IXOFF | IXANY);
	pattr.c_cflag &= ~CRTSCTS;

	// Paridad de entrada ninguna
	pattr.c_iflag &= ~(INPCK | ISTRIP);

	// No mapear CR a NL
	pattr.c_iflag &= ~ICRNL;

	// Mínimo de caracters a leer
	pattr.c_cc[VMIN] = 0;
	pattr.c_cc[VTIME] = 0;

	// Configurar puerto
	if (tcsetattr(icom, TCSANOW, &pattr) == -1) {
		printf("No se pudo configurar la UART\r\n");
		close(icom);
		return -1;
	}

	char buffer[1001];
	while (1) {
		usleep(1000);
		int leido = read(icom, buffer, 1000);
		if (leido < 0) break;
		if (leido == 0) continue;

		buffer[leido] = 0;
		printf("%s\r\n", buffer);
	}

	printf("Fin\r\n");
	close(icom);
	return 0;
}
