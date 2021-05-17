all_icons:
	cd icons/weather; ./convert.sh; cd ../..
	python generate_cpp_bitmap.py weather icons/weather/*bmp > src/icons/weather.bmp.h

