#!/bin/sh

#  Script to fetch recent asn.txt file from RIPE and recent
#  routeviews-prefix2as file from CAIDA, then update databases for
#  mccurtaind and restart mccurtaind.

ipv42asdb=/usr/local/etc/ipv42as.db
as2ipv4db=/usr/local/etc/as2ipv4.db
ipv62asdb=/usr/local/etc/ipv62as.db
as2ipv6db=/usr/local/etc/as2ipv6.db
asntxt=/usr/local/etc/asn.txt

fetch -o /tmp/asn.txt https://ftp.ripe.net/ripe/asnames/asn.txt
if [ $? -eq 0 ]; then
    mv /tmp/asn.txt ${asntxt}
fi

yesterday=`date -v -3d +%Y%m%d`
rvsubdir=`date -v -3d +%Y/%m`
rvdir="https://data.caida.org/datasets/routing/routeviews-prefix2as"
rv6dir="https://publicdata.caida.org/datasets/routing/routeviews6-prefix2as"
rvfile=`fetch -o - ${rvdir}/${rvsubdir} | grep rv2-${yesterday} | awk -F '"' '{print $2}'`
rv6file=`fetch -o - ${rv6dir}/${rvsubdir} | grep rv6-${yesterday} | awk -F '"' '{print $2}'`
if [ -n "${rvfile}" -a -n "${rv6file}" ]; then
    fetch -o /tmp/${rvfile} ${rvdir}/${rvsubdir}/${rvfile}
    if [ $? -eq 0 ]; then
	fetch -o /tmp/${rv6file} ${rv6dir}/${rvsubdir}/${rv6file}
	if [ $? -eq 0 ]; then
	    mkcurtaindb -i ${ipv42asdb} -I ${ipv62asdb} -a ${as2ipv4db} -A ${as2ipv6db} /tmp/${rvfile} /tmp/${rv6file}
	    /usr/local/etc/rc.d/mccurtaind restart
	    rm /tmp/${rv6file}
	fi
	rm /tmp/${rvfile}
    fi
fi


