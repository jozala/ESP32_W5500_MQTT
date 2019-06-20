#!/bin/bash

SCRIPT_DIR="$( cd "$( dirname "$0" )" && pwd )"

MQTT_HOST=$1

while true; do 
    currentDate=`date '+%Y-%m-%dT%H:%M:%S'`;
    $SCRIPT_DIR/mosquitto_pub.sh -h $MQTT_HOST -t "mydevice/listen" -m "$currentDate" -r; 
    echo "mydevice/listen <- $currentDate"; 
    sleep 5;
done