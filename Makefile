all:
	pio -f -c vim run

upload:
	pio -f -c vim run --target upload

clean:
	pio -f -c vim run --target clean

program:
	pio -f -c vim run --target program

uploadfs:
	pio -f -c vim run --target uploadfs

reload:
	pio -f -c vim init

monitor:
	pio -f -c vim device monitor

update:
	pio -f -c vim update

