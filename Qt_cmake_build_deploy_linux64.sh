#!/bin/bash

QT_PATH=$HOME"/Qt"
QT_Version="5.15.2"

helpFunction()
{
	echo "-P <path> -> Please enter Qt path (ex: -P $HOME"/Qt")"
	echo " "
	echo "-V <version> -> Please enter Qt version (ex: -V 5.15.2)"
	echo " "
}

Unknown_argument()
{
	helpFunction
	echo -e "\033[0;33m[Warning]: Unknown argument -> $1 \033[0m"
	exit 1
}

while getopts "P:V:h" opt
do
	case "$opt" in
		P ) QT_PATH="$OPTARG" ;;
		V ) QT_Version="$OPTARG" ;;
		h ) helpFunction ;;
		? ) Unknown_argument "$opt" ;;
	esac
done


LibDir=$PWD
shell_name=$0
QT_LIB_PATH=$QT_PATH/$QT_Version/gcc_64/lib
platforms_dir=$QT_PATH/$QT_Version/gcc_64/plugins/platforms/
find_command=x86_64-linux-gnu-readelf
c_count=0

export QT_DEBUG_PLUGINS=1
export PATH=$QT_PATH/$QT_Version/gcc_64/bin:$QT_PATH/Tools/CMake/bin:$QT_PATH/Tools/Ninja:$PATH
export LIB_PATH=$QT_LIB_PATH:$LIB_PATH
export PLUGIN_PATH=$QT_PATH/$QT_Version/gcc_64/plugins:$PLUGIN_PATH
export QML2_PATH=$QT_PATH/$QT_Version/gcc_64/qml:$QML2_PATH
export LD_LIBRARY_PATH=$QT_LIB_PATH

folder_array=($(ls $LibDir -l |  grep ^d | awk '{print $9}'))
#x86_64-linux-gnu-readelf -d $exefolder/$exe | grep "Shared library" | grep -o "\[.*" | sed "s/[\]]*$//g" | sed "s/^[\[]*//g"

echo Qt path: $QT_PATH
echo Qt version: $QT_Version

if ! [ -d "$QT_PATH/$QT_Version/gcc_64/bin" ]; then
    echo -e "\033[0;31m[Error]: The path not exist.($QT_PATH/$QT_Version/gcc_64/bin) \033[0m"
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
		$(mkdir "linux_build")
		cmake -S ./ -B ./linux_build -DCMAKE_GENERATOR:STRING=Ninja -DCMAKE_BUILD_TYPE:STRING=Release -DQT_QMAKE_EXECUTABLE:FILEPATH=$QT_PATH/$QT_Version/gcc_64/bin/qmake -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/gcc -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/g++
		cmake --build ./linux_build
		
		exe_array=($(ls -F ./linux_build | grep "*" | sed s/[*]*$//g ))
		for exe in ${exe_array[@]}
		do
			exefolder="../"$folder"_build_deploy_linux64"
			$(mkdir $exefolder)
			$(mkdir $exefolder"/platforms")
			cp ./linux_build/$exe $exefolder
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
		rm -rf linux_build
	done
	cd "../"
	
done

if [ "$c_count" = "0" ]
then
    echo -e "\033[0:33m[Warning]: CMakeLists.txt not found \033[0m"
fi


