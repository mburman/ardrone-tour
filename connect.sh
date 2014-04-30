{
  echo "killall udhcpd; iwconfig ath0 mode managed essid rasp; ifconfig ath0 192.168.42.2 netmask 255.255.255.0 up;";
  sleep 1;
} | telnet 192.168.1.1
