#!/bin/bash

QT_Version="5"

helpFunction()
{
	echo "-V <version> -> Please enter Qt version (ex: -V 5)"
	echo " "
}

Unknown_argument()
{
	helpFunction
	echo -e "\033[0;33m[Warning]: Unknown argument -> $1 \033[0m"
	exit 1
}

while getopts "V:h" opt
do
	case "$opt" in
		V ) QT_Version="$OPTARG" ;;
		h ) helpFunction ;;
		? ) Unknown_argument "$opt" ;;
	esac
done


LibDir=$PWD
shell_name=$0
QT_LIB_PATH=/lib/aarch64-linux-gnu/
platforms_dir=/lib/aarch64-linux-gnu/qt$QT_Version/plugins/platforms/
find_command=aarch64-linux-gnu-readelf
c_count=0

export QT_DEBUG_PLUGINS=1
export PATH=/usr/lib/qt$QT_Version/bin:$PATH
export LIB_PATH=$QT_LIB_PATH:$LIB_PATH
export PLUGIN_PATH=/lib/aarch64-linux-gnu/qt$QT_Version/plugins:$PLUGIN_PATH
export QML2_PATH=/lib/aarch64-linux-gnu/qt$QT_Version/qml:$QML2_PATH
export LD_LIBRARY_PATH=$QT_LIB_PATH

folder_array=($(ls $LibDir -l |  grep ^d | awk '{print $9}'))
#x86_64-linux-gnu-readelf -d $exefolder/$exe | grep "Shared library" | grep -o "\[.*" | sed "s/[\]]*$//g" | sed "s/^[\[]*//g"

echo Qt version: $QT_Version

if ! [ -d "/usr/lib/qt$QT_Version/bin" ]; then
    echo -e "\033[0;31m[Error]: The path not exist.(/usr/lib/qt$QT_Version/bin) \033[0m"
	helpFunction
	exit
elif ! [ -d "$QT_LIB_PATH" ]; then
	echo -e "\033[0;31m[Error]: The path not exist.($QT_LIB_PATH) \033[0m"
	helpFunction
	exit
elif ! [ -d "$platforms_dir" ]; then
	echo -e "\033[0;31m[Error]: The path not exist.($platforms_dir) \033[0m"
	helpFunction
	exit
fi

function find_lib_recursion() {
	more_lower_lib_array=($($find_command -d $1/$2 | grep "Shared library" | grep -o "\[.*" | sed s/[\]]*$//g | sed s/^[\[]*//g))
	for lower_lib in ${more_lower_lib_array[@]}
	do
		if [ -f $1/$lower_lib ]; then
			if ! [ -f $3/$lower_lib ]; then
				echo $1/$lower_lib
				cp $1/$lower_lib $3
				find_lib_recursion $1 $lower_lib $3
			fi
		fi
	done
}

for folder in ${folder_array[@]}
do
	pro_array=($(ls $folder | grep "CMakeLists.txt" | grep -v ".user"))
	cd $folder
	for pro in ${pro_array[@]}
	do
		c_count=$(($c_count+1))
		$(mkdir "RPI_build")
		/usr/bin/cmake -S ./ -B ./RPI_build '-DCMAKE_GENERATOR:STRING=Unix Makefiles' -DCMAKE_BUILD_TYPE:STRING=Release -DQT_QMAKE_EXECUTABLE:FILEPATH=qmake -DCMAKE_PREFIX_PATH:PATH=/usr -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/aarch64-linux-gnu-gcc -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/aarch64-linux-gnu-g++
		cmake --build ./RPI_build
		
		exe_array=($(ls -F ./RPI_build | grep "*" | sed s/[*]*$//g ))
		for exe in ${exe_array[@]}
		do
			exefolder="../"$folder"_build_deploy_linux64"
			$(mkdir $exefolder)
			$(mkdir $exefolder"/platforms")
			cp ./RPI_build/$exe $exefolder
			platforms_qt=($(ls $platforms_dir | grep ".so"))
			lib_array=($($find_command -d $exefolder/$exe | grep "Shared library" | grep -o "\[.*" | sed s/[\]]*$//g | sed s/^[\[]*//g))
			for lib in ${lib_array[@]}
			do
				if [ -f $QT_LIB_PATH/$lib ]; then
					echo $QT_LIB_PATH/$lib
					cp $QT_LIB_PATH/$lib $exefolder
					find_lib_recursion $QT_LIB_PATH $lib $exefolder
				fi
			done
			for pf_lib in ${platforms_qt[@]}
			do
				cp $platforms_dir$pf_lib $exefolder"/platforms/"
				pf_qt_libarray=($($find_command -d $platforms_dir$pf_lib | grep "Shared library" | grep -o "\[.*" | sed s/[\]]*$//g | sed s/^[\[]*//g))
				echo $platforms_dir$pf_lib
				for pf_qt_lib in ${pf_qt_libarray[@]}
				do
					if [ -f $QT_LIB_PATH/$pf_qt_lib  ]; then
						echo $QT_LIB_PATH/$pf_qt_lib
						cp $QT_LIB_PATH/$pf_qt_lib $exefolder
						find_lib_recursion $QT_LIB_PATH $pf_qt_lib $exefolder
					fi
				done
			done
			

		done
		rm -rf RPI_build
	done
	cd "../"
	
done

if [ "$c_count" = "0" ]
then
    echo -e "\033[0:33m[Warning]: CMakeLists.txt not found \033[0m"
fi


