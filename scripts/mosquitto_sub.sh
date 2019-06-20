#/bin/bash

trap_ctrlc ()
{
    # perform cleanup here
    echo "\n\n"
    echo "Removing docker container"
    docker kill mosquitto_sub
    echo "Cleanup done"
}

trap "trap_ctrlc" 2

docker run -d --name mosquitto_sub --rm --net=host  eclipse-mosquitto mosquitto_sub $@
docker logs -f mosquitto_sub
