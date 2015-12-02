all : kml_dist

GPP=g++

kml_dist : kml_dist.c
	$(GPP) -g -Wall -o kml_dist \
                 kml_dist.c
clean:
	-@rm kml_dist              >/dev/null 2>/dev/null
	-@rm *~                   >/dev/null 2>/dev/null
	@echo "Clean OK."
