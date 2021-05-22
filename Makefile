bitmap_test:
	g++ misc/bitmap_test.cpp -I. -o bmp_test -g -Wall

weather_icons:
	cd icons/weather; ./convert.sh; cd ../..
	python generate_cpp_bitmap.py weather_bmp icons/weather/*bmp > src/icons/weather.bmp.h

generic_icons:
	rm -f icons/generic/*bmp
	cd icons/; find generic/ -iname "*svg" -exec ./svg-to-bmp.sh 32 {} \;
	cd icons/; find generic/ -iname "*svg" -exec ./svg-to-bmp.sh 24 {} \;
	python generate_cpp_bitmap.py generic_bmp icons/generic/*bmp > src/icons/generic.bmp.h

all_icons: weather_icons generic_icons
