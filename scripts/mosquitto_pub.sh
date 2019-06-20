#/bin/bash

docker run --name mosquitto_pub -it --rm --net=host eclipse-mosquitto mosquitto_pub $@