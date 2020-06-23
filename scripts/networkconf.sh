#!/bin/bash

sudo systemctl stop firewalld.service

sudo ip link set dev enp2s0 address 44:03:2c:fc:c5:c8
sudo ip link set dev enp2s0 up

#sudo ifconfig enp2s0 192.168.10.1 netmask 255.255.255.0 up
sudo ip addr add 192.168.10.1/24 dev enp2s0

sudo iptables -t nat -A POSTROUTING -o wlp3s0 -j MASQUERADE
sudo iptables -A FORWARD -i enp2s0 -o wlp3s0 -j ACCEPT
sudo iptables -A FORWARD -i wlp3s0 -o enp2s0 -m state --state RELATED,ESTABLISHED -j ACCEPT

sudo systemctl start isc-dhcp-server