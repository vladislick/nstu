#!/bin/bash

# Name of the script
scriptName=`basename "$0"`

# TASK 1
# Check if it is only one of the running script
if [[ $(ps -e | grep $scriptName | wc -l) -eq 2 ]] 
then
	echo "It is only one script"
else
	echo "It is another copy of the script, aborted"
	exit
fi

# TASK 2
# Check count of interfaces 
infCount=$(ifconfig -s | wc -l);
infCount=$(( $infCount - 2 ));
echo "Available interfaces is $infCount"
if [[ $infCount -le 0 ]]; then
	echo "There is no available interfaces, aborted"
	exit
fi

# TASK 3
# Check activity of the interfaces
if [[ $(ss -t4 state established | wc -l) -eq 1 ]]; then
	echo "There is no connected network interface, aborted"
	exit
fi

# TASK 4
# Check the connection to the local network
gatewayIP=$(ip route | awk '/^default/ { print $3 }')
ping $gatewayIP -c 1 -W 1 >> /dev/null
if [[ $? -eq 0 ]]; then
	echo "There is available connection to local network"
else
	echo "There is no available connection to local network"
fi

# TASK 5
# Check the connection to the Internet HTTP
wget -q --spider http://google.com
if [[ $? -ne 0 ]]; then
	echo "There is not available Internet HTTP connection, aborted"
	exit
fi

# TASK 6
# Check the packages with tcpdump
while [ 1 ]; do
	if [[ $(tcpdump -i ens3 -c 10 | grep wifi | wc -l) -eq 0 ]]; then
		echo "$(date) ---- No packages"
	else
		echo -e "$(date)\n$(arp -a)\n" >> log.txt
		echo "$(date) ---- Find some packages, arp table logged, done"
		exit
	fi

	sleep 10;
done
