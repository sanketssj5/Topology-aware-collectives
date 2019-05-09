#!/bin/bash
#following code will populate the hostfile with working hosts
#clear the contents of the hostfile
truncate -s 0 ./hostfile

my_ip=$(hostname -I)
#remove last variable from bash
my_ip=${my_ip%?}
printf "$my_ip:8" >> ./hostfile

#arr contains list of ip addresses
arr=(172.27.19.2 172.27.19.3 172.27.19.4 172.27.19.5 172.27.19.6 172.27.19.7 172.27.19.8 172.27.19.10 172.27.19.11 172.27.19.12 172.27.19.13 172.27.19.15 172.27.19.16 172.27.19.17 172.27.19.18 172.27.19.20 172.27.19.21 172.27.19.22 172.27.19.23 172.27.19.24 172.27.19.27 172.27.19.30)

#myip + 15 other ips = 16
limit=15

for i in "${arr[@]}"
do
	#check if the ip is reachable or not
	if ping -c1 -w3 $i> /dev/null 2>&1 
	then 
		if [ $i != $my_ip ] #check if this ip is not me
		then
		printf "\n$i:8" >> ./hostfile
		let "limit--"
		fi
	fi
	if [ $limit == 0 ]
	then
		break
	fi
done
