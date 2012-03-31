#include <stdlib.h>
#include <stdio.h>

int main()
{
	system("ifconfig wlan0 down");
	system("iwconfig wlan0 mode monitor");
	system("ifconfig wlan0 up");
	system("./airodump-ng -f 2000 -t 10000 --channel 1,6,11 -w locationprint wlan0");
return 0;
}

