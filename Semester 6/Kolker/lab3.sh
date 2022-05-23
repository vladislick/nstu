cpuUser=$(vmstat | awk '(NR==3){print $13}')
cpuSystem=$(vmstat | awk '(NR==3){print $14}')
buff=$(vmstat | awk '(NR==3){print $5}')
cache=$(vmstat | awk '(NR==3){print $6}')
swap=$(vmstat | awk '(NR==3){print $3}')
ram=$(( $buff + $cache ))
cpu=$(( $cpuUser + $cpuSystem ))
while true; do 
	echo "CPU: $cpu, RAM: $ram, Swap: $swap"
	wget "https://api.thingspeak.com/update?api_key=GEXDY2TJWRB13PZ5&field1=$swap"
	sleep 20;
	wget "https://api.thingspeak.com/update?api_key=GEXDY2TJWRB13PZ5&field2=$cpu"
	sleep 20;	
	wget "https://api.thingspeak.com/update?api_key=GEXDY2TJWRB13PZ5&field3=$ram"
	if [[ $? -ne 0 ]]; then
		echo "Cannot send data to the server"
	fi
	sleep 20;
done
