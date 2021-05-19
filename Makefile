weather_icons:
	cd icons/weather; ./convert.sh; cd ../..
	python generate_cpp_bitmap.py weather_bmp icons/weather/*bmp > src/icons/weather.bmp.h

ALL: weather_icons
