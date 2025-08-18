# Mozart 2 Docker support

This docker support is designed to install the required tools configured with the good versions easily and separately from the host system.
Doing so Mozart2 can be compiled inside the container but installed on the host thanks to the generated binary.

## Start the container

```bash
sudo docker compose -f ubuntu-system.yml build
sudo docker compose -f ubuntu-system.yml run --rm ubuntu-system
```

## Start the container and allow X11 accesses (linux)

```bash
xhost +local:docker
sudo docker compose -f ubuntu-system.yml run --rm -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix ubuntu-system
```