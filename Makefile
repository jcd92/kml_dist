all : kml_dist

GPP=g++
GCC=gcc

kml_dist : kml_dist.cpp
	$(GPP) -g -Wall -o kml_dist \
                 kml_dist.cpp
clean:
	-@rm kml_dist              >/dev/null 2>/dev/null
	-@rm *~                   >/dev/null 2>/dev/null
	@echo "Clean OK."
